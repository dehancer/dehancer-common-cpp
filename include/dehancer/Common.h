//
// Created by denn nevera on 2019-07-18.
//

#pragma once

#include <iostream>
#include <optional>

#include "Expected.h"
#include "nlohmann/json.h"

#define PUBLIC_ENUM(OriginalType) std::underlying_type_t<OriginalType>
#define EXTEND_ENUM(OriginalType, LAST) static_cast<std::underlying_type_t<OriginalType>>(OriginalType::LAST)

namespace dehancer {

    using namespace nonstd;

    /***
     * Main universal intercommunication structure
     */
    typedef nlohmann::json json;

    struct Error;

    /***
     * Error handler spec
     */
    using ErrorHandler  = std::function<void(const Error &error)>;

    /***
     * Common Error handler
     */
    struct Error {

        /**`*
         * Create Error object from error condition or exception message string
         * @param code error condition code
         * @param message exception string
         */
        Error(const std::error_condition code, const std::string &message = "");

        /***
         * Get the error value
         * @return code
         */
        int value() const;

        /***
         * Get the error message string
         * @return error message
         */
        std::string message() const;

        /***
         * Error is negative or error can be skipped
         * @return true if error occurred
         */
        operator bool() const;

        /***
         * Error standard streaming
         * @param os
         * @param error
         * @return
         */
        friend std::ostream &operator<<(std::ostream &os, const Error &error) {
            os << error.value() << "/" << error.message();
            return os;
        }

    private:
        std::error_code code_;
        std::string exception_message_;
    };

    /***
     * Common singleton interface
     * @tparam T
     */
    template<typename T>
    class Singleton {
    public:
        static T &Instance() {
            static T instance;
            return instance;
        }

    protected:
        Singleton() {}

        ~Singleton() {}

    public:
        Singleton(Singleton const &) = delete;

        Singleton &operator=(Singleton const &) = delete;
    };

    /***
    *
    * Formated error string
    *
    * @param format
    * @param ...
    * @return
    */
    std::string error_string(const char *format, ...);

    static inline void _throw_abort(const char *file, int line, const std::string &msg) {
        std::cerr << "Capy logic error: assert failed:\t" << msg << "\n"
                  << "Capy logic error: source:\t\t" << file << ", line " << line << "\n";
        abort();
    }

#ifndef NDEBUG
#   define throw_abort(Msg) _throw_abort( __FILE__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif


    /***
     * Common error codes
     */
    enum class CommonError : int {

        /***
         * Skip the error
         */
                OK = 0,


        /***
         * not supported error
         */
                NOT_SUPPORTED = 300,

        /***
        *
        */
                EXCEPTION = 400,

        /***
         *
         */
                RESPONSE_ERROR = 403,

        /***
         *
         */
                RPC_ERROR = 500,

        /***
         * unknown error
         */
                UNKNOWN,

        /***
         * Resource not found
         */
                NOT_FOUND,

        /***
         * Collection range excaption
         */
                OUT_OF_RANGE,

        /**
         * File could not be parsed
         */
                PARSE_ERROR,

        /**
         * File could not be parsed
         */
                PERMISSIONS_ERROR,

        /***
         *
         */
                SECURITY_ISSUE,

        /**
         * the last error code
         */
                LAST
    };

    /***
     * Common Error category
     */
    class ErrorCategory : public std::error_category {
    public:
        virtual const char *name() const noexcept override;

        virtual std::string message(int ev) const override;

        virtual bool equivalent(const std::error_code &code, int condition) const noexcept override;

    };

    const std::error_category &error_category();

    std::error_condition make_error_condition(CommonError e);

}

namespace std {

    template<>
    struct is_error_condition_enum<dehancer::CommonError>: public true_type {
    };
}
