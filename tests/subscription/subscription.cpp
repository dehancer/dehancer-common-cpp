//
// Created by Dmitry Sukhorukov on 02.11.2024.
//

#include "dehancer/Subscription.h"
#include "dehancer/Utils.h"
#include "dehancer/Log.h"
#include "gtest/gtest.h"
#include "ed25519.hpp"

TEST(License, LicenseTest) {

    auto pr = ed25519::keys::Pair::Random();

    dehancer::Subscription::authority_key = pr->get_private_key().encode();
    dehancer::Subscription::authority_public_key = pr->get_public_key().encode();

    std::cout << std::endl;

    auto subs = dehancer::Subscription();

    subs.subscription_id = "qwerty";
    subs.seats_count = 3;
    subs.activated_count = 1;
    subs.is_current = false;

    auto error = subs.sign();

    EXPECT_TRUE(!error);

    dehancer::log::error(error, "Subscription error: %s", error.message().c_str());

    if (error) {
        std::cout << "Subscription error: " << error << std::endl;
    }

    std::cout << "Subscription: " << subs.json().dump(4) << std::endl;

    dehancer::log::print("Subscription: %s", subs.json().dump().c_str());

    auto encoded = dehancer::Subscription::Encode(subs);
    auto encoded_line = dehancer::Subscription::Encode(subs, false);

    std::cout << "Subscription encode: \n" << encoded << " : " << encoded.size() << std::endl;
    std::cout << "Subscription encode line: \n" <<  encoded_line << " : " << encoded_line.size() << std::endl;

    auto subs2 = dehancer::Subscription::Decode("  "+encoded_line+" ");

    EXPECT_TRUE(subs2);

    if (!subs2) {
        std::cerr << "Subscription decode error: " << subs2.error().message() << std::endl;
        return;
    }

    std::cout << "Subscription2["<<subs2->is_valid()<<"]: " << subs2->json().dump() << std::endl;

    EXPECT_TRUE(subs2->is_valid());

    subs2->is_current = !subs2->is_current;
    
    std::cout << "Subscription2["<<subs2->is_valid()<<"]: " << subs2->json().dump() << std::endl;

    EXPECT_TRUE(!subs2->is_valid());

    auto lic3 = dehancer::Subscription(subs);

    EXPECT_TRUE(lic3.is_valid());

}
