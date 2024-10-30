//
// Created by Dmitry Sukhorukov on 30.10.2024.
//

#pragma once

namespace dehancer {

    struct Subscription {

        std::string subscription_id;
        std::uint16_t seats_count;
        std::uint16_t activated_count;
        bool is_current;

        Subscription();

        Subscription(const Subscription& subscription);

        static expected<Subscription,Error> from_json(const json& json);

        [[nodiscard]] dehancer::json json() const;

        Subscription& operator=(const Subscription& right);
    };
}
