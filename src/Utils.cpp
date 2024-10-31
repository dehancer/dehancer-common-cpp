//
// Created by lotus mile on 31/10/2018.
//

#include <cstdarg>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>

#if WIN32
#include <direct.h>
#define S_ISDIR(x) ((x&_S_IFDIR)==_S_IFDIR)
#else
#include <unistd.h>
#endif

static constexpr const size_t PATH_MAX_STRING_SIZE = 256;

#include "dehancer/Utils.h"


namespace dehancer {

    std::string_view trim(std::string_view s)
    {
        s.remove_prefix(std::min(s.find_first_not_of(" \t\r\v\n"), s.size()));
        s.remove_suffix((s.size() - 1) - std::min(s.find_last_not_of(" \t\r\v\n"), s.size() - 1));

        return s;
    }

    int random(int min, int max)
    {
        static bool first = true;
        if (first)
        {
            srand((unsigned int) std::time(0));
            first = false;
        }
        return min + rand() % ((max + 1) - min);
    }

    namespace time {
        std::tm convert_iso8601(const std::string &timestr)
        {

            tzset();

            struct tm ctime{};
            struct tm tm_data{};

            memset(&ctime, 0, sizeof(struct tm));
            #if WIN32
            std::istringstream ss(timestr);
            ss >> std::get_time(&ctime, "%Y-%m-%dT%T%z");

            time_t ts = mktime(&ctime); // ??? - timezone пропал!;

            localtime_s(&tm_data, &ts);

            #else
            strptime(timestr.c_str(), "%FT%T%z", &ctime);

            long ts = mktime(&ctime) - timezone;
            localtime_r(&ts, &tm_data);
            #endif

            return tm_data;
        }

        std::tm get_time(const std::string &dateTime, bool adjust_year)
        {

            static const std::string dateTimeFormat{"%Y-%h-%d %H:%M:%S %Z"};
            std::istringstream ss{dateTime};
            std::tm dt{};
            ss >> std::get_time(&dt, dateTimeFormat.c_str());
            if (adjust_year)
                dt.tm_year += 1900;
            return dt;
        }

        std::time_t get_iso8601_time(const std::string &dateTime)
        {
            std::tm dt = convert_iso8601(dateTime);

            #if WIN32
            return _mkgmtime(&dt);
            #else
            return timegm(&dt);
            #endif
        }

        std::time_t get_epoch_time(const std::string &dateTime)
        {
            std::tm dt = get_time(dateTime, false);

            #if WIN32
            return _mkgmtime(&dt);
            #else
            return timegm(&dt);
            #endif
        }

        std::string utc_to_time_string(std::time_t t)
        {
            std::ostringstream ss;
            ss << std::put_time(std::gmtime(&t), "%Y-%m-%d %H:%M:%S %Z");
            return ss.str();
        }

        std::string utc_to_date_string(std::time_t t)
        {
            std::ostringstream ss;
            ss << std::put_time(std::gmtime(&t), "%Y-%m-%d");
            return ss.str();
        }

        time_t now_utc()
        {
            std::time_t t = std::time(nullptr);
            std::tm *nowTm;
            nowTm = std::gmtime(&t);
            return nowTm != nullptr ? mktime(nowTm) : now();
        }
    }

    namespace file {
        struct pathSeparator {
            bool operator()(char ch) const
            {
                #if WIN32
                return ch == '\\';
                #else
                return ch == '/';
                #endif
            }
        };

        std::string last_component(std::string const &pathname)
        {
            return std::string(
                    std::find_if(pathname.rbegin(), pathname.rend(),
                                 pathSeparator()).base(),
                    pathname.end());
        }

        std::string deleting_extension(std::string const &filename)
        {
            std::string::const_reverse_iterator
                    pivot
                    = std::find(filename.rbegin(), filename.rend(), '.');
            return pivot == filename.rend()
                   ? filename
                   : std::string(filename.begin(), pivot.base() - 1);
        }

        // copy in binary mode
        bool copy_file(const char *source, const char *destination)
        {
            std::ifstream src(source, std::ios::binary);
            std::ofstream dest(destination, std::ios::binary);
            dest << src.rdbuf();
            return src && dest;
        }

        /* recursive mkdir based on
        http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
        */

        /* recursive mkdir */
        int mkdir_p(const char *dir, const mode_t mode)
        {
            char tmp[PATH_MAX_STRING_SIZE];
            #if WIN32
            char sep = '\\';
            #else
            char sep = '/';
            #endif
            char *p = nullptr;
            struct stat sb{};
            size_t len;

            /* copy path */
            len = strnlen(dir, PATH_MAX_STRING_SIZE);
            if (len == 0 || len == PATH_MAX_STRING_SIZE)
            {
                return -1;
            }
            memcpy(tmp, dir, len);
            tmp[len] = '\0';

            /* remove trailing slash */
            if (tmp[len - 1] == sep)
            {
                tmp[len - 1] = '\0';
            }

            /* check if path exists and is a directory */
            if (stat(tmp, &sb) == 0)
            {
                if (S_ISDIR (sb.st_mode))
                {
                    return 0;
                }
            }

            /* recursive mkdir */
            for (p = tmp + 1; *p; p++)
            {
                if (*p == sep)
                {
                    #if WIN32
                    /* In Windows skip drive letter */
                    if(*(p-1) == ':') continue;
                    #endif
                    *p = 0;
                    /* test path */
                    if (stat(tmp, &sb) != 0)
                    {
                        /* path does not exist - create directory */
                        #if WIN32
                        if (mkdir(tmp) < 0)
                        #else
                        if (mkdir(tmp, mode) < 0)
                        #endif
                        {
                            return -1;
                        }
                    } else if (!S_ISDIR(sb.st_mode))
                    {
                        /* not a directory */
                        return -1;
                    }
                    *p = sep;
                }
            }
            /* test path */
            if (stat(tmp, &sb) != 0)
            {
                /* path does not exist - create directory */
                #if WIN32
                if (mkdir(tmp) < 0)
                #else
                if (mkdir(tmp, mode) < 0)
                #endif
                {
                    return -1;
                }
            } else if (!S_ISDIR(sb.st_mode))
            {
                /* not a directory */
                return -1;
            }
            return 0;
        }
    }

    namespace url {
        std::string encode(const std::string &value)
        {
            std::ostringstream escaped;
            escaped.fill('0');
            escaped << std::hex;

            for (char c : value)
            {
                // Keep alphanumeric and other accepted characters intact
                if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                {
                    escaped << c;
                    continue;
                }

                // Any other characters are percent-encoded
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << int((unsigned char) c);
                escaped << std::nouppercase;
            }

            return escaped.str();
        }

        char from_hex(char ch)
        {
            return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
        }

        std::string decode(const std::string &text)
        {
            char h;
            std::ostringstream escaped;
            escaped.fill('0');

            for (auto i = text.begin(), n = text.end(); i != n; ++i)
            {
                std::string::value_type c = (*i);

                if (c == '%')
                {
                    if (i[1] && i[2])
                    {
                        h = from_hex(i[1]) << 4 | from_hex(i[2]);
                        escaped << h;
                        i += 2;
                    }
                } else if (c == '+')
                {
                    escaped << ' ';
                } else
                {
                    escaped << c;
                }
            }

            return escaped.str();
        }
    }
}