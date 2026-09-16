/**
 * @file
 * @brief Plattformspezifische Implementierung für Linux (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#include <unistd.h>

#include <filesystem>
#include <climits>
#include <string>

module pfadfinder;

import :unix;

namespace fs = std::filesystem;

namespace pfadfinder
{
    fs::path system_environment::executable_path() const
    {
        char path[PATH_MAX]{};
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);

        if (len == -1)
            throw indeterminable_exe_path{};

        path[len] = '\0';

        return fs::path{ path };
    }

    fs::path system_environment::static_data_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_static_data_dir(exe_dir, app_name);
    }

    fs::path system_environment::data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_data_dir(app_name);
    }

    fs::path system_environment::user_config_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_config_dir(app_name);
    }

    fs::path system_environment::cache_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_cache_dir(app_name);
    }

    fs::path system_environment::log_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return get_xdg_user_log_dir(app_name);
    }

    fs::path system_environment::temp_dir(const std::string& app_name) const
    {
        return get_temp_dir(app_name);
    }

    fs::path system_environment::home_dir() const
    {
        return get_home_dir();
    }

    fs::path system_environment::shared_config_dir(const std::string& app_name) const
    {
        return fs::path{ "/etc" } / app_name;
    }
}
