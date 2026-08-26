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
#include <string>

#include <unistd.h>

module pfadfinder:unix;

import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    [[nodiscard]] fs::path get_home_dir()
    {
        const char* home = std::getenv("HOME");

        if (!home)
            throw environment_variable_not_set{ "HOME" };

        return fs::path{ home };
    }

    [[nodiscard]] fs::path get_temp_dir(const std::string& app_name)
    {
        return fs::temp_directory_path() / app_name;
    }

    [[nodiscard]] fs::path get_xdg_user_data_dir(const std::string& app_name)
    {
        return get_home_dir() / ".local" / "share" / app_name;
    }

    [[nodiscard]] fs::path get_xdg_user_config_dir(const std::string& app_name)
    {
        return get_home_dir() / ".config" / app_name;
    }

    [[nodiscard]] fs::path get_xdg_user_cache_dir(const std::string& app_name)
    {
        return get_home_dir() / ".cache" / app_name;
    }

    [[nodiscard]] fs::path get_xdg_user_log_dir(const std::string& app_name)
    {
        return get_home_dir() / ".local" / "state" / app_name / "log";
    }

    [[nodiscard]] fs::path get_xdg_static_data_dir(const fs::path& exe_dir, const std::string& app_name)
    {
        return exe_dir.parent_path() / "share" / app_name;
    }
}
