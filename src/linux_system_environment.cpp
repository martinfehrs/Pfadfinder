/**
 * @file
 * @brief Plattformspezifische Implementierung für Linux (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#include <unistd.h>
#include <linux/limits.h>

#include <filesystem>
#include <optional>
#include <string>

module pfadfinder;

import :unix;

namespace fs = std::filesystem;

namespace pfadfinder
{
    std::optional<fs::path> default_system_environment::executable_path() const
    {
        std::array<char, PATH_MAX> path{};
        ssize_t len = readlink("/proc/self/exe", path.data(), std::size(path) - 1);

        if (len == -1)
            return std::nullopt;

        return fs::path{ path.data() };
    }

    fs::path default_system_environment::static_data_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_static_data_dir(exe_dir, app_name);
    }

    std::optional<fs::path> default_system_environment::data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_data_dir(app_name);
    }

    std::optional<fs::path> default_system_environment::user_config_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_config_dir(app_name);
    }

    std::optional<fs::path> default_system_environment::cache_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_cache_dir(app_name);
    }

    std::optional<fs::path> default_system_environment::log_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_log_dir(app_name);
    }

    std::optional<fs::path> default_system_environment::temp_dir(const std::string& app_name) const
    {
        return get_temp_dir(app_name);
    }

    std::optional<fs::path> default_system_environment::home_dir() const
    {
        return get_home_dir();
    }

    std::optional<fs::path> default_system_environment::shared_config_dir(const std::string& app_name) const
    {
        return fs::path{ "/etc" } / app_name;
    }
}
