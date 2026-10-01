/**
 * @file windows_environment_tests.cpp
 * @brief Integrationstests für windows_system_environment
 * @author Martin Fehrs
 */

#include <filesystem>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <catch2/catch_all.hpp>

import pfadfinder;

namespace fs = std::filesystem;

TEST_CASE("windows_system_environment::executable_path returns correct path")
{
    pfadfinder::system_environment backend;
    auto exe_path = backend.executable_path();

    REQUIRE_FALSE(exe_path.empty());
    REQUIRE(exe_path.is_absolute());
    REQUIRE(exe_path.filename() == "test_pfadfinder.exe");
}

TEST_CASE("windows_system_environment::static_data_dir returns executable directory")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("C:/Program Files/MyApp");
    auto app_name = std::string("test_app");

    auto static_dir = backend.static_data_dir(exe_dir, app_name);
    // Unter Windows: Gleich dem Binärverzeichnis
    REQUIRE(static_dir == exe_dir);
}

TEST_CASE("windows_system_environment::data_dir returns APPDATA path")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("C:/Program Files/MyApp");
    auto app_name = std::string("test_app");

    auto data_dir = backend.data_dir(exe_dir, app_name);

    DWORD size = GetEnvironmentVariableA("APPDATA", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("APPDATA", buffer.data(), size);

    auto expected = fs::path(buffer) / "test_app";
    REQUIRE(data_dir == expected);
}

TEST_CASE("windows_system_environment::user_config_dir returns APPDATA path")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("C:/Program Files/MyApp");
    auto app_name = std::string("test_app");

    auto config_dir = backend.user_config_dir(exe_dir, app_name);

    DWORD size = GetEnvironmentVariableA("APPDATA", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("APPDATA", buffer.data(), size);

    auto expected = fs::path(buffer) / "test_app";
    REQUIRE(config_dir == expected);
}

TEST_CASE("windows_system_environment::cache_dir returns LOCALAPPDATA path with Cache subdirectory")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("C:/Program Files/MyApp");
    auto app_name = std::string("test_app");

    auto cache_dir = backend.cache_dir(exe_dir, app_name);

    DWORD size = GetEnvironmentVariableA("LOCALAPPDATA", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("LOCALAPPDATA", buffer.data(), size);

    auto expected = fs::path(buffer) / "test_app" / "Cache";
    REQUIRE(cache_dir == expected);
}

TEST_CASE("windows_system_environment::log_dir returns LOCALAPPDATA path with Logs subdirectory")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("C:/Program Files/MyApp");
    auto app_name = std::string("test_app");

    auto log_dir = backend.log_dir(exe_dir, app_name);

    DWORD size = GetEnvironmentVariableA("LOCALAPPDATA", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("LOCALAPPDATA", buffer.data(), size);

    auto expected = fs::path(buffer) / "test_app" / "Logs";
    REQUIRE(log_dir == expected);
}

TEST_CASE("windows_system_environment::temp_dir returns TEMP path with app subdirectory")
{
    pfadfinder::system_environment backend;
    auto app_name = std::string("test_app");

    auto temp_dir = backend.temp_dir(app_name);

    DWORD size = GetEnvironmentVariableA("TEMP", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("TEMP", buffer.data(), size);

    auto expected = fs::path(buffer) / "test_app";
    REQUIRE(temp_dir == expected);
}

TEST_CASE("windows_system_environment::home_dir returns USERPROFILE path")
{
    pfadfinder::system_environment backend;
    auto home_dir = backend.home_dir();

    DWORD size = GetEnvironmentVariableA("USERPROFILE", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("USERPROFILE", buffer.data(), size);

    REQUIRE(home_dir == fs::path(buffer));
}

TEST_CASE("windows_system_environment::shared_config_dir returns ALLUSERSAPPDATA path")
{
    pfadfinder::system_environment backend;
    auto app_name = std::string("test_app");

    auto shared_config_dir = backend.shared_config_dir(app_name);

    DWORD size = GetEnvironmentVariableA("ALLUSERSAPPDATA", nullptr, 0);
    REQUIRE(size != 0);

    std::string buffer(size - 1, '\0');
    GetEnvironmentVariableA("ALLUSERSAPPDATA", buffer.data(), size);

    auto expected = fs::path(buffer) / "test_app";
    REQUIRE(shared_config_dir == expected);
}

TEST_CASE("windows_system_environment::home_dir throws when USERPROFILE is not set")
{
    const char* original = std::getenv("USERPROFILE");
    SetEnvironmentVariableA("USERPROFILE", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.home_dir(), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("USERPROFILE", original);
}

TEST_CASE("windows_system_environment::data_dir throws when APPDATA is not set")
{
    const char* original = std::getenv("APPDATA");
    SetEnvironmentVariableA("APPDATA", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.data_dir(fs::path("C:/Program Files/MyApp"), "test_app"), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("APPDATA", original);
}

TEST_CASE("windows_system_environment::user_config_dir throws when APPDATA is not set")
{
    const char* original = std::getenv("APPDATA");
    SetEnvironmentVariableA("APPDATA", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.user_config_dir(fs::path("C:/Program Files/MyApp"), "test_app"), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("APPDATA", original);
}

TEST_CASE("windows_system_environment::cache_dir throws when LOCALAPPDATA is not set")
{
    const char* original = std::getenv("LOCALAPPDATA");
    SetEnvironmentVariableA("LOCALAPPDATA", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.cache_dir(fs::path("C:/Program Files/MyApp"), "test_app"), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("LOCALAPPDATA", original);
}

TEST_CASE("windows_system_environment::log_dir throws when LOCALAPPDATA is not set")
{
    const char* original = std::getenv("LOCALAPPDATA");
    SetEnvironmentVariableA("LOCALAPPDATA", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.log_dir(fs::path("C:/Program Files/MyApp"), "test_app"), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("LOCALAPPDATA", original);
}

TEST_CASE("windows_system_environment::temp_dir throws when TEMP is not set")
{
    const char* original = std::getenv("TEMP");
    SetEnvironmentVariableA("TEMP", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.temp_dir("test_app"), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("TEMP", original);
}

TEST_CASE("windows_system_environment::shared_config_dir throws when ALLUSERSAPPDATA is not set")
{
    const char* original = std::getenv("ALLUSERSAPPDATA");
    SetEnvironmentVariableA("ALLUSERSAPPDATA", nullptr);
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.shared_config_dir("test_app"), pfadfinder::environment_variable_not_set);
    
    SetEnvironmentVariableA("ALLUSERSAPPDATA", original);
}
