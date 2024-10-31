//
// Created by denn nevera on 2019-08-10.
//

#include "dehancer/Subscription.h"
#include "dehancer/Base64.h"
#include "dehancer/Utils.h"
#include <ed25519.hpp>
#include <climits>

namespace dehancer {

    std::string Subscription::authority_key;
    std::string Subscription::authority_public_key;

    static inline auto make_digest(const Subscription& subscription) {
        return ed25519::Digest([subscription](auto &calculator){
            calculator.append(subscription.subscription_id);
            calculator.append(static_cast<uint16_t>(subscription.seats_count));
            calculator.append(static_cast<uint16_t>(subscription.activated_count));
            calculator.append(static_cast<bool>(subscription.is_current));
        });
    }

    Subscription::Subscription()
            : subscription_id(""),
              seats_count(0),
              activated_count(0),
              is_current(false) {

    }

    Subscription::Subscription(const dehancer::Subscription &s) {
        *this = s;
    }

    Subscription &Subscription::operator=(const dehancer::Subscription &s) {
        subscription_id = s.subscription_id;
        seats_count = s.seats_count;
        activated_count = s.activated_count;
        is_current = s.is_current;
        return *this;
    }

    expected <Subscription, Error> Subscription::from_json(const dehancer::json &_json) {
        try {
            Subscription s;

            s.subscription_id = _json.at("subscriptionId").get<std::string>();
            s.seats_count = _json.at("seatsCount").get<std::uint16_t>();
            s.activated_count = _json.at("activatedCount").get<std::uint16_t>();
            s.is_current = _json.at("isCurrent").get<bool>();

            return s;
        }
        catch (const std::exception &e) {
            return make_unexpected(Error(CommonError::PARSE_ERROR, e.what()));
        }
        catch (...) {
            return make_unexpected(Error(CommonError::PARSE_ERROR, "Subscription text could not be decoded..."));
        }
    }

    dehancer::json Subscription::json() const {
        dehancer::json data = {
                {"subscriptionId", static_cast<std::string>(subscription_id)},
                {"seatsCount",     static_cast<uint16_t>(seats_count)},
                {"activatedCount", static_cast<uint16_t>(activated_count)},
                {"isCurrent",      static_cast<bool>(is_current)},
        };

        return data;
    }

    std::string Subscription::Encode(const Subscription &subscription, bool line_break_enabled) {
        std::string base64;
        base64::encode(subscription.json().dump(), base64, line_break_enabled ? 76 : INT_MAX);
        return base64;
    }

    expected <Subscription, Error> Subscription::Decode(const std::string &base64_in) {

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
}