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

namespace fs = std::filesystem;

// Tests für das pfadfinder-Modul
TEST_CASE("pfadfinder::application_environment: Pfadfunktionen", "[pfadfinder]") {
    // Erstelle eine application_environment Instanz mit Test-App-Name
    const std::string test_app_name = "test_app";
    pfadfinder::application_environment env(test_app_name);
    
    SECTION("executable_path gibt einen gültigen Pfad zurück") {
        auto path = env.executable_path();
        REQUIRE_FALSE(path.empty());
        REQUIRE(path.is_absolute());
        REQUIRE(fs::exists(path));
    }
    
    SECTION("executable_directory gibt ein gültiges Verzeichnis zurück") {
        auto dir = env.executable_directory();
        REQUIRE_FALSE(dir.empty());
        REQUIRE(dir.is_absolute());
        REQUIRE(fs::exists(dir));
        REQUIRE(fs::is_directory(dir));
    }
    
    SECTION("executable_directory ist das Elternverzeichnis von executable_path") {
        auto exe_path = env.executable_path();
        auto exe_dir = env.executable_directory();
        REQUIRE(exe_dir == exe_path.parent_path());
    }

    SECTION("data_directory wirft directory_not_found wenn nicht existiert") {
        // data_directory ist normalerweise schreibgeschützt und existiert nicht
        REQUIRE_THROWS_AS(env.data_directory(), pfadfinder::directory_not_found);
    }

    SECTION("user_data_directory wirft directory_not_found wenn nicht existiert") {
        REQUIRE_THROWS_AS(env.user_data_directory(), pfadfinder::directory_not_found);
    }

    SECTION("create_user_data_directory erstellt Verzeichnis und gibt Pfad zurück") {
        auto user_dir = env.create_user_data_directory();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(user_dir.filename() == test_app_name);
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
        // Nach dem Erstellen sollte user_data_directory() funktionieren
        auto user_dir2 = env.user_data_directory();
        REQUIRE(user_dir == user_dir2);
        // Aufräumen
        fs::remove_all(user_dir);
    }

    SECTION("user_data_directory enthält HOME oder APPDATA im Pfad") {
        auto user_dir = env.create_user_data_directory();
        std::string user_dir_str = user_dir.string();
        
        // Aufräumen nach dem Test
        fs::remove_all(user_dir);
        
#if defined(_WIN32)
        const char* appdata = std::getenv("APPDATA");
        if (appdata)
            REQUIRE(user_dir_str.find(appdata) != std::string::npos);
#elif defined(__linux__) || defined(__APPLE__)
        const char* home = std::getenv("HOME");
        if (home)
            REQUIRE(user_dir_str.find(home) != std::string::npos);
#endif
    }

    SECTION("config_directory wirft directory_not_found wenn nicht existiert") {
        REQUIRE_THROWS_AS(env.config_directory(), pfadfinder::directory_not_found);
    }

    SECTION("create_config_directory erstellt Verzeichnis und gibt Pfad zurück") {
        auto config_dir = env.create_config_directory();
        REQUIRE_FALSE(config_dir.empty());
        REQUIRE(config_dir.is_absolute());
        REQUIRE(config_dir.filename() == test_app_name);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
        // Nach dem Erstellen sollte config_directory() funktionieren
        auto config_dir2 = env.config_directory();
        REQUIRE(config_dir == config_dir2);
        // Aufräumen
        fs::remove_all(config_dir);
    }

    SECTION("cache_directory wirft directory_not_found wenn nicht existiert") {
        REQUIRE_THROWS_AS(env.cache_directory(), pfadfinder::directory_not_found);
    }

    SECTION("create_cache_directory erstellt Verzeichnis und gibt Pfad zurück") {
        auto cache_dir = env.create_cache_directory();
        REQUIRE_FALSE(cache_dir.empty());
        REQUIRE(cache_dir.is_absolute());
        
#if defined(__linux__) || (defined(__APPLE__) && !defined(__BUNDLE__))
        // Unter Linux und macOS CLI: ~/.cache/<name>
        REQUIRE(cache_dir.filename() == test_app_name);
#elif defined(_WIN32)
        // Unter Windows: %LOCALAPPDATA%/<name>/Cache
        REQUIRE(cache_dir.parent_path().filename() == test_app_name);
        REQUIRE(cache_dir.filename() == "Cache");
#endif
        REQUIRE(fs::exists(cache_dir));
        REQUIRE(fs::is_directory(cache_dir));
        // Nach dem Erstellen sollte cache_directory() funktionieren
        auto cache_dir2 = env.cache_directory();
        REQUIRE(cache_dir == cache_dir2);
        // Aufräumen
        fs::remove_all(cache_dir);
    }

    SECTION("log_directory wirft directory_not_found wenn nicht existiert") {
        // Verwende einen eindeutigen Namen, der garantiert nicht existiert
        pfadfinder::application_environment env_unique("test_app_log_unique_12345");
        REQUIRE_THROWS_AS(env_unique.log_directory(), pfadfinder::directory_not_found);
    }

    SECTION("create_log_directory erstellt Verzeichnis und gibt Pfad zurück") {
        auto log_dir = env.create_log_directory();
        REQUIRE_FALSE(log_dir.empty());
        REQUIRE(log_dir.is_absolute());
        
        std::string log_dir_str = log_dir.string();
        REQUIRE(log_dir_str.find(test_app_name) != std::string::npos);
        
#if defined(_WIN32)
        // Unter Windows: %LOCALAPPDATA%/<name>/Logs
        REQUIRE(log_dir.parent_path().filename() == test_app_name);
        REQUIRE(log_dir.filename() == "Logs");
#elif defined(__linux__)
        // Unter Linux: ~/.local/state/<name>/log
        REQUIRE(log_dir.filename() == "log");
        REQUIRE(log_dir.parent_path().filename() == test_app_name);
#endif
        REQUIRE(fs::exists(log_dir));
        REQUIRE(fs::is_directory(log_dir));
        // Nach dem Erstellen sollte log_directory() funktionieren
        auto log_dir2 = env.log_directory();
        REQUIRE(log_dir == log_dir2);
        // Aufräumen
        fs::remove_all(log_dir);
    }

    SECTION("log_directory enthält den app_name") {
        auto log_dir = env.create_log_directory();
        std::string log_dir_str = log_dir.string();
        REQUIRE(log_dir_str.find(test_app_name) != std::string::npos);
        // Aufräumen
        fs::remove_all(log_dir);
    }

    SECTION("temp_directory wirft directory_not_found wenn nicht existiert") {
        // Verwende einen eindeutigen Namen, der garantiert nicht existiert
        pfadfinder::application_environment env_unique("test_app_temp_unique_12345");
        REQUIRE_THROWS_AS(env_unique.temp_directory(), pfadfinder::directory_not_found);
    }

    SECTION("create_temp_directory erstellt Verzeichnis und gibt Pfad zurück") {
        auto temp_dir = env.create_temp_directory();
        REQUIRE_FALSE(temp_dir.empty());
        REQUIRE(temp_dir.is_absolute());
        REQUIRE(temp_dir.filename() == test_app_name);
        REQUIRE(fs::exists(temp_dir));
        REQUIRE(fs::is_directory(temp_dir));
        // Nach dem Erstellen sollte temp_directory() funktionieren
        auto temp_dir2 = env.temp_directory();
        REQUIRE(temp_dir == temp_dir2);
        // Aufräumen
        fs::remove_all(temp_dir);
    }

    SECTION("temp_directory liegt im System-Temp-Verzeichnis") {
        auto temp_dir = env.create_temp_directory();
        auto system_temp = fs::temp_directory_path();
        
        // Der Pfad sollte im System-Temp-Verzeichnis oder einem Unterverzeichnis liegen
        REQUIRE(temp_dir.string().find(system_temp.string()) != std::string::npos);
        // Aufräumen
        fs::remove_all(temp_dir);
    }

    SECTION("user_directory gibt einen gültigen Pfad zurück") {
        auto user_dir = env.user_directory();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
    }

    SECTION("user_directory entspricht HOME oder USERPROFILE") {
        auto user_dir = env.user_directory();
        
#if defined(_WIN32)
        const char* expected_home = std::getenv("USERPROFILE");
#elif defined(__linux__) || defined(__APPLE__)
        const char* expected_home = std::getenv("HOME");
#endif
        
        if (expected_home)
            REQUIRE(user_dir == fs::path(expected_home));
    }

    SECTION("Mehrere Instanzen mit verschiedenen app_names") {
        pfadfinder::application_environment env1("app1");
        pfadfinder::application_environment env2("app2");
        
        auto dir1 = env1.create_user_data_directory();
        auto dir2 = env2.create_user_data_directory();
        
        // Die Verzeichnisse sollten unterschiedlich sein
        REQUIRE(dir1 != dir2);
        REQUIRE(dir1.filename() == "app1");
        REQUIRE(dir2.filename() == "app2");
        
        // Aufräumen
        fs::remove_all(dir1);
        fs::remove_all(dir2);
    }
}

// Tests für Ausnahmen
TEST_CASE("pfadfinder: Ausnahmen", "[pfadfinder][exceptions]") {
    // Verwende eindeutige Namen, die garantiert nicht existieren
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_data_unique_12345").data_directory(), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_user_unique_12345").user_data_directory(), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_config_unique_12345").config_directory(), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_cache_unique_12345").cache_directory(), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_log_unique_12345").log_directory(), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_temp_unique_12345").temp_directory(), 
                      pfadfinder::directory_not_found);
}

