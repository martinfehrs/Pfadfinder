/**
 * @file macos_environment_tests.cpp
 * @brief Integrationstests für macos_system_environment
 * @author Martin Fehrs
 */

#include <filesystem>
#include <string>

#include <unistd.h>

#include <catch2/catch_all.hpp>

import pfadfinder;

namespace fs = std::filesystem;

TEST_CASE("macos_system_environment::executable_path returns correct path")
{
    pfadfinder::system_environment backend;
    auto exe_path = backend.executable_path();

    REQUIRE_FALSE(exe_path.empty());
    REQUIRE(exe_path.is_absolute());
    REQUIRE(exe_path.filename() == "test_pfadfinder");
}

TEST_CASE("macos_system_environment::static_data_dir returns correct path for CLI")
{
    pfadfinder::system_environment backend;
    // CLI-App: exe_dir ist nicht in einem Bundle
    auto exe_dir = fs::path("/usr/local/bin");
    auto app_name = std::string("test_app");

    auto static_dir = backend.static_data_dir(exe_dir, app_name);
    // Für CLI-Apps: parent_path()/share/app_name
    REQUIRE(static_dir == fs::path("/usr/local/share/test_app"));
}

TEST_CASE("macos_system_environment::static_data_dir returns correct path for bundle")
{
    pfadfinder::system_environment backend;
    // Bundle-App: exe_dir enthält Contents/MacOS
    auto exe_dir = fs::path("/Applications/TestApp.app/Contents/MacOS");
    auto app_name = std::string("test_app");

    auto static_dir = backend.static_data_dir(exe_dir, app_name);
    // Für Bundles: Contents/MacOS/../Resources/app_name
    REQUIRE(static_dir == fs::path("/Applications/TestApp.app/Contents/Resources/test_app"));
}

TEST_CASE("macos_system_environment::data_dir returns correct path for CLI")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/usr/local/bin");
    auto app_name = std::string("test_app");

    auto data_dir = backend.data_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für CLI-Apps: ~/.local/share/app_name
    auto expected = fs::path(home) / ".local" / "share" / "test_app";
    REQUIRE(data_dir == expected);
}

TEST_CASE("macos_system_environment::data_dir returns correct path for bundle")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/Applications/TestApp.app/Contents/MacOS");
    auto app_name = std::string("test_app");

    auto data_dir = backend.data_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für Bundles: ~/Library/Application Support/app_name
    auto expected = fs::path(home) / "Library" / "Application Support" / "test_app";
    REQUIRE(data_dir == expected);
}

TEST_CASE("macos_system_environment::user_config_dir returns correct path for CLI")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/usr/local/bin");
    auto app_name = std::string("test_app");

    auto config_dir = backend.user_config_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für CLI-Apps: ~/.config/app_name
    auto expected = fs::path(home) / ".config" / "test_app";
    REQUIRE(config_dir == expected);
}

TEST_CASE("macos_system_environment::user_config_dir returns correct path for bundle")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/Applications/TestApp.app/Contents/MacOS");
    auto app_name = std::string("test_app");

    auto config_dir = backend.user_config_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für Bundles: ~/Library/Preferences/app_name
    auto expected = fs::path(home) / "Library" / "Preferences" / "test_app";
    REQUIRE(config_dir == expected);
}

TEST_CASE("macos_system_environment::cache_dir returns correct path for CLI")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/usr/local/bin");
    auto app_name = std::string("test_app");

    auto cache_dir = backend.cache_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für CLI-Apps: ~/.cache/app_name
    auto expected = fs::path(home) / ".cache" / "test_app";
    REQUIRE(cache_dir == expected);
}

TEST_CASE("macos_system_environment::cache_dir returns correct path for bundle")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/Applications/TestApp.app/Contents/MacOS");
    auto app_name = std::string("test_app");

    auto cache_dir = backend.cache_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für Bundles: ~/Library/Caches/app_name
    auto expected = fs::path(home) / "Library" / "Caches" / "test_app";
    REQUIRE(cache_dir == expected);
}

TEST_CASE("macos_system_environment::log_dir returns correct path for CLI")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/usr/local/bin");
    auto app_name = std::string("test_app");

    auto log_dir = backend.log_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für CLI-Apps: ~/.local/state/app_name/log
    auto expected = fs::path(home) / ".local" / "state" / "test_app" / "log";
    REQUIRE(log_dir == expected);
}

TEST_CASE("macos_system_environment::log_dir returns correct path for bundle")
{
    pfadfinder::system_environment backend;
    auto exe_dir = fs::path("/Applications/TestApp.app/Contents/MacOS");
    auto app_name = std::string("test_app");

    auto log_dir = backend.log_dir(exe_dir, app_name);

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    // Für Bundles: ~/Library/Logs/app_name
    auto expected = fs::path(home) / "Library" / "Logs" / "test_app";
    REQUIRE(log_dir == expected);
}

TEST_CASE("macos_system_environment::temp_dir returns correct path")
{
    pfadfinder::system_environment backend;
    auto app_name = std::string("test_app");

    auto temp_dir = backend.temp_dir(app_name);

    auto expected = fs::temp_directory_path() / "test_app";
    REQUIRE(temp_dir == expected);
}

TEST_CASE("macos_system_environment::home_dir returns correct path")
{
    pfadfinder::system_environment backend;
    auto home_dir = backend.home_dir();

    const char* home = std::getenv("HOME");
    REQUIRE(home != nullptr);

    REQUIRE(home_dir == fs::path(home));
}

TEST_CASE("macos_system_environment::shared_config_dir returns /Library/Preferences path")
{
    pfadfinder::system_environment backend;
    auto app_name = std::string("test_app");

    auto shared_config_dir = backend.shared_config_dir(app_name);
    REQUIRE(shared_config_dir == fs::path("/Library/Preferences/test_app"));
}

TEST_CASE("macos_system_environment::home_dir throws when HOME is not set")
{
    // Save original HOME
    const char* original_home = std::getenv("HOME");
    
    // Temporarily unset HOME
    unsetenv("HOME");
    
    pfadfinder::system_environment backend;
    REQUIRE_THROWS_AS(backend.home_dir(), pfadfinder::environment_variable_not_set);
    
    // Restore HOME
    if (original_home)
        setenv("HOME", original_home, 1);
}
