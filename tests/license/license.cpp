//
// Created by denn nevera on 2019-08-10.
//

//#include "authority_pvk.h"
//#include "key_file.h"

#include "dehancer/License.h"
#include "dehancer/Utils.h"
#include "gtest/gtest.h"
//#include "../dotenv/dotenv_utils.h"


TEST(License, LicenseTest) {

    dehancer::License::authority_key = ""; //__DEHANCERD_AUTHORITY_PVK__;

    std::cout << std::endl;

    auto lic = dehancer::License();

    lic.type = dehancer::License::Type::promo;
    lic.expiry_date = dehancer::time::now_utc() + 3600 * 2;
    lic.issue_date = dehancer::time::now_utc();
    lic.name = "Test license";
    lic.maintainer = "Dehancer Ofx";
    lic.email = "some.user@gmail.com";

    auto error = lic.sign();

    EXPECT_TRUE(!error);

    if (error) {
        std::cout << "License error: " << error << std::endl;
    }

    std::cout << "License: " << lic.json().dump() << std::endl;

    auto encoded = dehancer::License::Encode(lic);

    std::cout << "License encode: \n" << encoded << " : " << encoded.size() << std::endl;

    auto lic2 = dehancer::License::Decode("  "+encoded+" ");

    EXPECT_TRUE(lic2);

    if (!lic2) {
        std::cerr << "License decode error: " << lic2.error().message() << std::endl;
        return;
    }

    std::cout << "License2["<<lic2->is_valid()<<"]: " << lic2->json().dump() << std::endl;

    EXPECT_TRUE(lic2->is_valid());

    lic2->name = "Bad";

    std::cout << "License2["<<lic2->is_valid()<<"]: " << lic2->json().dump() << std::endl;

    EXPECT_TRUE(!lic2->is_valid());

    auto lic3 = dehancer::License(lic);

    EXPECT_TRUE(lic3.is_valid());

}
