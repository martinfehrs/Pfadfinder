/**
 * @file
 * @brief Plattformspezifische Implementierung für Linux (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <unistd.h>
#include <limits.h>

#include <expected>
#include <filesystem>
#include <string>

export module pfadfinder:system_backend;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Fehlercodes für Linux
    export enum class error_code
    {
        home_not_set,         ///< Home-Verzeichnis nicht gesetzt
        linux_readlink_failed ///< readlink /proc/self/exe fehlgeschlagen
    };

    export const char* error_message(error_code ec)
    {
        switch (ec)
        {
            case error_code::home_not_set:          return "Home directory not set";
            case error_code::linux_readlink_failed: return "readlink failed";
            default:                                return "Unknown error";
        }
    }

    std::expected<fs::path, error_code> get_executable_path()
    {
        char path[PATH_MAX] = {0};
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len == -1)
            return std::unexpected(error_code::linux_readlink_failed);
        path[len] = '\0';
        return fs::path(path);
    }

    std::expected<fs::path, error_code> get_data_directory(const fs::path& exe_dir, const std::string& app_name)
    {
        // Linux: von /usr/bin/myapp zu /usr/share/myapp
        return exe_dir.parent_path() / "share" / app_name;
    }

    std::expected<fs::path, error_code> get_user_data_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* home = std::getenv("HOME");
        if (!home)
            return std::unexpected(error_code::home_not_set);
        return fs::path(home) / ".local" / "share" / app_name;
    }

    std::expected<fs::path, error_code> get_config_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* home = std::getenv("HOME");
        if (!home)
            return std::unexpected(error_code::home_not_set);
        return fs::path(home) / ".config" / app_name;
    }

    std::expected<fs::path, error_code> get_cache_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* home = std::getenv("HOME");
        if (!home)
            return std::unexpected(error_code::home_not_set);
        return fs::path(home) / ".cache" / app_name;
    }

    std::expected<fs::path, error_code> get_log_directory(const fs::path& /*exe_dir*/, const std::string& app_name)
    {
        const char* home = std::getenv("HOME");
        if (!home)
            return std::unexpected(error_code::home_not_set);
        return fs::path(home) / ".local" / "state" / app_name / "log";
    }

    std::expected<fs::path, error_code> get_system_temp_directory()
    {
        return fs::temp_directory_path();
    }

    std::expected<fs::path, error_code> get_user_directory()
    {
        const char* home = std::getenv("HOME");
        if (!home)
            return std::unexpected(error_code::home_not_set);
        return fs::path(home);
    }

}
