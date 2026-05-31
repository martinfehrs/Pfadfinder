/**
 * @file
 * @brief Plattformspezifische Implementierung für macOS (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <mach-o/dyld.h>
#include <limits.h>

#include <filesystem>
#include <string>

export module pfadfinder:system_backend;

import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für macOS
    export struct home_not_set : pathfinder_error
    {
        home_not_set() : pathfinder_error("Home directory not set") {}
    };

    export struct get_executable_path_failed : pathfinder_error
    {
        get_executable_path_failed() : pathfinder_error("_NSGetExecutablePath failed") {}
    };

    export struct realpath_failed : pathfinder_error
    {
        realpath_failed() : pathfinder_error("realpath failed") {}
    };

    export struct system_environment
    {
        static fs::path executable_path()
        {
            char path[PATH_MAX] = {0};
            uint32_t size = sizeof(path);
            if (_NSGetExecutablePath(path, &size) != 0)
                throw get_executable_path_failed();
            
            // Symbolische Links auflösen, um den tatsächlichen Pfad zu erhalten
            char real_path[PATH_MAX] = {0};
            if (realpath(path, real_path) == nullptr)
                throw realpath_failed();
            
            return fs::path(real_path);
        }

        static fs::path data_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            // macOS: Prüfen, ob wir in einem Bundle sind
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: von .../Contents/MacOS/ zu .../Contents/Resources/
                return exe_dir.parent_path().parent_path() / "Resources" / app_name;
            }
            else
            {
                // Nicht gebündelt: ähnlich wie Linux
                return exe_dir.parent_path() / "share" / app_name;
            }
        }

        static fs::path user_data_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Application Support/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Application Support" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.local/share/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".local" / "share" / app_name;
            }
        }

        static fs::path config_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Preferences/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Preferences" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.config/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".config" / app_name;
            }
        }

        static fs::path cache_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Caches/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Caches" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.cache/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".cache" / app_name;
            }
        }

        static fs::path log_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Logs/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Logs" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.local/state/<appname>/log (XDG-konform)
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".local" / "state" / app_name / "log";
            }
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
