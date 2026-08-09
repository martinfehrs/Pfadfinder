/**
 * @file
 * @brief Plattformspezifische Implementierung für Windows (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <filesystem>
#include <string>

module pfadfinder;

namespace fs = std::filesystem;

namespace pfadfinder
{
    fs::path system_environment::executable_path() const
    {
        wchar_t path[MAX_PATH]{};

        if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
            throw indeterminable_exe_path{};

        return fs::path{ path };
    }

    fs::path system_environment::static_data_dir(const fs::path& exe_dir, const std::string& app_name) const
    {
        return exe_dir;
    }

    fs::path system_environment::shared_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

        if (!allusersappdata)
            throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

        return fs::path{ allusersappdata } / app_name;
    }

    fs::path system_environment::user_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        const char* appdata = std::getenv("APPDATA");

        if (!appdata)
            throw environment_variable_not_set{ "APPDATA" };

        return fs::path{ appdata } / app_name;
    }

    fs::path system_environment::user_config_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        const char* appdata = std::getenv("APPDATA");

        if (!appdata)
            throw environment_variable_not_set{ "APPDATA" };

        return fs::path{ appdata } / app_name;
    }

    fs::path system_environment::user_cache_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        const char* localappdata = std::getenv("LOCALAPPDATA");

        if (!localappdata)
            throw environment_variable_not_set{ "LOCALAPPDATA" };

        return fs::path{ localappdata } / app_name / "Cache";
    }

    fs::path system_environment::user_log_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        const char* localappdata = std::getenv("LOCALAPPDATA");

        if (!localappdata)
            throw environment_variable_not_set{ "LOCALAPPDATA" };

        return fs::path{ localappdata } / app_name / "Logs";
    }

    fs::path system_environment::temp_dir(const std::string& app_name) const
    {
        const char* temp = std::getenv("TEMP");

        if (!temp)
            throw environment_variable_not_set{ "TEMP" };

        return fs::path{ temp } / app_name;
    }

    fs::path system_environment::user_dir() const
    {
        const char* userprofile = std::getenv("USERPROFILE");

        if (!userprofile)
            throw environment_variable_not_set{ "USERPROFILE" };

        return fs::path{ userprofile };
    }

    fs::path system_environment::shared_cache_dir(const std::string& app_name) const
    {
        const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

        if (!allusersappdata)
            throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

        return fs::path{ allusersappdata } / app_name / "Cache";
    }

    fs::path system_environment::shared_log_dir(const std::string& app_name) const
    {
        const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

        if (!allusersappdata)
            throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

        return fs::path{ allusersappdata } / app_name / "Logs";
    }

    fs::path system_environment::shared_config_dir(const std::string& app_name) const
    {
        const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

        if (!allusersappdata)
            throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

        return fs::path{ allusersappdata } / app_name;
    }
}
