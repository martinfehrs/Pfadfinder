/**
 * @file
 * @brief Plattformspezifische Implementierung für macOS (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <mach-o/dyld.h>
#include <limits.h>

#include <expected>
#include <filesystem>
#include <string>

export module pfadfinder:system_backend;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Fehlercodes für macOS
    export enum class error_code
    {
        home_not_set,                     ///< Home-Verzeichnis nicht gesetzt
        macos_get_executable_path_failed, ///< _NSGetExecutablePath fehlgeschlagen
        macos_realpath_failed             ///< realpath fehlgeschlagen
    };

    export const char* error_message(error_code ec)
    {
        switch (ec)
        {
            case error_code::home_not_set:                     return "Home directory not set";
            case error_code::macos_get_executable_path_failed: return "_NSGetExecutablePath failed";
            case error_code::macos_realpath_failed:            return "realpath failed";
            default:                                           return "Unknown error";
        }
    }

    std::expected<fs::path, error_code> get_executable_path()
    {
        char path[PATH_MAX] = {0};
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0)
            return std::unexpected(error_code::macos_get_executable_path_failed);
        
        // Symbolische Links auflösen, um den tatsächlichen Pfad zu erhalten
        char real_path[PATH_MAX] = {0};
        if (realpath(path, real_path) == nullptr)
            return std::unexpected(error_code::macos_realpath_failed);
        
        return fs::path(real_path);
    }

    std::expected<fs::path, error_code> get_data_directory(const fs::path& exe_dir, const std::string& app_name)
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

    std::expected<fs::path, error_code> get_user_data_directory(const fs::path& exe_dir, const std::string& app_name)
    {
        std::string exe_dir_str = exe_dir.string();
        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
        {
            // Bundle: ~/Library/Application Support/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / "Library" / "Application Support" / app_name;
        }
        else
        {
            // Nicht gebündelt: ~/.local/share/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / ".local" / "share" / app_name;
        }
    }

    std::expected<fs::path, error_code> get_config_directory(const fs::path& exe_dir, const std::string& app_name)
    {
        std::string exe_dir_str = exe_dir.string();
        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
        {
            // Bundle: ~/Library/Preferences/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / "Library" / "Preferences" / app_name;
        }
        else
        {
            // Nicht gebündelt: ~/.config/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / ".config" / app_name;
        }
    }

    std::expected<fs::path, error_code> get_cache_directory(const fs::path& exe_dir, const std::string& app_name)
    {
        std::string exe_dir_str = exe_dir.string();
        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
        {
            // Bundle: ~/Library/Caches/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / "Library" / "Caches" / app_name;
        }
        else
        {
            // Nicht gebündelt: ~/.cache/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / ".cache" / app_name;
        }
    }

    std::expected<fs::path, error_code> get_log_directory(const fs::path& exe_dir, const std::string& app_name)
    {
        std::string exe_dir_str = exe_dir.string();
        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
        {
            // Bundle: ~/Library/Logs/<appname>
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / "Library" / "Logs" / app_name;
        }
        else
        {
            // Nicht gebündelt: ~/.local/state/<appname>/log (XDG-konform)
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error_code::home_not_set);
            return fs::path(home) / ".local" / "state" / app_name / "log";
        }
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
