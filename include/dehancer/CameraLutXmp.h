//
// Created by denn nevera on 2019-10-12.
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
#include "dehancer/License.h"

namespace dehancer {

    class CameraLutXmp {

    public:

        using CLutBuffer = std::vector<std::uint8_t>;

    public:


        static dehancer::expected<CameraLutXmp,Error> Create(const std::string &buffer, const Blowfish::KeyType& key);
        static dehancer::expected<CameraLutXmp,Error> Create(const std::string &buffer);

        /**
        * Open xmp-mlut-file
        * @param path - file path
        * @param key - pass key, by default empty means properties read only
        * @return
        */
        static dehancer::expected<CameraLutXmp,Error> Open(const std::string &path, const Blowfish::KeyType& key);
        static dehancer::expected<CameraLutXmp,Error> Open(const std::string &path);

        Exiv2::Value::UniquePtr get_value(const std::string &key) const ;
        const std::vector<std::string> & get_key_list() const;

        time_t get_datetime() const ;
        int get_revision() const ;
        int get_order() const ;
        bool is_photo_enabled() const ;
        bool is_video_enabled() const ;
        bool is_published() const ;
        std::string get_name() const ;
        std::string get_vendor() const ;
        std::string get_model() const ;
        std::string get_format() const ;
        std::string get_caption() const ;
        std::string get_description() const ;
        std::string get_tags() const ;
        std::string get_author() const ;
        std::string get_maintainer() const ;
        const std::vector<dehancer::License::Type>& get_license_matrix() const ;
        const CLutBuffer& get_clut() const ;

        CameraLutXmp(const CameraLutXmp& other);
        ~CameraLutXmp();

    private:
        CameraLutXmp():path_(){};
        std::string path_;
        std::map<std::string, Exiv2::Value::UniquePtr> meta_;
        CLutBuffer clut_;
        std::vector<dehancer::License::Type> license_matrix_;

    private:
        static dehancer::expected<CameraLutXmp,Error> parse(const std::string &metaBuffer,
                                                     const Blowfish::KeyType &key, const std::string& path);
    };
}
