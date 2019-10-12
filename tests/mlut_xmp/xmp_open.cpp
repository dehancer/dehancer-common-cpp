//
// Created by denn nevera on 2019-07-18.
//

#include "dehancer/MLutXmp.h"


#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>

TEST(XMP, XMPOpenTest) {

  std::cout << std::endl;

  std::string file_path = "../../../tests/mlut_xmp/mlut.mlut";

  std::cout << "Open test: " << file_path << std::endl;

  /*
   * * read properties
   * */

  auto xmp = dehancer::MLutXmp::Open(file_path);

  EXPECT_TRUE(xmp);

  if (xmp) {

    auto list = xmp->get_key_list();
    std::cout << std::endl;

    std::for_each(list.begin(), list.end(), [&xmp](const std::string &key) {
        auto value = xmp->get_value(key);

        if (value) {
          if (value->typeId() == Exiv2::TypeId::string || value->typeId() == Exiv2::TypeId::xmpText)
            std::cout << " mlut_xmp key: " << key << " = " << value->toString() << std::endl;
          else if (value->typeId() == Exiv2::TypeId::signedLong || value->typeId() == Exiv2::TypeId::unsignedLong)
            std::cout << " mlut_xmp key: " << key << " = " << value->toLong() << std::endl;
          else {
            std::cout << " mlut_xmp key: " << key << " type:  " << std::hex << value->typeId() << std::endl;
          }
        }
    });

    EXPECT_TRUE(xmp->get_cluts().size() == 3);

    for (int i = 0; i < 3; i++) {
      auto data = xmp->get_cluts()[i];

      std::ofstream outFile;

      std::string file = "./";
      xmp->get_name();
      file.append(xmp->get_name());
      file.append("[");
      file.append(std::to_string(i));
      file.append("].png");

      outFile.open(file, std::fstream::out | std::ofstream::binary);

      std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(outFile));

      auto str = std::string(data.begin(), data.end());

      std::cout << " mlut_xmp luts: " << xmp->get_cluts().size() << "  >> " << std::endl;
      std::copy(data.begin(), data.end(), std::ostream_iterator<uint16_t>(std::cout, " "));

      std::cout << std::endl;
    }

  } else {
    std::cerr << "Error: " << xmp.error().message() << std::endl;
  }
}