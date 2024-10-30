//
// Created by denn nevera on 2019-08-10.
//

#include "dehancer/Subscription.h."

namespace dehancer {

    Subscription::Subscription()
    : subscription_id(""),
    seats_count(0),
    activated_count(0),
    is_current(false)
    {

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

    expected<Subscription, Error> License::from_json(const dehancer::json &_json) {
        try {
            Subscription s;

            s.subscriptin_id = _json.at("subscriptionId").get<std::string>();
            s.seats_count =  _json.at("seatsCount").get<std::uint16_t>();
            s.activated_count =  _json.at("activatedCount").get<std::uint16_t>();
            s.is_current = _json.at("isCurrent").get<std::bool>();

            return s;
        }
        catch (const std::exception &e) {
            return make_unexpected(Error(CommonError::PARSE_ERROR, e.what()));
        }
        catch (...) {
            return make_unexpected(Error(CommonError::PARSE_ERROR, "Subscription text could not be decoded..."));
        }
    }

    dehancer::json License::json() const {
        dehancer::json data = {
                {"subscriptionId",  static_cast<std::string>(subscription_id)},
                {"seatsCount",      static_cast<uint16_t>(seats_count)},
                {"activatedCount",  static_cast<uint16_t>(activated_count)},
                {"isCurrent",      static_cast<std::bool>(is_current)},
        };

        return data;
    }
}