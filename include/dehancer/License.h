//
// Created by denn nevera on 2019-08-10.
//

#pragma once

#include <string>
#include "dehancer/Common.h"


namespace dehancer{

    struct License {

        enum class Type:int {
            promo     = 0,
            light     = 1,
            standard  = 2,
            pro       = 3,
            unknown   = -1
        };

        Type        type;
        std::string email;
        std::string name;
        std::string maintainer;
        time_t      issue_date;
        time_t      expiry_date;
        bool        offline_enabling;

        /**
         * Sign the activation key
         * @return error or OK
         */
        Error sign();

        /**
         * Activation key is valid
         * @return true or false
         */
        bool is_valid() const;

        /**
         * Check expiration date
         * @return true or false
         */
        bool is_expired() const ;

        /**
        * Check the activation key is active
        * @return true or false
        */
        bool is_active() const;

        /**
         * Get the license id
         * @return unique license id string
         */
        const std::string& get_id() const ;

        /**
         * Get the cativation key signature
         * @return signature base58 string
         */
        const std::string& get_signature() const;

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
        time_t get_activation_date() const;

        /**
         * Create a template of licens activation key
         */
        License();

        /**
         * Copy
         * @param license
         */
        License(const License& license);

        /**
         * Decode License activation key from base64 string
         * @param base64
         * @return license or error if string can't be decoded
         */
        static expected<License,Error> Decode(const std::string& base64);

        /**
         * Encode the license to base64 activation key string
         * @param license
         * @return base64 activation key string
         */
        static std::string Encode(const License& license);

        /**
         * Encode license from json object
         * @param json
         * @return licens ot error
         */
        static expected<License,Error> from_json(const json& json);

        /**
         * Dump as json
         * @return json object
         */
        dehancer::json json() const;

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


