/**
 * @file
 * @brief Plattformspezifische Implementierung für macOS (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#include <mach-o/dyld.h>

#include <filesystem>
#include <climits>
#include <string>

module pfadfinder;

import :unix;

namespace fs = std::filesystem;

namespace pfadfinder
{
    [[nodiscard]] bool is_macos_bundle(const fs::path& exe_dir)
    {
        const auto exe_dir_str = exe_dir.string();
        return exe_dir_str.find("Contents/MacOS") != std::string::npos;
    }

    [[nodiscard]] fs::path get_macos_user_data_dir(const std::string& app_name)
    {
        return get_home_dir() / "Library" / "Application Support" / app_name;
    }

    [[nodiscard]] fs::path get_macos_user_config_dir(const std::string& app_name)
    {
        return get_home_dir() / "Library" / "Preferences" / app_name;
    }

    [[nodiscard]] fs::path get_macos_user_cache_dir(const std::string& app_name)
    {
        return get_home_dir() / "Library" / "Caches" / app_name;
    }

    [[nodiscard]] fs::path get_macos_user_log_dir(const std::string& app_name)
    {
        return get_home_dir() / "Library" / "Logs" / app_name;
    }

    [[nodiscard]] fs::path get_macos_bundle_static_data_dir(const fs::path& exe_dir, const std::string& app_name)
    {
        return exe_dir.parent_path().parent_path() / "Resources" / app_name;
    }

    fs::path system_environment::executable_path() const
    {
        char path[PATH_MAX]{};
        uint32_t size = sizeof(path);

        if (_NSGetExecutablePath(path, &size) != 0)
            throw indeterminable_exe_path{};
        
        char real_path[PATH_MAX]{};

        if (realpath(path, real_path) == nullptr)
            throw indeterminable_exe_path{};
        
        return fs::path{ real_path };
    }

    fs::path system_environment::static_data_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return is_macos_bundle(exe_dir)
            ? get_macos_bundle_static_data_dir(exe_dir, app_name)
            : get_xdg_static_data_dir(exe_dir, app_name);
    }

    fs::path system_environment::shared_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return fs::path{ "/Library/Application Support" } / app_name;
    }

    fs::path system_environment::user_data_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return is_macos_bundle(exe_dir)
            ? get_macos_user_data_dir(app_name)
            : get_xdg_user_data_dir(app_name);
    }

    fs::path system_environment::user_config_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return is_macos_bundle(exe_dir)
            ? get_macos_user_config_dir(app_name)
            : get_xdg_user_config_dir(app_name);
    }

    fs::path system_environment::user_cache_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return is_macos_bundle(exe_dir)
            ? get_macos_user_cache_dir(app_name)
            : get_xdg_user_cache_dir(app_name);
    }

    fs::path system_environment::user_log_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return is_macos_bundle(exe_dir)
            ? get_macos_user_log_dir(app_name)
            : get_xdg_user_log_dir(app_name);
    }

    fs::path system_environment::temp_dir(const std::string& app_name) const
    {
        return get_temp_dir(app_name);
    }

    fs::path system_environment::user_dir() const
    {
        return get_home_dir();
    }

    fs::path system_environment::shared_cache_dir(const std::string& app_name) const
    {
        return fs::path{ "/Library/Caches" } / app_name;
    }

    fs::path system_environment::shared_log_dir(const std::string& app_name) const
    {
        return fs::path{ "/Library/Logs" } / app_name;
    }

    fs::path system_environment::shared_config_dir(const std::string& app_name) const
    {
        return fs::path{ "/Library/Preferences" } / app_name;
    }
}
