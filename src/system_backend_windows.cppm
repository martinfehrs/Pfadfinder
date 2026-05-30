/**
 * @file
 * @brief Plattformspezifische Implementierung für Windows (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <windows.h>

#include <expected>
#include <filesystem>
#include <string>

export module pfadfinder:system_backend;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Fehlercodes für Windows
    export enum class error_code
    {
        home_not_set,                       ///< Home-Verzeichnis nicht gesetzt
        appdata_not_set,                    ///< APPDATA nicht gesetzt
        localappdata_not_set,               ///< LOCALAPPDATA nicht gesetzt
        windows_get_module_file_name_failed ///< GetModuleFileNameW fehlgeschlagen
    };

    export const char* error_message(error_code ec)
    {
        switch (ec)
        {
            case error_code::home_not_set:                        return "Home directory not set";
            case error_code::appdata_not_set:                     return "APPDATA environment variable not set";
            case error_code::localappdata_not_set:                return "LOCALAPPDATA environment variable not set";
            case error_code::windows_get_module_file_name_failed: return "GetModuleFileNameW failed";
            default:                                              return "Unknown error";
        }
    }

    std::expected<fs::path, error_code> get_executable_path()
    {
        wchar_t path[MAX_PATH] = {0};
        if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
            return std::unexpected(error_code::windows_get_module_file_name_failed);
        return fs::path(path);
    }

    std::expected<fs::path, error_code> get_data_directory(const fs::path& exe_dir, const std::string& app_name)
    {
        // Windows: Datenverzeichnis ist das Binärverzeichnis mit app_name
        return exe_dir;
    }

    std::expected<fs::path, error_code> get_user_data_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* appdata = std::getenv("APPDATA");
        if (!appdata)
            return std::unexpected(error_code::appdata_not_set);
        return fs::path(appdata) / app_name;
    }

    std::expected<fs::path, error_code> get_config_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* appdata = std::getenv("APPDATA");
        if (!appdata)
            return std::unexpected(error_code::appdata_not_set);
        return fs::path(appdata) / app_name;
    }

    std::expected<fs::path, error_code> get_cache_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* localappdata = std::getenv("LOCALAPPDATA");
        if (!localappdata)
            return std::unexpected(error_code::localappdata_not_set);
        return fs::path(localappdata) / app_name / "Cache";
    }

    std::expected<fs::path, error_code> get_log_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* localappdata = std::getenv("LOCALAPPDATA");
        if (!localappdata)
            return std::unexpected(error_code::localappdata_not_set);
        return fs::path(localappdata) / app_name / "Logs";
    }

    std::expected<fs::path, error_code> get_system_temp_directory()
    {
        const char* temp = std::getenv("TEMP");
        if (!temp)
            return std::unexpected(error_code::home_not_set);
        return fs::path(temp);
    }

    std::expected<fs::path, error_code> get_user_directory()
    {
        const char* userprofile = std::getenv("USERPROFILE");
        if (!userprofile)
            return std::unexpected(error_code::home_not_set);
        return fs::path(userprofile);
    }

}
