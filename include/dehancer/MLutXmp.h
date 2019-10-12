//
// Created by denn nevera on 2019-07-18.
//
#pragma once

#include <exiv2/exiv2.hpp>
#include <optional>
#include <string>
#include <vector>
#include <map>

#include "dehancer/Common.h"
#include "dehancer/Deferred.h"
#include "dehancer/Blowfish.h"

namespace dehancer {

    class MLutXmp {

    public:

        using CLutBuffer = std::vector<std::uint8_t>;

    public:

        /**
         * Open xmp-mlut-file
         * @param path - file path
         * @param key - pass key, by default empty means properties read only
         * @return
         */
        static dehancer::expected<MLutXmp,Error> Open(const std::string &path, const Blowfish::KeyType& key);
        static dehancer::expected<MLutXmp,Error> Open(const std::string &path);

        Exiv2::Value::UniquePtr get_value(const std::string &key) const ;
        const std::vector<std::string> & get_key_list() const;

        time_t get_datetime() const ;
        int get_revision() const ;
        std::string get_name() const ;
        std::string get_caption() const ;
        std::string get_description() const ;
        std::string get_tags() const ;
        std::string get_author() const ;
        std::string get_maintainer() const ;
        const int   get_ISO_index() const ;
        const int   get_expand_mode() const;
        const float get_expand_impact() const;
        const std::vector<CLutBuffer>& get_cluts() const ;

        MLutXmp(const MLutXmp& other);
        ~MLutXmp();

    private:
        MLutXmp():path_(){};
        std::string path_;
        std::map<std::string, Exiv2::Value::UniquePtr> meta_;
        std::vector<CLutBuffer> cluts_;
    };
}
