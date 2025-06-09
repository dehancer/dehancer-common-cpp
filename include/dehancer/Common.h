//
// Created by denn nevera on 2019-07-18.
//

#pragma once

#include <iostream>
#include <optional>
#include <thread>
#include <mutex>
#include <typeindex>

#include "Expected.h"
#include "nlohmann/json.h"

#define PUBLIC_ENUM(OriginalType) std::underlying_type_t<OriginalType>
#define EXTEND_ENUM(OriginalType, LAST) static_cast<std::underlying_type_t<OriginalType>>(OriginalType::LAST)

#ifndef INC_SINGLETON_API_H_
#define INC_SINGLETON_API_H_

#if defined(_WIN32) || defined(__CYGWIN__)
#  if defined(singleton_EXPORTS) // add by CMake
#    ifdef __GNUC__
#      define  SINGLETON_API __attribute__(dllexport)
#    else
#      define  SINGLETON_API __declspec(dllexport)
#    endif
#  else
#    ifdef __GNUC__
#      define  SINGLETON_API __attribute__(dllimport)
#    else
#      define  SINGLETON_API __declspec(dllimport)
#    endif
#  endif // singleton_EXPORTS

#elif defined __GNUC__
#  if __GNUC__ >= 4
#    define SINGLETON_API __attribute__ ((visibility ("default")))
#  else
#    define SINGLETON_API
#  endif

#elif defined __clang__
#  define SINGLETON_API __attribute__ ((visibility ("default")))

#else
#   error "Do not know how to export classes for this platform"
#endif


#endif

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
    using ErrorHandler = std::function<void(const Error &error)>;

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
    class SimpleSingleton {
    public:
        static T &Instance() {
            static T instance;
            return instance;
        }

    protected:
        SimpleSingleton() = default;

        ~SimpleSingleton() = default;

    public:
        SimpleSingleton(SimpleSingleton const &) = delete;

        SimpleSingleton &operator=(SimpleSingleton const &) = delete;
    };

    SINGLETON_API void get_shared_instance(const std::type_index &typeIndex,
                                           void *(*get_static_instance)(),
                                           void *&instance);

    /***
        * Common singleton interface
        * @tparam T
        */
    template<typename T>
    class SharedSingleton {
    public:
        static T &Instance() {
            // static T instance;
            // return instance;
            static void *instance = nullptr;
            if (instance == nullptr)
                get_shared_instance(typeid(T), &get_static_instance, instance);
            return *static_cast<T *>(instance);
        }

        SharedSingleton(SharedSingleton const &) = delete;

        SharedSingleton &operator=(SharedSingleton const &) = delete;

    protected:
        SharedSingleton() = default;
        ~SharedSingleton() = default;

        static void *get_static_instance() {
            static T t;
            return reinterpret_cast<void *>(&reinterpret_cast<char &>(t));
        }
    };

    template<typename T>
    class ControlledSingleton {
    public:
        using InstanceType = T;

        static InstanceType &Instance() {
            static InstanceType *instance = nullptr;
            static std::once_flag flag;
            std::call_once(flag, [&] {
                if (!instance) {
                    instance = new InstanceType();
                }
            });
            return *instance;
        }

        static
        void CreateInstance() {
            InstanceType &p = Instance();
        }

        static
        void DestroyInstance() {
            InstanceType &p = Instance();
            delete &p;
        }

    protected:
        ControlledSingleton() = default;

        ~ControlledSingleton() = default;

    public:
        ControlledSingleton(ControlledSingleton const &) = delete;

        ControlledSingleton &operator=(ControlledSingleton const &) = delete;
    };

#if defined(DEHANCER_CONTROLLED_SINGLETON)
    template<class T>using Singleton=ControlledSingleton<T>;
#else
    template<class T>
    using Singleton = SharedSingleton<T>;
#endif

    /***
    *
    * Formated error string
    *
    * @param format
    * @param ...
    * @return
    */
    std::string error_string(const char *format, ...);

    std::string message_string(const char *format, ...);

    static inline void _throw_abort(const char *file, int line, const std::string &msg) {
        std::cerr << "Dehancer logic error: assert failed:\t" << msg << "\n"
            << "Dehancer logic error: source:\t\t" << file << ", line " << line << "\n";
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
         * Rest auth error (typically bad or outdated auth token
         */
        REST_AUTH_ERROR = 600,

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
        [[nodiscard]] virtual const char *name() const noexcept override;

        [[nodiscard]] virtual std::string message(int ev) const override;

        [[nodiscard]] virtual bool equivalent(const std::error_code &code, int condition) const noexcept override;
    };

    const std::error_category &error_category();

    std::error_condition make_error_condition(CommonError e);
}

namespace std {
    template<>
    struct is_error_condition_enum<dehancer::CommonError> : public true_type {
    };
}
