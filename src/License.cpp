//
// Created by denn nevera on 2019-08-10.
//

#include "dehancer/License.h"
#include "dehancer/Base64.h"
#include "dehancer/Utils.h"
#include <ed25519.hpp>
#include <climits>
#include "dehancer/obfy/instr.h"

namespace dehancer {

    std::string License::authority_key;
    std::string License::authority_public_key;

    std::string_view trim(std::string_view s)
    {
      s.remove_prefix(std::min(s.find_first_not_of(" \t\r\v\n"), s.size()));
      s.remove_suffix((s.size() - 1) - std::min(s.find_last_not_of(" \t\r\v\n"), s.size() - 1));

      return s;
    }

    static inline auto make_digest(const License& lic) {
      return ed25519::Digest([lic](auto &calculator){
          calculator.append(static_cast<std::uint16_t>(lic.version));
          calculator.append(static_cast<int>(lic.type));
          calculator.append(lic.email);
          calculator.append(lic.name);
          if (!lic.host.empty())
            calculator.append(lic.host);
          calculator.append(lic.get_id());
          calculator.append(lic.maintainer);
          calculator.append(lic.upgraded_id);
          calculator.append(lic.offline_enabling);
          calculator.append(static_cast<int>(lic.issue_date));
          calculator.append(static_cast<int>(lic.expiry_date));
      });
    }

    License::License():
      type(Type::unknown),
      version(1),
      email(""),
      name(""),
      host(""),
      maintainer(""),
      upgraded_id(""),
      issue_date(-1),
      expiry_date(-1),
      offline_enabling(false),
      id_(ed25519::Seed().encode()),
      signature_(""),
      activation_date_(-1),
      is_active_(false)
    {}

    License::License(const dehancer::License &license)
    {
      *this = license;
    }

    License& License::operator = (const dehancer::License &license) {
      version = license.version;
      type = license.type;
      email = license.email;
      name = license.name;
      host = license.host;
      maintainer = license.maintainer;
      upgraded_id = license.upgraded_id;
      issue_date = license.issue_date;
      expiry_date = license.expiry_date;
      activation_date_ = license.activation_date_;
      id_ = license.id_;
      signature_ = license.signature_;
      is_active_ = license.is_active_;
      offline_enabling = license.offline_enabling;
      return *this;
    }

    expected<License,Error> License::from_json(const dehancer::json &_json) {
      try {

        License lic;

        if (_json.count("version")>0)
          lic.version      = _json.at("version").get<std::uint16_t>();
        else
          lic.version = 1;

        lic.type         = _json.at("type").get<Type>();
        lic.email        = _json.at("email").get<std::string>();
        lic.name         = _json.at("name").get<std::string>();
        
        if (_json.count("host")>0)
          lic.host         = _json.at("host").get<std::string>();
        else
          lic.host = "";
        
        lic.maintainer   = _json.at("maintainer").get<std::string>();
        lic.issue_date   = _json.at("issue_date").get<time_t>();

        if (_json.count("upgraded_id")>0)
          lic.upgraded_id  = _json.at("upgraded_id").get<std::string>();

        lic.expiry_date  = _json.at("expiry_date").get<time_t>();
        lic.activation_date_  = _json.at("activation_date").get<time_t>();
        lic.id_          = _json.at("id").get<std::string>();
        lic.signature_   = _json.at("signature").get<std::string>();
        lic.is_active_    = _json.at("is_active").get<bool>();
        lic.offline_enabling    = _json.at("offline_enabling").get<bool>();

        return lic;
      }
      catch (const std::exception &e) {
        return make_unexpected(Error(CommonError::PARSE_ERROR, e.what()));
      }
      catch (...) {
        return make_unexpected(Error(CommonError::PARSE_ERROR, "License text could not be decoded..."));
      }
    }

    dehancer::json License::json() const {
      dehancer::json data = {
              {"version", static_cast<std::uint16_t>(version)},
              {"type", static_cast<int>(type)},
              {"email", email},
              {"name", name},
              {"host", host},
              {"id", id_},
              {"maintainer", maintainer},
              {"issue_date", static_cast<time_t>(issue_date)},
              {"upgraded_id", static_cast<std::string>(upgraded_id)},
              {"expiry_date", static_cast<time_t>(expiry_date)},
              {"activation_date", static_cast<time_t>(activation_date_)},
              {"signature", signature_},
              {"is_active", is_active_},
              {"offline_enabling", offline_enabling},
      };

      return data;
    }

    Error License::sign() {
      OBF_BEGIN

        if (!signature_.empty())
          return Error(CommonError::PERMISSIONS_ERROR, "License has already been signed...");

        if (authority_key.empty())
          return Error(CommonError::PERMISSIONS_ERROR, "License could not be signed on client ... ");

        auto digest = make_digest(*this);

        auto pair = ed25519::keys::Pair::FromPrivateKey(authority_key);

        auto signature = pair->sign(digest);

        if (!signature) {
          return Error(CommonError::SECURITY_ISSUE, "License could not be signed...");
        }

        signature_ = signature->encode();

        RETURN( Error(CommonError::OK));

      OBF_END
    }

    std::string License::Encode(const License& license, bool line_break_enabled) {
      std::string base64;
      base64::encode(license.json().dump(), base64, line_break_enabled?76:INT_MAX);
      return  base64;
    }

    expected<License,Error> License::Decode(const std::string &base64_in) {

      try {

        if (base64_in.empty())
          return make_unexpected(Error(CommonError::NOT_FOUND, "Inactive license"));

        std::string base64(trim(base64_in));

        if (base64.empty())
          return make_unexpected(Error(CommonError::NOT_FOUND, "Inactive license"));

        std::string buffer;

        base64::decode(base64, buffer);

        dehancer::json json_data = json::parse(buffer);

        return License::from_json(json_data);

      }
      catch (std::exception& e) {
        return make_unexpected(Error(CommonError::PARSE_ERROR, "License text could not be decoded..."));
      }
      catch (...) {
        return make_unexpected(Error(CommonError::PARSE_ERROR, "License text could not be decoded..."));
      }
    }

    bool License::is_expired() const {
      OBF_BEGIN
        RETURN (expiry_date == 0 ? false : (expiry_date - time::now_utc()) < 0);
      OBF_END
    }

    bool License::is_active() const {
      OBF_BEGIN
        RETURN( is_active_ && activation_date_>= 0);
      OBF_END
    }
    bool License::is_valid() const {
      OBF_BEGIN

        auto digest = make_digest(*this);

        auto pk =  ed25519::keys::Public::Decode(authority_public_key);

        if (!pk) return false;

        auto signature = ed25519::Signature::Decode(signature_);

        if (!signature) return false;

        RETURN(signature->verify(digest, *pk));

      OBF_END
    }

    void License::activate() {
      if (is_active_) return;
      is_active_ = true;
      activation_date_ = std::time(nullptr);
    }

    void License::deactivate() {
      if (!is_active_) return;
      is_active_ = false;
    }

    const std::string& License::get_id() const {return id_;}

    const std::string& License::get_signature() const {return signature_;}

    time_t License::get_activation_date() const { return  activation_date_; }

}