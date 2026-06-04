/**
 * @file
 * @brief Plattformspezifische Implementierung für Windows (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <windows.h>

#include <filesystem>
#include <string>

export module pfadfinder:system_backend;

import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für Windows
    export struct home_not_set : error
    {
        home_not_set() : error("Home directory not set") {}
    };

    export struct appdata_not_set : error
    {
        appdata_not_set() : error("APPDATA environment variable not set") {}
    };

    export struct localappdata_not_set : error
    {
        localappdata_not_set() : error("LOCALAPPDATA environment variable not set") {}
    };

    export struct allusersappdata_not_set : error
    {
        allusersappdata_not_set() : error("ALLUSERSAPPDATA environment variable not set") {}
    };

    export struct get_module_file_name_failed : error
    {
        get_module_file_name_failed() : error("GetModuleFileNameW failed") {}
    };

    export struct system_environment
    {
        static fs::path executable_path()
        {
            wchar_t path[MAX_PATH] = {0};
            if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
                throw get_module_file_name_failed();
            return fs::path(path);
        }

        static fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            // Windows: Datenverzeichnis ist das Binärverzeichnis
            return exe_dir;
        }

        static fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            // Windows: %ALLUSERSAPPDATA%/<appname>
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");
            if (!allusersappdata)
                throw allusersappdata_not_set();
            return fs::path(allusersappdata) / app_name;
        }

        static fs::path user_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* appdata = std::getenv("APPDATA");
            if (!appdata)
                throw appdata_not_set();
            return fs::path(appdata) / app_name;
        }

        static fs::path config_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* appdata = std::getenv("APPDATA");
            if (!appdata)
                throw appdata_not_set();
            return fs::path(appdata) / app_name;
        }

        static fs::path cache_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");
            if (!localappdata)
                throw localappdata_not_set();
            return fs::path(localappdata) / app_name / "Cache";
        }

        static fs::path log_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");
            if (!localappdata)
                throw localappdata_not_set();
            return fs::path(localappdata) / app_name / "Logs";
        }

        static fs::path temp_dir(const std::string& app_name)
        {
            const char* temp = std::getenv("TEMP");
            if (!temp)
                throw home_not_set();
            return fs::path(temp) / app_name;
        }

        static fs::path user_dir()
        {
            const char* userprofile = std::getenv("USERPROFILE");
            if (!userprofile)
                throw home_not_set();
            return fs::path(userprofile);
        }
    };

}
