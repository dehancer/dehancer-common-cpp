//
// Created by lotus mile on 31/10/2018.
//

#pragma once

#include <time.h>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <optional>
#include <stdarg.h>
#include <chrono>

#include "Common.h"

namespace dehancer {

    int random(int min, int max);

    namespace time {
        /**
        *  Converts UTC time string to a time_t value.
        *  return tm struct
        *  */
        std::time_t get_epoch_time(const std::string &dateTime);

        /**
         *  Converts UTC time string to a tm struct.
         *  return tm struct, tm_year = tm_year+1900
         *  */
        std::tm get_time(const std::string &dateTime, bool adjust_year = true);

        std::time_t get_iso8601_time(const std::string &dateTime);

        std::string utc_to_time_string(std::time_t t);

        std::string utc_to_date_string(std::time_t t);

        static inline std::time_t now()
        {
            return std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
        }

        std::time_t now_utc();
    }

    //
    // Clang on OSX does not support <filesystem>
    //

    namespace file {
        std::string last_component(std::string const &pathname);

        std::string deleting_extension(std::string const &filename);

        bool copy_file(const char *source, const char *destination);

        int mkdir_p(const char *dir, const mode_t mode);
    }

    namespace url {
        std::string encode(const std::string &value);

        std::string decode(const std::string &text);
    }
}
