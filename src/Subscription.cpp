//
// Created by denn nevera on 2019-08-10.
//

#include "dehancer/Subscription.h"
#include "dehancer/Base64.h"
#include "dehancer/Utils.h"
#include <ed25519.hpp>
#include <climits>
#include "dehancer/obfy/instr.h"

namespace dehancer {

    static inline auto make_digest(const Subscription &subscription) {
        return ed25519::Digest([subscription](auto &calculator) {
            calculator.append(subscription.subscription_id);
            calculator.append(static_cast<uint16_t>(subscription.seats_count));
            calculator.append(static_cast<uint16_t>(subscription.activated_count));
            calculator.append(std::to_string(subscription.expires_at));
            calculator.append(static_cast<uint16_t>(subscription.offline_days));
            calculator.append(static_cast<bool>(subscription.is_current));
            calculator.append(static_cast<bool>(subscription.cancel_at_period_end));
            calculator.append(subscription.email);
        });
    }

    Subscription::Subscription()
            : seats_count(0),
              activated_count(0),
              is_current(false),
              expires_at(0),
              last_checked(0),
              offline_days(0),
              cancel_at_period_end(true) {
    }

    Subscription::Subscription(const dehancer::Subscription &s) {
        *this = s;
    }

    Subscription &Subscription::operator=(const dehancer::Subscription &s) {
        title = s.title;
        subscription_id = s.subscription_id;
        seats_count = s.seats_count;
        activated_count = s.activated_count;
        expires_at = s.expires_at;
        last_checked = s.last_checked;
        is_current = s.is_current;
        signature_ = s.signature_;
        cancel_at_period_end = s.cancel_at_period_end;
        offline_days = s.offline_days;
        pk_ = s.pk_;
        email = s.email;
        return *this;
    }

    void Subscription::update_pk(const std::string &pk) {
        pk_ = pk;
    }

    expected<Subscription, Error> Subscription::from_json(const dehancer::json &_json) {
        Subscription s;

        s.title = _json["title"].get<std::string>();
        s.subscription_id = _json["subscriptionId"].get<std::string>();
        s.seats_count = _json["seatsCount"].get<std::uint16_t>();
        s.activated_count = _json["activatedCount"].get<std::uint16_t>();
        s.offline_days = _json["offlineDays"].get<std::uint16_t>();
        s.expires_at = _json["expiresAt"].get<std::time_t>();
        s.is_current = _json["isCurrent"].get<bool>();
        s.cancel_at_period_end = _json["cancelAtPeriodEnd"].get<bool>();
        s.last_checked = _json["lastChecked"].get<std::time_t>();

        // FIXME what is signature?
        // s.signature_ = _json["signature"].get<std::string>();

        return s;
    }

    dehancer::json Subscription::json() const {
        dehancer::json data = {
                {"title",             title},
                {"subscriptionId",    subscription_id},
                {"seatsCount",        static_cast<uint16_t>(seats_count)},
                {"activatedCount",    static_cast<uint16_t>(activated_count)},
                {"expiresAt",         static_cast<std::time_t>(expires_at)},
                {"lastChecked",       static_cast<std::time_t>(last_checked)},
                {"offlineDays",       static_cast<uint16_t>(offline_days)},
                {"cancelAtPeriodEnd", static_cast<bool>(cancel_at_period_end)},
                {"isCurrent",         static_cast<bool>(is_current)},
                {"email",             email},
                {"signature",         signature_}
        };

        return data;
    }

    Error Subscription::sign(const std::string &pvk) {
        OBF_BEGIN

            if (!signature_.empty())
                return Error(CommonError::PERMISSIONS_ERROR, "License has already been signed...");

            if (pvk.empty())
                return Error(CommonError::PERMISSIONS_ERROR, "License could not be signed on client ... ");

            auto digest = make_digest(*this);

            auto pair = ed25519::keys::Pair::FromPrivateKey(pvk);

            auto signature = pair->sign(digest);

            if (!signature) {
                return Error(CommonError::SECURITY_ISSUE, "Subscription could not be signed...");
            }

            signature_ = signature->encode();

            RETURN(Error(CommonError::OK));

        OBF_END
    }

    std::string Subscription::Encode(const Subscription &subscription, bool line_break_enabled) {
        std::string base64;
        base64::encode(subscription.json().dump(), base64, line_break_enabled ? 76 : INT_MAX);
        return base64;
    }

    expected<Subscription, Error> Subscription::Decode(const std::string &base64_in) {

        try {

            if (base64_in.empty())
                return make_unexpected(Error(CommonError::NOT_FOUND, "Inactive subscription"));

            std::string base64(trim(base64_in));

            if (base64.empty())
                return make_unexpected(Error(CommonError::NOT_FOUND, "Inactive subscription"));

            std::string buffer;

            base64::decode(base64, buffer);

            dehancer::json json_data = json::parse(buffer);

            return Subscription::from_json(json_data);

        }
        catch (std::exception &e) {
            return make_unexpected(Error(CommonError::PARSE_ERROR, "Subscription text could not be decoded..."));
        }
        catch (...) {
            return make_unexpected(Error(CommonError::PARSE_ERROR, "Subscription text could not be decoded..."));
        }
    }

    bool Subscription::is_valid() const {
        OBF_BEGIN

            auto digest = make_digest(*this);

            auto pk = ed25519::keys::Public::Decode(pk_);

            if (!pk) return false;

            auto signature = ed25519::Signature::Decode(signature_);

            if (!signature) return false;

            RETURN(signature->verify(digest, *pk));

        OBF_END
    }

    bool Subscription::is_offline_exceeded() const {
        if (offline_days == 0) {
            return false;
        }

        const auto days_from_last_check = (std::chrono::seconds(std::time(nullptr)).count()
                                           - last_checked) / 86400;
        return days_from_last_check > offline_days;
    }

    const std::string &Subscription::get_signature() const { return signature_; }


}
