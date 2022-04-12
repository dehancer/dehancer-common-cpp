//
// Created by denn nevera on 2019-08-10.
//

#include "dehancer/License.h"
#include "dehancer/Utils.h"
#include "dehancer/Log.h"
#include "gtest/gtest.h"
#include "ed25519.hpp"

TEST(License, LicenseTest) {

  auto pr = ed25519::keys::Pair::Random();

  dehancer::License::authority_key = pr->get_private_key().encode();
  dehancer::License::authority_public_key = pr->get_public_key().encode();

  std::cout << std::endl;

  auto lic = dehancer::License();

  lic.type = dehancer::License::Type::promo;
  lic.expiry_date = dehancer::time::now_utc() + 3600 * 2;
  lic.issue_date = dehancer::time::now_utc();
  lic.name = "Test license";
  lic.maintainer = "Dehancer Ofx";
  lic.email = "some.user@gmail.com";
  lic.host  = "Final Cut Pro";

  auto error = lic.sign();

  EXPECT_TRUE(!error);
  
  dehancer::log::error(error, "License error: %s", error.message().c_str());
  
  if (error) {
    std::cout << "License error: " << error << std::endl;
  }

  std::cout << "License: " << lic.json().dump(4) << std::endl;
  
  dehancer::log::print("License: %s", lic.json().dump().c_str());
  
  auto encoded = dehancer::License::Encode(lic);
  auto encoded_line = dehancer::License::Encode(lic, false);

  std::cout << "License encode: \n" << encoded << " : " << encoded.size() << std::endl;
  std::cout << "License encode line: \n" <<  encoded_line << " : " << encoded_line.size() << std::endl;

  auto lic2 = dehancer::License::Decode("  "+encoded_line+" ");

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
