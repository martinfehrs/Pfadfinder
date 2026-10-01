/**
 * @file
 * @brief Plattformspezifische Implementierung für Windows (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
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

    fs::path system_environment::data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        DWORD size = GetEnvironmentVariableA("APPDATA", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "APPDATA" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("APPDATA", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "APPDATA" };

        return fs::path{ buffer } / app_name;
    }

    fs::path system_environment::user_config_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        DWORD size = GetEnvironmentVariableA("APPDATA", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "APPDATA" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("APPDATA", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "APPDATA" };

        return fs::path{ buffer } / app_name;
    }

    fs::path system_environment::cache_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        DWORD size = GetEnvironmentVariableA("LOCALAPPDATA", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "LOCALAPPDATA" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("LOCALAPPDATA", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "LOCALAPPDATA" };

        return fs::path{ buffer } / app_name / "Cache";
    }

    fs::path system_environment::log_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const
    {
        DWORD size = GetEnvironmentVariableA("LOCALAPPDATA", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "LOCALAPPDATA" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("LOCALAPPDATA", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "LOCALAPPDATA" };

        return fs::path{ buffer } / app_name / "Logs";
    }

    fs::path system_environment::temp_dir(const std::string& app_name) const
    {
        DWORD size = GetEnvironmentVariableA("TEMP", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "TEMP" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("TEMP", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "TEMP" };

        return fs::path{ buffer } / app_name;
    }

    fs::path system_environment::home_dir() const
    {
        DWORD size = GetEnvironmentVariableA("USERPROFILE", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "USERPROFILE" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("USERPROFILE", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "USERPROFILE" };

        return fs::path{ buffer };
    }

    fs::path system_environment::shared_config_dir(const std::string& app_name) const
    {
        DWORD size = GetEnvironmentVariableA("ALLUSERSAPPDATA", nullptr, 0);

        if (size == 0)
            throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

        std::string buffer(size - 1, '\0');
        if (GetEnvironmentVariableA("ALLUSERSAPPDATA", buffer.data(), size) == 0)
            throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

        return fs::path{ buffer } / app_name;
    }
}
