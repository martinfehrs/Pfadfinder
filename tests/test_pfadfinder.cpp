/**
 * @file test_pfadfinder.cpp
 * @brief Testanwendung für das pfadfinder-Modul mit CATCH2.
 * @author Martin Fehrs
 */

// Standardbibliotheks-Header
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
        auto path = env.executable_path();
        REQUIRE_FALSE(path.empty());
        REQUIRE(path.is_absolute());
        REQUIRE(std::filesystem::exists(path));
    }
    
    SECTION("executable_directory gibt ein gültiges Verzeichnis zurück") {
        auto dir = env.executable_directory();
        REQUIRE_FALSE(dir.empty());
        REQUIRE(dir.is_absolute());
        REQUIRE(std::filesystem::exists(dir));
        REQUIRE(std::filesystem::is_directory(dir));
    }
    
    SECTION("executable_directory ist das Elternverzeichnis von executable_path") {
        auto exe_path = env.executable_path();
        auto exe_dir = env.executable_directory();
        REQUIRE(exe_dir == exe_path.parent_path());
    }

    SECTION("data_directory gibt einen gültigen Pfad zurück") {
        auto data_dir = env.data_directory();
        REQUIRE_FALSE(data_dir.empty());
        REQUIRE(data_dir.is_absolute());
    }

    SECTION("data_directory enthält den app_name") {
#if defined(__linux__)
        auto data_dir = env.data_directory();
        // Unter Linux: data_dir filename ist app_name
        REQUIRE(data_dir.filename() == test_app_name);
#elif defined(_WIN32)
        auto exe_dir = env.executable_directory();
        auto data_dir = env.data_directory();
        // Unter Windows: data_dir ist exe_dir
        REQUIRE(data_dir == exe_dir);
#endif
    }

    SECTION("user_data_directory gibt einen gültigen Pfad zurück") {
        auto user_dir = env.user_data_directory();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(user_dir.filename() == test_app_name);
    }

    SECTION("user_data_directory enthält HOME oder APPDATA im Pfad") {
#if defined(_WIN32)
        auto user_dir = env.user_data_directory();
        std::string user_dir_str = user_dir.string();
        // Unter Windows sollte APPDATA im Pfad enthalten sein
        const char* appdata = std::getenv("APPDATA");
        if (appdata)
            REQUIRE(user_dir_str.find(appdata) != std::string::npos);
#elif defined(__linux__) || defined(__APPLE__)
        auto user_dir = env.user_data_directory();
        std::string user_dir_str = user_dir.string();
        const char* home = std::getenv("HOME");
        if (home)
            REQUIRE(user_dir_str.find(home) != std::string::npos);
#endif
    }

    SECTION("config_directory gibt einen gültigen Pfad zurück") {
        auto config_dir = env.config_directory();
        REQUIRE_FALSE(config_dir.empty());
        REQUIRE(config_dir.is_absolute());
        REQUIRE(config_dir.filename() == test_app_name);
    }

    SECTION("cache_directory gibt einen gültigen Pfad zurück") {
        auto cache_dir = env.cache_directory();
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
        auto log_dir = env.log_directory();
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
            REQUIRE(log_dir_str.find(home) != std::string::npos);
        REQUIRE((log_dir.filename() == test_app_name || log_dir.parent_path().filename() == "Logs"));
#endif
    }

    SECTION("log_directory enthält den app_name") {
        auto log_dir = env.log_directory();
        std::string log_dir_str = log_dir.string();
        REQUIRE(log_dir_str.find(test_app_name) != std::string::npos);
    }

    SECTION("temp_directory gibt einen gültigen Pfad zurück") {
        auto temp_dir = env.temp_directory();
        REQUIRE_FALSE(temp_dir.empty());
        REQUIRE(temp_dir.is_absolute());
        REQUIRE(temp_dir.filename() == test_app_name);
    }

    SECTION("temp_directory liegt im System-Temp-Verzeichnis") {
        auto temp_dir = env.temp_directory();
        auto system_temp = std::filesystem::temp_directory_path();
        
        // Der Pfad sollte im System-Temp-Verzeichnis oder einem Unterverzeichnis liegen
        REQUIRE(temp_dir.string().find(system_temp.string()) != std::string::npos);
    }

    SECTION("user_directory gibt einen gültigen Pfad zurück") {
        auto user_dir = env.user_directory();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(std::filesystem::exists(user_dir));
        REQUIRE(std::filesystem::is_directory(user_dir));
    }

    SECTION("user_directory entspricht HOME oder USERPROFILE") {
        auto user_dir = env.user_directory();
        
#if defined(_WIN32)
        const char* expected_home = std::getenv("USERPROFILE");
#elif defined(__linux__) || defined(__APPLE__)
        const char* expected_home = std::getenv("HOME");
#endif
        
        if (expected_home)
            REQUIRE(user_dir == std::filesystem::path(expected_home));
    }

    SECTION("Mehrere Instanzen mit verschiedenen app_names") {
        pfadfinder::application_environment env1("app1");
        pfadfinder::application_environment env2("app2");
        
        auto dir1 = env1.data_directory();
        auto dir2 = env2.data_directory();
        
        // Jede Instanz sollte ihren eigenen app_name verwenden
        REQUIRE(dir1.filename() == "app1");
        REQUIRE(dir2.filename() == "app2");
    }
}
