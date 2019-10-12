//
// Created by denn nevera on 2019-07-18.
//

#include "dehancer/MLutXmp.h"
#include "dehancer/Blowfish.h"
#include "dehancer/Base64.h"
#include "dehancer/Utils.h"

#include <fstream>
#include <sstream>

namespace dehancer {

    static const std::string xmp_meta_prefix = "MLutXmp.Dehancer.mlutAttributes.undo[1]/rdf:";
    static const std::string xmp_clut_prefix = "MLutXmp.Dehancer.mlutClutList.undo[1]/rdf:";

    template<typename T>
    std::vector<T> slice(std::vector<T> const &v, int m, int n)
    {
        auto first = v.cbegin() + m;
        auto last = v.cbegin() + n;// + 1;

        std::vector<T> vec(first, last);
        return vec;
    }


    MLutXmp::~MLutXmp() {}

    dehancer::expected<MLutXmp,Error> MLutXmp::Open(const std::string &path) {
        return MLutXmp::Open(path,Blowfish::KeyType());
    }

    dehancer::expected<MLutXmp,Error> MLutXmp::Open(const std::string& path, const Blowfish::KeyType& key) {

        std::string metaBuffer;
        std::ifstream inFile;
        try {

            inFile.open(path.c_str(),  std::fstream::in);

            if (!inFile.is_open()) {
                return dehancer::make_unexpected(Error(
                        CommonError::NOT_FOUND,
                        error_string("mlut_xmp file %s could not be opened or found", path.c_str())));
            }

            inFile.seekg(0, std::ios::end);
            metaBuffer.reserve(inFile.tellg());
            inFile.seekg(0, std::ios::beg);

            metaBuffer.assign((std::istreambuf_iterator<char>(inFile)),
                              std::istreambuf_iterator<char>());

            inFile.close();

            Exiv2::XmpData xmpData;

            if (0 != Exiv2::XmpParser::decode(xmpData, metaBuffer)) {
                return dehancer::make_unexpected(Error(
                        CommonError::PARSE_ERROR,
                        error_string("mlut_xmp file %s could not be parsed", path.c_str())));
            }

            Exiv2::XmpParser::terminate();

            Blowfish fish(key.empty() ? Blowfish::KeyType({0,0,0,0,0,0,0,0}) : key);

            auto xmp = MLutXmp();
            xmp.path_ = path;

            for (auto md = xmpData.begin(); md != xmpData.end(); ++md) {

                bool is_clut = false;

                if (!key.empty()) {


                    for (int i = 0; i < 3; ++i) {

                        std::stringstream iss;

                        iss << xmp_clut_prefix << "clutList[" << i + 1 << "]";

                        if (iss.str() == md->key()) {

                            is_clut = true;

                            CLutBuffer tmp;
                            tmp.resize(md->value().count());

                            md->value().copy(tmp.data(), Exiv2::ByteOrder::littleEndian);

                            CLutBuffer out;
                            CLutBuffer buffer;

                            base64::decode(tmp, out);

                            fish.decrypt(out, buffer);

                            xmp.cluts_.push_back(buffer);

                            break;
                        }
                    }
                }

                if (is_clut) continue;

                xmp.meta_[md->key()] = md->value().clone();
            }

            return xmp;
        }
        catch (std::exception &err) {
            return dehancer::make_unexpected(Error(CommonError::NOT_FOUND, err.what()));
        }
    }

    MLutXmp::MLutXmp(const MLutXmp &other):path_(other.path_){
        for (auto &v: other.meta_) {
            meta_[v.first] = std::move(v.second->clone());
        }
        for (auto &v: other.cluts_) {
            cluts_.push_back(v);
        }
    };

    Exiv2::Value::UniquePtr MLutXmp::get_value(const std::string &name) const {
        try {
            std::string key = xmp_meta_prefix;
            key.append(name);
            if (auto& v = meta_.at(key.c_str()))
                return  std::move(v->clone());
            return nullptr;
        }
        catch (...) {
            return nullptr;
        }
    }

    static std::vector<std::string> keys = {
            "nsrevision",
            "nscolorType",
            "nsISOIndex",
            "nsexpandBlendingMode",
            "nsexpandImpact",
            "nsauthor",
            "nscaption",
            "nsdescription",
            "nsmaintainer",
            "nslutSize",
            "nsfilmType",
            "nslutType",
            "nstags",
            "serial",
            "datetime"
    };

    const std::vector<std::string>& MLutXmp::get_key_list() const {
        return keys;
    }

    const int MLutXmp::get_ISO_index() const {
        if (get_value("nsISOIndex"))
            return get_value("nsISOIndex")->toLong();
        return 100;
    }

    const int MLutXmp::get_expand_mode() const {
        if (get_value("nsexpandBlendingMode"))
            return get_value("nsexpandBlendingMode")->toLong();
        return 0;
    }

    const float MLutXmp::get_expand_impact() const {
        if (get_value("nsexpandImpact"))
            return get_value("nsexpandImpact")->toFloat();
        return 1;
    }

    time_t MLutXmp::get_datetime() const {
        if (get_value("datetime"))
            return time::get_iso8601_time(get_value("datetime")->toString());
        else
            return std::time(0);
    }

    int MLutXmp::get_revision() const {
        if (get_value("nsrevision"))
            return get_value("nsrevision")->toLong();
        return 0;
    };

    std::string MLutXmp::get_name() const {
        return file::deleting_extension(file::last_component(path_));
    }

    std::string MLutXmp::get_caption() const {
        if (get_value("nscaption"))
            return get_value("nscaption")->toString();
        return "";
    }

    std::string MLutXmp::get_description() const {
        if (get_value("nsdescription"))
            return get_value("nsdescription")->toString();
        return "";
    }

    std::string MLutXmp::get_tags() const {
        if (get_value("nstags"))
            return get_value("nstags")->toString();
        return "";
    }

    std::string MLutXmp::get_author() const {
        if (get_value("nsauthor"))
            return get_value("nsauthor")->toString();
        return "";
    }

    std::string MLutXmp::get_maintainer() const {
        if (get_value("nsmaintainer"))
            return get_value("nsmaintainer")->toString();
        return "";
    }

    const std::vector<MLutXmp::CLutBuffer>& MLutXmp::get_cluts() const {
        return cluts_;
    }

}