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

module pfadfinder

namespace fs = std::filesystem;

namespace pfadfinder
{
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
        const auto exe_dir_str = exe_dir.string();

        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            return exe_dir.parent_path().parent_path() / "Resources" / app_name;
        else
            return exe_dir.parent_path() / "share" / app_name;
    }

    fs::path system_environment::shared_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        return fs::path{ "/Library/Application Support" } / app_name;
    }

    fs::path system_environment::user_data_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        const auto exe_dir_str = exe_dir.string();

        const char* home = std::getenv("HOME");

        if (!home)
            throw environment_variable_not_set{ "HOME" };

        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            return fs::path{ home } / "Library" / "Application Support" / app_name;
        else
            return fs::path{ home } / ".local" / "share" / app_name;
    }

    fs::path system_environment::user_config_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        const auto exe_dir_str = exe_dir.string();

        const char* home = std::getenv("HOME");

        if (!home)
            throw environment_variable_not_set{ "HOME" };

        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            return fs::path{ home } / "Library" / "Preferences" / app_name;
        else
            return fs::path{ home } / ".config" / app_name;
    }

    fs::path system_environment::user_cache_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        const auto exe_dir_str = exe_dir.string();

        const char* home = std::getenv("HOME");

        if (!home)
            throw environment_variable_not_set{ "HOME" };

        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            return fs::path{ home } / "Library" / "Caches" / app_name;
        else
            return fs::path{ home } / ".cache" / app_name;
    }

    fs::path system_environment::user_log_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        const auto exe_dir_str = exe_dir.string();

        const char* home = std::getenv("HOME");

        if (!home)
            throw environment_variable_not_set{ "HOME" };

        if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            return fs::path{ home } / "Library" / "Logs" / app_name;
        else
            return fs::path{ home } / ".local" / "state" / app_name / "log";
    }

    fs::path system_environment::temp_dir(const std::string& app_name) const
    {
        return fs::temp_directory_path() / app_name;
    }

    fs::path system_environment::user_dir() const
    {
        const char* home = std::getenv("HOME");

        if (!home)
            throw environment_variable_not_set{ "HOME" };

        return fs::path{ home };
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
