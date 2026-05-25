/**
 * @file test_pfadfinder.cpp
 * @brief Testanwendung für das pfadfinder-Modul mit CATCH2.
 * @author Martin Fehrs
 */

// Standardbibliotheks-Header
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <string>

// CATCH2 Header (single-include version)
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Import des zu testenden Moduls
import pfadfinder;

// Tests für das pfadfinder-Modul
TEST_CASE("pfadfinder::application_environment: Pfadfunktionen", "[pfadfinder]") {
    // Erstelle eine application_environment Instanz mit Test-App-Name
    const std::string test_app_name = "test_app";
    pfadfinder::application_environment env(test_app_name);
    
    SECTION("executable_path gibt einen gültigen Pfad zurück") {
        auto path_result = env.executable_path();
        REQUIRE(path_result.has_value());
        
        auto path = path_result.value();
        REQUIRE_FALSE(path.empty());
        REQUIRE(path.is_absolute());
        REQUIRE(std::filesystem::exists(path));
    }
    
    SECTION("executable_path gibt Fehler zurück auf unsupported Plattform") {
        // Dieser Test kann nur auf nicht unterstützten Plattformen getestet werden
        // Hier nur als Dokumentation
        // auto path_result = env.executable_path();
        // REQUIRE_FALSE(path_result.has_value());
        // REQUIRE(path_result.error() == pfadfinder::error::platform_not_supported);
    }
    
    SECTION("executable_directory gibt ein gültiges Verzeichnis zurück") {
        auto dir_result = env.executable_directory();
        REQUIRE(dir_result.has_value());
        
        auto dir = dir_result.value();
        REQUIRE_FALSE(dir.empty());
        REQUIRE(dir.is_absolute());
        REQUIRE(std::filesystem::exists(dir));
        REQUIRE(std::filesystem::is_directory(dir));
    }
    
    SECTION("executable_directory ist das Elternverzeichnis von executable_path") {
        auto exe_path_result = env.executable_path();
        auto exe_dir_result = env.executable_directory();
        
        REQUIRE(exe_path_result.has_value());
        REQUIRE(exe_dir_result.has_value());
        
        auto exe_path = exe_path_result.value();
        auto exe_dir = exe_dir_result.value();
        
        REQUIRE(exe_dir == exe_path.parent_path());
    }

    SECTION("data_directory gibt einen gültigen Pfad zurück") {
        auto data_dir_result = env.data_directory();
        REQUIRE(data_dir_result.has_value());
        
        auto data_dir = data_dir_result.value();
        REQUIRE_FALSE(data_dir.empty());
        REQUIRE(data_dir.is_absolute());
    }

    SECTION("data_directory enthält den app_name") {
#if defined(__linux__)
        auto data_dir_result = env.data_directory();
        REQUIRE(data_dir_result.has_value());
        auto data_dir = data_dir_result.value();
        // Unter Linux sollte das Datenverzeichnis den App-Namen enthalten
        REQUIRE(data_dir.filename() == test_app_name);
#elif defined(_WIN32)
        auto exe_dir_result = env.executable_directory();
        auto data_dir_result = env.data_directory();
        REQUIRE(exe_dir_result.has_value());
        REQUIRE(data_dir_result.has_value());
        auto exe_dir = exe_dir_result.value();
        auto data_dir = data_dir_result.value();
        // Unter Windows: data_dir sollte exe_dir/test_app sein
        REQUIRE(data_dir == exe_dir / test_app_name);
#endif
    }

    SECTION("user_data_directory gibt einen gültigen Pfad zurück") {
        auto user_dir_result = env.user_data_directory();
        REQUIRE(user_dir_result.has_value());
        
        auto user_dir = user_dir_result.value();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(user_dir.filename() == test_app_name);
    }

    SECTION("user_data_directory enthält HOME oder APPDATA im Pfad") {
#if defined(_WIN32)
        auto user_dir_result = env.user_data_directory();
        REQUIRE(user_dir_result.has_value());
        auto user_dir = user_dir_result.value();
        std::string user_dir_str = user_dir.string();
        // Unter Windows sollte APPDATA im Pfad enthalten sein
        const char* appdata = std::getenv("APPDATA");
        if (appdata)
        {
            REQUIRE(user_dir_str.find(appdata) != std::string::npos);
        }
#elif defined(__linux__) || defined(__APPLE__)
        auto user_dir_result = env.user_data_directory();
        REQUIRE(user_dir_result.has_value());
        auto user_dir = user_dir_result.value();
        std::string user_dir_str = user_dir.string();
        const char* home = std::getenv("HOME");
        if (home)
        {
            REQUIRE(user_dir_str.find(home) != std::string::npos);
        }
#endif
    }

    SECTION("config_directory gibt einen gültigen Pfad zurück") {
        auto config_dir_result = env.config_directory();
        REQUIRE(config_dir_result.has_value());
        
        auto config_dir = config_dir_result.value();
        REQUIRE_FALSE(config_dir.empty());
        REQUIRE(config_dir.is_absolute());
        REQUIRE(config_dir.filename() == test_app_name);
    }

    SECTION("cache_directory gibt einen gültigen Pfad zurück") {
        auto cache_dir_result = env.cache_directory();
        REQUIRE(cache_dir_result.has_value());
        
        auto cache_dir = cache_dir_result.value();
        REQUIRE_FALSE(cache_dir.empty());
        REQUIRE(cache_dir.is_absolute());
        
#if defined(__linux__) || (defined(__APPLE__) && !defined(__BUNDLE__))
        // Unter Linux und macOS CLI: ~/.cache/<name>
        REQUIRE(cache_dir.filename() == test_app_name);
#elif defined(_WIN32)
        // Unter Windows: %LOCALAPPDATA%/<name>/Cache
        REQUIRE(cache_dir.parent_path().filename() == test_app_name);
#endif
    }

    SECTION("log_directory gibt einen gültigen Pfad zurück") {
        auto log_dir_result = env.log_directory();
        REQUIRE(log_dir_result.has_value());
        
        auto log_dir = log_dir_result.value();
        REQUIRE_FALSE(log_dir.empty());
        REQUIRE(log_dir.is_absolute());
        
#if defined(_WIN32)
        // Unter Windows: %LOCALAPPDATA%/<name>/Logs
        REQUIRE(log_dir.parent_path().filename() == test_app_name);
        REQUIRE(log_dir.filename() == "Logs");
#elif defined(__linux__)
        // Unter Linux: ~/.local/state/<name>/log
        REQUIRE(log_dir.filename() == "log");
        REQUIRE(log_dir.parent_path().filename() == test_app_name);
#elif defined(__APPLE__)
        // Unter macOS: Bundle -> ~/Library/Logs/<name>, CLI -> ~/.local/state/<name>/log
        std::string log_dir_str = log_dir.string();
        const char* home = std::getenv("HOME");
        if (home)
        {
            REQUIRE(log_dir_str.find(home) != std::string::npos);
        }
        REQUIRE(log_dir.filename() == test_app_name || log_dir.parent_path().filename() == "Logs");
#endif
    }

    SECTION("log_directory enthält den app_name") {
        auto log_dir_result = env.log_directory();
        REQUIRE(log_dir_result.has_value());
        
        auto log_dir = log_dir_result.value();
        std::string log_dir_str = log_dir.string();
        REQUIRE(log_dir_str.find(test_app_name) != std::string::npos);
    }

    SECTION("temp_directory gibt einen gültigen Pfad zurück") {
        auto temp_dir_result = env.temp_directory();
        REQUIRE(temp_dir_result.has_value());
        
        auto temp_dir = temp_dir_result.value();
        REQUIRE_FALSE(temp_dir.empty());
        REQUIRE(temp_dir.is_absolute());
        REQUIRE(temp_dir.filename() == test_app_name);
    }

    SECTION("temp_directory liegt im System-Temp-Verzeichnis") {
        auto temp_dir_result = env.temp_directory();
        REQUIRE(temp_dir_result.has_value());
        
        auto temp_dir = temp_dir_result.value();
        auto system_temp = std::filesystem::temp_directory_path();
        
        // Der Pfad sollte im System-Temp-Verzeichnis oder einem Unterverzeichnis liegen
        REQUIRE(temp_dir.string().find(system_temp.string()) != std::string::npos);
    }

    SECTION("user_directory gibt einen gültigen Pfad zurück") {
        auto user_dir_result = env.user_directory();
        REQUIRE(user_dir_result.has_value());
        
        auto user_dir = user_dir_result.value();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(std::filesystem::exists(user_dir));
        REQUIRE(std::filesystem::is_directory(user_dir));
    }

    SECTION("user_directory entspricht HOME oder USERPROFILE") {
        auto user_dir_result = env.user_directory();
        REQUIRE(user_dir_result.has_value());
        
        auto user_dir = user_dir_result.value();
        
#if defined(_WIN32)
        const char* expected_home = std::getenv("USERPROFILE");
#elif defined(__linux__) || defined(__APPLE__)
        const char* expected_home = std::getenv("HOME");
#endif
        
        if (expected_home)
        {
            REQUIRE(user_dir == std::filesystem::path(expected_home));
        }
    }

    SECTION("error_message gibt korrekte Fehlermeldungen zurück")
    {
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::platform_not_supported)) ==
            "Platform not supported");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::windows_get_module_file_name_failed)) ==
            "GetModuleFileNameW failed");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::appdata_not_set)) ==
            "APPDATA environment variable not set");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::localappdata_not_set)) ==
            "LOCALAPPDATA environment variable not set");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::macos_get_executable_path_failed)) ==
            "_NSGetExecutablePath failed");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::macos_realpath_failed)) ==
            "realpath failed");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::linux_readlink_failed)) ==
            "readlink /proc/self/exe failed");
        REQUIRE(
            std::string(pfadfinder::error_message(pfadfinder::error::home_not_set)) ==
            "HOME environment variable not set");
    }

    SECTION("Mehrere Instanzen mit verschiedenen app_names") {
        pfadfinder::application_environment env1("app1");
        pfadfinder::application_environment env2("app2");
        
        auto dir1_result = env1.data_directory();
        auto dir2_result = env2.data_directory();
        REQUIRE(dir1_result.has_value());
        REQUIRE(dir2_result.has_value());
        
        // Jede Instanz sollte ihren eigenen app_name verwenden
        REQUIRE(dir1_result.value().filename() == "app1");
        REQUIRE(dir2_result.value().filename() == "app2");
    }
}
