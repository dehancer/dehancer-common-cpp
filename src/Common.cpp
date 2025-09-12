//
// Created by denn nevera on 2019-06-03.
//

#include "dehancer/Common.h"
#include <memory>
#include <string>
#include <cstdarg>
#include <typeindex>

namespace dehancer {
    namespace singleton {
        struct SingleTonHolder {
            void *object_{};
            std::shared_ptr<std::mutex> mutex_;
        };
    }

    // Global mutex
    static inline std::mutex &get_single_to_mutex() {
        // s_singleTonMutex is not 100% safety for multithread
        // but if there's any singleton object used before thread, it's safe enough.
        static std::mutex s_singleTonMutex;
        return s_singleTonMutex;
    }

    static inline singleton::SingleTonHolder *get_single_ton_type(const std::type_index &typeIndex) {
        static std::unordered_map<std::type_index, singleton::SingleTonHolder> s_singleObjects;

        // Check the old value
        auto itr = s_singleObjects.find(typeIndex);
        if (itr != s_singleObjects.end())
            return &itr->second;

        // Create new one if no old value
        std::pair singleHolder(
            typeIndex,
            singleton::SingleTonHolder()
        );
        itr = s_singleObjects.insert(singleHolder).first;
        singleton::SingleTonHolder &singleTonHolder = itr->second;
        singleTonHolder.object_ = nullptr;
        singleTonHolder.mutex_ = std::make_shared<std::mutex>();

        return &singleTonHolder;
    }

    SINGLETON_API void get_shared_instance(const std::type_index &typeIndex,
                                         void *(*getStaticInstance)(),
                                         void *&instance) {
        // Get the single instance
        singleton::SingleTonHolder *singleTonHolder = nullptr; {
            // Locks and get the global mutex
            std::lock_guard myLock(get_single_to_mutex());
            if (instance != nullptr)
                return;

            singleTonHolder = get_single_ton_type(typeIndex);
        }

        // Create single instance
        {
            // Locks class T and make sure to call construction only once
            std::lock_guard myLock(*singleTonHolder->mutex_);
            if (singleTonHolder->object_ == nullptr) {
                // construct the instance with static funciton
                singleTonHolder->object_ = (*getStaticInstance)();
            }
        }

        // Save single instance object
        {
            std::lock_guard myLock(get_single_to_mutex());
            instance = singleTonHolder->object_;
        }
    }


    Error::Error(const std::error_condition code, const std::string &message): code_(code.value(), code.category()),
                                                                               exception_message_(message) {
    }

    int Error::value() const {
        return code_.value();
    }

    std::string Error::message() const {
        if (!exception_message_.empty()) {
            return exception_message_;
        }
        return code_.message();
    }

    Error::operator bool() const {
        return code_.value() != static_cast<int>(CommonError::OK);
    }

    std::string error_string(const char *format, ...) {
        char buffer[1024] = {};
        va_list ap = {};

        va_start(ap, format);
        vsnprintf(buffer, sizeof(buffer), format, ap);
        va_end(ap);

        return "Dehancer error: " + std::string(buffer);
    }

    std::string message_string(const char *format, ...) {
        char buffer[1024] = {};
        va_list ap = {};

        va_start(ap, format);
        vsnprintf(buffer, sizeof(buffer), format, ap);
        va_end(ap);

        return {buffer};
    }

    using namespace std;

    const char *ErrorCategory::name() const noexcept {
        return "Dehancer";
    }

    bool ErrorCategory::equivalent(const std::error_code &code, int condition) const noexcept {
        return code.value() == condition;
    }

    std::string ErrorCategory::message(int ev) const {
        switch (ev) {
            case static_cast<int>(CommonError::OK):
                return "OK";

            case static_cast<int>(CommonError::NOT_SUPPORTED):
                return "Not supported format";

            default:
                return "Unknown error";
        }
    }

    const std::error_category &error_category() {
        static ErrorCategory instance;
        return instance;
    }

    std::error_condition make_error_condition(dehancer::CommonError e) {
        return {static_cast<int>(e),error_category()};
    }
}
