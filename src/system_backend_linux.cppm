/**
 * @file
 * @brief Plattformspezifische Implementierung für Linux (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <unistd.h>
#include <limits.h>

#include <filesystem>
#include <stdexcept>
#include <string>

export module pfadfinder:system_backend;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für Linux
    export class pathfinder_error : public std::runtime_error
    {
    public:
        explicit pathfinder_error(const char* message) : std::runtime_error(message) {}
    };

    export class home_not_set : public pathfinder_error
    {
    public:
        home_not_set() : pathfinder_error("Home directory not set") {}
    };

    export class readlink_failed : public pathfinder_error
    {
    public:
        readlink_failed() : pathfinder_error("readlink failed") {}
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

        static fs::path data_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            // Linux: von /usr/bin/myapp zu /usr/share/myapp
            return exe_dir.parent_path() / "share" / app_name;
        }

        static fs::path user_data_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".local" / "share" / app_name;
        }

        static fs::path config_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".config" / app_name;
        }

        static fs::path cache_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".cache" / app_name;
        }

        static fs::path log_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".local" / "state" / app_name / "log";
        }

        static fs::path temp_directory(const std::string& app_name)
        {
            return fs::temp_directory_path() / app_name;
        }

        static fs::path user_directory()
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home);
        }
    };

}
