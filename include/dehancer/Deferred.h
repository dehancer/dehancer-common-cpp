//
// Created by denn nevera on 2019-06-24.
//

#pragma once

#include <functional>
#include <system_error>
#include <mutex>
#include <thread>
#include <atomic>
#include <shared_mutex>
#include <dehancer/Common.h>

namespace dehancer {

    /***
     * Deferred handlers object
     * @tparam Types
     */
    template<class ... Types>
    class Deferred {
    public:

        using VoidHandler      = std::function<void()>;

        /***
         * Data handler prototype
         */
        using DataHandler      = std::function<void(Types... parameters)>;

        using ProgressHandler      = std::function<void(size_t total, size_t progress, size_t count, size_t index)>;

        /***
         * Success handler prototype
         */
        using SuccessHandler   = VoidHandler;

        /***
         * Finalize handler prototype
         */
        using FinalizeHandler  = VoidHandler;

        /***
         * Create Deferred object with error state
         * @param error - error state
         */
        Deferred(const Error &error = Error(CommonError::OK)):
                error_(error) {}

        /***
         * Copy constructor
         * @param that
         */
        Deferred(const Deferred &that) = delete;

        /***
         * Replacer
         * @param that
         */
        Deferred(Deferred &&that) = delete;

        /***
         *
         * Check a statee of the deferred object
         * @return true or false
         */
        operator bool() const {
          return !failed_;
        }

        /**
         * @todo: prepare stack-like reporting
         */

        /***
         * Report data if they received
         * @param parameters
         * @return the object
         */
        const Deferred &report_data_complete(Types... parameters) const {
          if (data_complete_handler_) data_complete_handler_(parameters...);
          return *this;
        }

        /***
         * Report data if they start receiving
         * @param parameters
         * @return the object
         */
        const Deferred &report_data_start(Types... parameters) const {
          if (data_start_handler_) data_start_handler_(parameters...);
          return *this;
        }

        const Deferred &report_progress(size_t total, size_t progress, size_t count, size_t index) {
          if (progress_handler_) progress_handler_(total,progress,count,index);
          return *this;
        }

        /***
         * Reports success if data receiving has done succesfuly
         * @return the object
         */
        const Deferred &report_success() {
          failed_ = false;
          if (success_handler_) success_handler_();
          return *this;
        }

        /***
         * Report error if some error occured
         * @param error
         * @return the object
         */
        const Deferred &report_error(const Error &error) {
          error_ = error;
          if (error_) {
            failed_ = true;
            if (error_handler_) error_handler_(error_);
            error_ = Error(CommonError::OK);
          }
          return *this;
        }

        const Deferred &report_finalize() {
          if (finalize_handler_) finalize_handler_();
          reset();
          return *this;
        }

        /***
         * Deferred call on data complete event
         * @param callback
         * @return the object
         */
        Deferred &on_data_complete(const DataHandler &callback) {
          data_complete_handler_ = callback;
          return *this;
        }

        /***
         * Deferred call on data start event
         * @param callback
         * @return the object
         */
        Deferred &on_data_start(const DataHandler &callback) {
          data_start_handler_ = callback;
          return *this;
        }

        /***
         * Deferred call on every progress event
         * @param callback
         * @return the object
         */
        Deferred &on_progress(const ProgressHandler &callback) {
          progress_handler_ = callback;
          return *this;
        }

        /***
         * Error handler on error event
         * @param callback
         * @return the object
         */
        Deferred &on_error(const ErrorHandler &callback) {
          error_handler_ = callback;
          return *this;
        }

        /***
         * it calls in any case at the end
         * @param callback
         * @return the object
         */
        Deferred &on_finalize(const FinalizeHandler &callback) {
          finalize_handler_ = callback;
          return *this;
        }

        /**
        *  Destructor
        */
        virtual ~Deferred() {
          //if (error_ && error_handler_) error_handler_(error_);
          //if (finalize_handler_) finalize_handler_();
          reset();
        }

    protected:
        DataHandler     data_complete_handler_     = nullptr;
        DataHandler     data_start_handler_        = nullptr;
        ProgressHandler progress_handler_ = nullptr;
        SuccessHandler  success_handler_  = nullptr;
        ErrorHandler    error_handler_    = nullptr;
        FinalizeHandler finalize_handler_ = nullptr;

    private:
        Error error_;
        bool failed_;

        void reset(){
          data_start_handler_ = nullptr;
          data_complete_handler_ = nullptr;
          progress_handler_ = nullptr;
          success_handler_ = nullptr;
          error_handler_ = nullptr;
          finalize_handler_ = nullptr;
        }
    };
}