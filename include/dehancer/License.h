//
// Created by denn nevera on 2019-08-10.
//

#pragma once

#include <string>
#include <cinttypes>
#include "dehancer/Common.h"


namespace dehancer{

    struct License {

        enum class Type:int {
            promo     =  0,
            light     =  1,
            standard  =  2,
            pro       =  3,
            unknown   = -1
        };

        /* license type*/
        Type           type;
        /* software version */
        std::uint16_t  version;
        /* user email */
        std::string    email;
        /* software name */
        std::string    name;
        /* software host */
        std::string    host;
        /* license maintainer */
        std::string    maintainer;
        /* activation key upgraded from or empty */
        std::string    upgraded_id;
        /* activation issue date -1 == undefined */
        time_t         issue_date;
        /* expiration date -1 == undefined 0 - unlimited */
        time_t         expiry_date;
        /* user can activate offline */
        bool           offline_enabling;

        /**
         * Sign the activation key
         * @return error or OK
         */
        Error sign();

        /**
         * Activation key is valid
         * @return true or false
         */
        [[nodiscard]] bool is_valid() const;

        /**
         * Check expiration date
         * @return true or false
         */
        [[nodiscard]] bool is_expired() const ;

        /**
        * Check the activation key is active
        * @return true or false
        */
        [[nodiscard]] bool is_active() const;

        /**
         * Get the license id
         * @return unique license id string
         */
        [[nodiscard]] const std::string& get_id() const ;

        /**
         * Get the cativation key signature
         * @return signature base58 string
         */
        [[nodiscard]] const std::string& get_signature() const;

        /**
         * Activate the key
         */
        void activate();

        /**
         * Deactivate the key
         */
        void deactivate();

        /**
         * Get activation date
         * @return date in time_t
         */
        [[nodiscard]] time_t get_activation_date() const;

        /**
         * Create a template of licens activation key
         */
        License();

        /**
         * Copy
         * @param license instance
         */
        License(const License& license);

        /**
         * Decode License activation key from base64 string
         * @param base64 base64 encoded string
         * @return license or error if string can't be decoded
         */
        static expected<License,Error> Decode(const std::string& base64);

        /**
         * Encode the license to base64 activation key string
         * @param license instance
         * @param line_break_enabled enable line break
         * @return base64 activation key string
         */
        static std::string Encode(const License& license, bool line_break_enabled = true);

        /**
         * Encode license from json object
         * @param json lic serialized in json 
         * @return licens ot error
         */
        static expected<License,Error> from_json(const json& json);

        /**
         * Dump as json
         * @return json object
         */
        [[nodiscard]] dehancer::json json() const;

        License& operator=(const License& right);

    private:
        std::string id_;
        std::string signature_;
        time_t      activation_date_;
        bool        is_active_;

    public:
        static std::string authority_key;
        static std::string authority_public_key;

    };
}


