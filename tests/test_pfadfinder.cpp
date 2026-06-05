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
    
    SECTION("executable_dir gibt ein gültiges Verzeichnis zurück") {
        auto dir = env.executable_dir();
        REQUIRE_FALSE(dir.empty());
        REQUIRE(dir.is_absolute());
        REQUIRE(fs::exists(dir));
        REQUIRE(fs::is_directory(dir));
    }
    
    SECTION("executable_dir ist das Elternverzeichnis von executable_path") {
        auto exe_path = env.executable_path();
        auto exe_dir = env.executable_dir();
        REQUIRE(exe_dir == exe_path.parent_path());
    }

#if !defined(_WIN32)
    SECTION("static_data_dir wirft directory_not_found wenn nicht existiert") {
        // static_data_dir ist normalerweise schreibgeschützt und existiert nicht
        // Unter Windows gibt static_data_dir() executable_dir() zurück, das existiert
        REQUIRE_THROWS_AS(env.static_data_dir(), pfadfinder::directory_not_found);
    }
#endif

    SECTION("user_data_dir wirft directory_not_found wenn nicht existiert und create_dir=false") {
        REQUIRE_THROWS_AS(env.user_data_dir(false), pfadfinder::directory_not_found);
    }

    SECTION("user_data_dir erstellt Verzeichnis und gibt Pfad zurück wenn create_dir=true") {
        auto user_dir = env.user_data_dir(true);
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(user_dir.filename() == test_app_name);
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
        // Nach dem Erstellen sollte user_data_dir() funktionieren
        auto user_dir2 = env.user_data_dir();
        REQUIRE(user_dir == user_dir2);
        // Aufräumen
        fs::remove_all(user_dir);
    }

    SECTION("user_data_dir enthält HOME oder APPDATA im Pfad") {
        auto user_dir = env.user_data_dir();
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

    SECTION("config_dir wirft directory_not_found wenn nicht existiert und create_dir=false") {
        REQUIRE_THROWS_AS(env.config_dir(false), pfadfinder::directory_not_found);
    }

    SECTION("config_dir erstellt Verzeichnis und gibt Pfad zurück wenn create_dir=true") {
        auto config_dir = env.config_dir(true);
        REQUIRE_FALSE(config_dir.empty());
        REQUIRE(config_dir.is_absolute());
        REQUIRE(config_dir.filename() == test_app_name);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
        // Nach dem Erstellen sollte config_dir() funktionieren
        auto config_dir2 = env.config_dir();
        REQUIRE(config_dir == config_dir2);
        // Aufräumen
        fs::remove_all(config_dir);
    }

    SECTION("cache_dir wirft directory_not_found wenn nicht existiert und create_dir=false") {
        REQUIRE_THROWS_AS(env.cache_dir(false), pfadfinder::directory_not_found);
    }

    SECTION("cache_dir erstellt Verzeichnis und gibt Pfad zurück wenn create_dir=true") {
        auto cache_dir = env.cache_dir(true);
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
        // Nach dem Erstellen sollte cache_dir() funktionieren
        auto cache_dir2 = env.cache_dir();
        REQUIRE(cache_dir == cache_dir2);
        // Aufräumen
        fs::remove_all(cache_dir);
    }

    SECTION("log_dir wirft directory_not_found wenn nicht existiert und create_dir=false") {
        // Verwende einen eindeutigen Namen, der garantiert nicht existiert
        pfadfinder::application_environment env_unique("test_app_log_unique_12345");
        REQUIRE_THROWS_AS(env_unique.log_dir(false), pfadfinder::directory_not_found);
    }

    SECTION("log_dir erstellt Verzeichnis und gibt Pfad zurück wenn create_dir=true") {
        auto log_dir = env.log_dir(true);
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
        // Nach dem Erstellen sollte log_dir() funktionieren
        auto log_dir2 = env.log_dir();
        REQUIRE(log_dir == log_dir2);
        // Aufräumen
        fs::remove_all(log_dir);
    }

    SECTION("log_dir enthält den app_name") {
        auto log_dir = env.log_dir();
        std::string log_dir_str = log_dir.string();
        REQUIRE(log_dir_str.find(test_app_name) != std::string::npos);
        // Aufräumen
        fs::remove_all(log_dir);
    }

    SECTION("temp_dir wirft directory_not_found wenn nicht existiert") {
        // Verwende einen eindeutigen Namen, der garantiert nicht existiert
        pfadfinder::application_environment env_unique("test_app_temp_unique_12345");
        REQUIRE_THROWS_AS(env_unique.temp_dir(), pfadfinder::directory_not_found);
    }

    SECTION("create_temp_dir erstellt Verzeichnis und gibt Pfad zurück") {
        auto temp_dir = env.create_temp_dir();
        REQUIRE_FALSE(temp_dir.empty());
        REQUIRE(temp_dir.is_absolute());
        REQUIRE(temp_dir.filename() == test_app_name);
        REQUIRE(fs::exists(temp_dir));
        REQUIRE(fs::is_directory(temp_dir));
        // Nach dem Erstellen sollte temp_dir() funktionieren
        auto temp_dir2 = env.temp_dir();
        REQUIRE(temp_dir == temp_dir2);
        // Aufräumen
        fs::remove_all(temp_dir);
    }

    SECTION("temp_dir liegt im System-Temp-Verzeichnis") {
        auto temp_dir = env.create_temp_dir();
        auto system_temp = fs::temp_directory_path();
        
        // Der Pfad sollte im System-Temp-Verzeichnis oder einem Unterverzeichnis liegen
        REQUIRE(temp_dir.string().find(system_temp.string()) != std::string::npos);
        // Aufräumen
        fs::remove_all(temp_dir);
    }

    SECTION("user_dir gibt einen gültigen Pfad zurück") {
        auto user_dir = env.user_dir();
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
    }

    SECTION("user_dir entspricht HOME oder USERPROFILE") {
        auto user_dir = env.user_dir();
        
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
        
        auto dir1 = env1.user_data_dir();
        auto dir2 = env2.user_data_dir();
        
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
    // static_data_dir() wirft nicht unter Windows, da es executable_dir() entspricht
#if !defined(_WIN32)
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_data_unique_12345").static_data_dir(), 
                      pfadfinder::directory_not_found);
#endif
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_user_unique_12345").user_data_dir(false), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_config_unique_12345").config_dir(false), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_cache_unique_12345").cache_dir(false), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_log_unique_12345").log_dir(false), 
                      pfadfinder::directory_not_found);
    REQUIRE_THROWS_AS(pfadfinder::application_environment("test_app_temp_unique_12345").temp_dir(), 
                      pfadfinder::directory_not_found);
}

