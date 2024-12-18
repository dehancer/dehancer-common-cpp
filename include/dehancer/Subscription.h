//
// Created by Dmitry Sukhorukov on 30.10.2024.
//

#pragma once

#include <string>
#include <cinttypes>
#include <chrono>
#include "dehancer/Common.h"

namespace dehancer {

    struct Subscription {

        std::string title;
        std::string subscription_id;
        std::uint16_t seats_count{};
        std::uint16_t activated_count{};
        std::time_t expires_at{};
        std::time_t last_checked{};
        std::uint16_t offline_days;
        bool cancel_at_period_end;

        bool is_current{};

        Subscription();

        Subscription(const Subscription &subscription);

        Subscription &operator=(const Subscription &right);

        Error sign(const std::string &pvk);

        static expected<Subscription, Error> Decode(const std::string &base64);

        static std::string Encode(const Subscription &subscription, bool line_break_enabled = true);

        static expected<Subscription, Error> from_json(const json &json);

        [[nodiscard]] dehancer::json json() const;

        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] bool is_offline_exceeded() const;

        [[nodiscard]] const std::string &get_signature() const;

        void update_pk(const std::string &);

    private:
        std::string pk_;
        std::string signature_;
    };
}
