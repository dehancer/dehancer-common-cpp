//
// Created by Dmitry Sukhorukov on 30.10.2024.
//

#pragma once

#include <string>
#include <cinttypes>
#include "dehancer/Common.h"

namespace dehancer {

    struct Subscription {

        std::string subscription_id;
        std::uint16_t seats_count{};
        std::uint16_t activated_count{};
        bool is_current{};

        Subscription();

        Subscription(const Subscription &subscription);

        Error sign();

        static expected <Subscription, Error> Decode(const std::string &base64);

        static std::string Encode(const Subscription &subscription, bool line_break_enabled = true);

        static expected <Subscription, Error> from_json(const json &json);

        [[nodiscard]] dehancer::json json() const;

        Subscription &operator=(const Subscription &right);

    public:
        static std::string authority_key;
        static std::string authority_public_key;
    private:
        std::string signature_;
    };
}
