/**
 * @file
 * @brief Gemeinsame UNIX-Implementierungen für pfadfinder (linux und macOS)
 * @author Martin Fehrs
 *
 * Dieses Modul enthält plattformübergreifende UNIX-spezifische Implementierungen,
 * die sowohl von Linux als auch von macOS verwendet werden.
 */

module;

#include <filesystem>
#include <optional>
#include <string>

#include <unistd.h>

module pfadfinder:unix;

import :errors;

namespace fs = std::filesystem;

namespace pfadfinder
{
    [[nodiscard]] std::optional<fs::path> get_home_dir()
    {
        const char* home = std::getenv("HOME");

        if (!home)
            return std::nullopt;

        return fs::path{ home };
    }

    [[nodiscard]] fs::path get_temp_dir(const std::string& app_name)
    {
        return fs::temp_directory_path() / app_name;
    }

    [[nodiscard]] std::optional<fs::path> get_unix_user_data_dir(const std::string& app_name)
    {
        auto home = get_home_dir();
        if (!home.has_value())
            return std::nullopt;

        return *home / ".local" / "share" / app_name;
    }

    [[nodiscard]] std::optional<fs::path> get_unix_user_config_dir(const std::string& app_name)
    {
        auto home = get_home_dir();
        if (!home.has_value())
            return std::nullopt;

        return *home / ".config" / app_name;
    }

    [[nodiscard]] std::optional<fs::path> get_unix_user_cache_dir(const std::string& app_name)
    {
        auto home = get_home_dir();
        if (!home.has_value())
            return std::nullopt;

        return *home / ".cache" / app_name;
    }

    [[nodiscard]] std::optional<fs::path> get_unix_user_log_dir(const std::string& app_name)
    {
        auto home = get_home_dir();
        if (!home.has_value())
            return std::nullopt;

        return *home / ".local" / "state" / app_name / "log";
    }

    [[nodiscard]] fs::path get_unix_static_data_dir(const fs::path& exe_dir, const std::string& app_name)
    {
        return exe_dir.parent_path() / "share" / app_name;
    }
}
