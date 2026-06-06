/**
 * @file
 * @brief Plattformspezifische Implementierung für Linux (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <unistd.h>

#include <filesystem>
#include <climits>
#include <string>

export module pfadfinder:system_backend;

import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für Linux
    export struct readlink_failed : error
    {
        readlink_failed() : error("readlink failed") {}
    };

    export struct system_environment
    {
        static fs::path executable_path()
        {
            char path[PATH_MAX] = {0};
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
            if (len == -1)
                throw readlink_failed();
            path[len] = '\0';
            return fs::path(path);
        }

        static fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            // Linux: von /usr/bin/myapp zu /usr/share/myapp
            return exe_dir.parent_path() / "share" / app_name;
        }

        static fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            // Linux: /var/lib/<appname>
            return fs::path("/var/lib") / app_name;
        }

        static fs::path user_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".local" / "share" / app_name;
        }

        static fs::path config_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".config" / app_name;
        }

        static fs::path cache_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".cache" / app_name;
        }

        static fs::path log_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".local" / "state" / app_name / "log";
        }

        static fs::path temp_dir(const std::string& app_name)
        {
            return fs::temp_directory_path() / app_name;
        }

        static fs::path user_dir()
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home);
        }
    };

}
