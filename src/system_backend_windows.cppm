/**
 * @file
 * @brief Plattformspezifische Implementierung für Windows (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <windows.h>

#include <filesystem>
#include <stdexcept>
#include <string>

export module pfadfinder:system_backend;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für Windows
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

    export class appdata_not_set : public pathfinder_error
    {
    public:
        appdata_not_set() : pathfinder_error("APPDATA environment variable not set") {}
    };

    export class localappdata_not_set : public pathfinder_error
    {
    public:
        localappdata_not_set() : pathfinder_error("LOCALAPPDATA environment variable not set") {}
    };

    export class get_module_file_name_failed : public pathfinder_error
    {
    public:
        get_module_file_name_failed() : pathfinder_error("GetModuleFileNameW failed") {}
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

        static fs::path data_directory(const fs::path& exe_dir, const std::string& app_name)
        {
            // Windows: Datenverzeichnis ist das Binärverzeichnis
            return exe_dir;
        }

        static fs::path user_data_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* appdata = std::getenv("APPDATA");
            if (!appdata)
                throw appdata_not_set();
            return fs::path(appdata) / app_name;
        }

        static fs::path config_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* appdata = std::getenv("APPDATA");
            if (!appdata)
                throw appdata_not_set();
            return fs::path(appdata) / app_name;
        }

        static fs::path cache_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");
            if (!localappdata)
                throw localappdata_not_set();
            return fs::path(localappdata) / app_name / "Cache";
        }

        static fs::path log_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");
            if (!localappdata)
                throw localappdata_not_set();
            return fs::path(localappdata) / app_name / "Logs";
        }

        static fs::path temp_directory(const std::string& app_name)
        {
            const char* temp = std::getenv("TEMP");
            if (!temp)
                throw home_not_set();
            return fs::path(temp) / app_name;
        }

        static fs::path user_directory()
        {
            const char* userprofile = std::getenv("USERPROFILE");
            if (!userprofile)
                throw home_not_set();
            return fs::path(userprofile);
        }
    };

}
