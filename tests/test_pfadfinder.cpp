/**
 * @file test_pfadfinder.cpp
 * @brief Unit-Tests für die application_environment-Klasse mit Mock-Backend
 * @author Martin Fehrs
 */

// Standardbibliotheks-Header
#include <filesystem>
#include <string>
#include <fstream>

// CATCH2 Header (single-include version)
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Import des zu testenden Moduls
import pfadfinder;

namespace fs = std::filesystem;

// Mock-Backend für Unit-Tests
// Dieses Backend leitet alle Pfade in ein temporäres Verzeichnis um,
// damit wir die Verzeichnisfunktionen ohne Admin-Rechte testen können.

namespace test_backend
{
    struct test_system_environment : pfadfinder::system_environment
    {
        fs::path base_temp_dir;
        
        test_system_environment() : base_temp_dir(fs::temp_directory_path() / "pfadfinder_test")
        {
            // Basisverzeichnis erstellen
            fs::create_directories(base_temp_dir);
        }
        
        ~test_system_environment()
        {
            // Aufräumen
            fs::remove_all(base_temp_dir);
        }
        
        [[nodiscard]] fs::path executable_path() const override
        {
            return base_temp_dir / "bin" / "test_app";
        }
        
        [[nodiscard]] fs::path static_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            return base_temp_dir / "share" / app_name;
        }
        
        [[nodiscard]] fs::path data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            return base_temp_dir / "home" / ".local" / "share" / app_name;
        }
        
        [[nodiscard]] fs::path user_config_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            return base_temp_dir / "home" / ".config" / app_name;
        }
        
        [[nodiscard]] fs::path cache_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            return base_temp_dir / "home" / ".cache" / app_name;
        }
        
        [[nodiscard]] fs::path log_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            return base_temp_dir / "home" / ".local" / "state" / app_name / "log";
        }
        
        [[nodiscard]] fs::path temp_dir(const std::string& app_name) const override
        {
            return base_temp_dir / "tmp" / app_name;
        }
        
        [[nodiscard]] fs::path home_dir() const override
        {
            return base_temp_dir / "home";
        }
        
        [[nodiscard]] fs::path shared_config_dir(const std::string& app_name) const override
        {
            return base_temp_dir / "var" / "lib" / app_name;
        }
    };
}

// Unit-Tests für die application_environment-Klasse mit Mock-Backend
TEST_CASE("pfadfinder::application_environment: Unit-Tests mit Mock-Backend")
{
    using test_env_type = pfadfinder::application_environment<test_backend::test_system_environment>;
    const std::string test_app_name = "test_app";
    
    // Erstelle ein Mock-Backend und eine Environment-Instanz
    test_backend::test_system_environment backend;
    test_env_type env(test_app_name, backend);

    // Test executable_path
    SECTION("executable_path gibt gültigen Pfad zurück")
    {
        auto path = env.executable_path();
        REQUIRE_FALSE(path.empty());
        REQUIRE(path.is_absolute());
        REQUIRE(path == backend.base_temp_dir / "bin" / "test_app");
    }

    // Test executable_dir
    SECTION("executable_dir gibt gültiges Verzeichnis zurück")
    {
        auto dir = env.executable_dir();
        REQUIRE_FALSE(dir.empty());
        REQUIRE(dir.is_absolute());
        REQUIRE(dir == backend.base_temp_dir / "bin");
    }

    SECTION("executable_dir ist das Elternverzeichnis von executable_path")
    {
        auto exe_path = env.executable_path();
        auto exe_dir = env.executable_dir();
        REQUIRE(exe_dir == exe_path.parent_path());
    }

    // Test home_dir
    SECTION("home_dir gibt gültigen Pfad zurück")
    {
        auto home_dir = env.home_dir();
        REQUIRE_FALSE(home_dir.empty());
        REQUIRE(home_dir.is_absolute());
        REQUIRE(home_dir == backend.base_temp_dir / "home");
    }

    // Test static_data_dir mit existierendem Verzeichnis
    SECTION("static_data_dir gibt gültigen Pfad zurück wenn Verzeichnis existiert")
    {
        // Verzeichnis erstellen
        auto expected_path = backend.base_temp_dir / "share" / test_app_name;
        fs::create_directories(expected_path);
        
        auto data_dir = env.static_data_dir();
        REQUIRE(data_dir == expected_path);
        REQUIRE(fs::exists(data_dir));
        REQUIRE(fs::is_directory(data_dir));
    }

    SECTION("static_data_dir mit rel_path gibt gültigen Pfad zurück")
    {
        // Verzeichnis erstellen
        auto expected_path = backend.base_temp_dir / "share" / test_app_name / "data";
        fs::create_directories(expected_path);
        
        auto data_dir = env.static_data_dir("data");
        REQUIRE(data_dir == expected_path);
        REQUIRE(fs::exists(data_dir));
        REQUIRE(fs::is_directory(data_dir));
    }

    SECTION("static_data_dir wirft wenn Verzeichnis nicht existiert")
    {
        auto expected_path = backend.base_temp_dir / "share" / test_app_name;
        // Sicherstellen, dass Verzeichnis nicht existiert
        fs::remove_all(expected_path);
        
        REQUIRE_THROWS_AS(env.static_data_dir(), pfadfinder::directory_not_found);
    }

    // Test data_dir
    SECTION("data_dir wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_data_app", backend);
        REQUIRE_THROWS_AS(env_unique.data_dir(), pfadfinder::directory_not_found);
    }

    SECTION("data_dir mit rel_path wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_data_app", backend);
        REQUIRE_THROWS_AS(env_unique.data_dir("subdir"), pfadfinder::directory_not_found);
    }

    // Test create_data_dir
    SECTION("create_data_dir erstellt Verzeichnis und gibt Pfad zurück")
    {
        auto home_dir = env.create_data_dir();
        auto expected = backend.base_temp_dir / "home" / ".local" / "share" / test_app_name;
        REQUIRE(home_dir == expected);
        REQUIRE(fs::exists(home_dir));
        REQUIRE(fs::is_directory(home_dir));
    }

    SECTION("create_data_dir mit rel_path erstellt Verzeichnis")
    {
        auto home_dir = env.create_data_dir("subdir");
        auto expected = backend.base_temp_dir / "home" / ".local" / "share" / test_app_name / "subdir";
        REQUIRE(home_dir == expected);
        REQUIRE(fs::exists(home_dir));
        REQUIRE(fs::is_directory(home_dir));
    }

    // Test user_config_dir
    SECTION("create_user_config_dir erstellt Verzeichnis und gibt Pfad zurück")
    {
        auto config_dir = env.create_user_config_dir();
        auto expected = backend.base_temp_dir / "home" / ".config" / test_app_name;
        REQUIRE(config_dir == expected);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }

    SECTION("create_user_config_dir mit rel_path erstellt Verzeichnis")
    {
        auto config_dir = env.create_user_config_dir("subdir");
        auto expected = backend.base_temp_dir / "home" / ".config" / test_app_name / "subdir";
        REQUIRE(config_dir == expected);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }

    SECTION("user_config_dir wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_config_app", backend);
        REQUIRE_THROWS_AS(env_unique.user_config_dir(), pfadfinder::directory_not_found);
    }

    SECTION("user_config_file gibt gültigen Pfad zurück wenn Datei existiert")
    {
        auto config_dir = env.create_user_config_dir();
        auto config_file = config_dir / "test_config.json";
        
        // Datei erstellen
        std::ofstream test_file(config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.user_config_file("test_config.json");
        REQUIRE(result == config_file);
        REQUIRE(fs::exists(result));
        REQUIRE(fs::is_regular_file(result));
    }

    SECTION("user_config_file wirft wenn Datei nicht existiert")
    {
        REQUIRE_THROWS_AS(env.user_config_file("nonexistent_config.json"), pfadfinder::file_not_found);
    }

    SECTION("find_user_config_file gibt gültigen Pfad zurück wenn Datei existiert")
    {
        auto config_dir = env.create_user_config_dir();
        auto config_file = config_dir / "test_config.json";
        
        // Datei erstellen
        std::ofstream test_file(config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.find_user_config_file("test_config.json");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == config_file);
        REQUIRE(fs::exists(result.value()));
        REQUIRE(fs::is_regular_file(result.value()));
    }

    SECTION("find_user_config_file gibt std::nullopt zurück wenn Datei nicht existiert")
    {
        auto result = env.find_user_config_file("nonexistent_config.json");
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    SECTION("find_user_config_dir gibt gültigen Pfad zurück wenn Verzeichnis existiert")
    {
        auto config_dir = env.create_user_config_dir();
        auto result = env.find_user_config_dir();
        REQUIRE(result.has_value());
        REQUIRE(result.value() == config_dir);
        REQUIRE(fs::exists(result.value()));
        REQUIRE(fs::is_directory(result.value()));
    }

    SECTION("find_user_config_dir gibt std::nullopt zurück wenn Verzeichnis nicht existiert")
    {
        test_env_type env_no_create("nonexistent_config_dir_app", backend);
        auto result = env_no_create.find_user_config_dir();
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    SECTION("find_user_config_dir mit rel_path gibt gültigen Pfad zurück wenn Verzeichnis existiert")
    {
        auto config_dir = env.create_user_config_dir();
        auto subdir = config_dir / "subdir";
        fs::create_directories(subdir);
        
        auto result = env.find_user_config_dir("subdir");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == subdir);
        REQUIRE(fs::exists(result.value()));
        REQUIRE(fs::is_directory(result.value()));
    }

    SECTION("find_user_config_dir mit rel_path gibt std::nullopt zurück wenn Verzeichnis nicht existiert")
    {
        auto result = env.find_user_config_dir("nonexistent_subdir");
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    SECTION("shared_config_file gibt gültigen Pfad zurück wenn Datei existiert")
    {
        auto config_dir = backend.base_temp_dir / "var" / "lib" / test_app_name;
        auto config_file = config_dir / "shared_config.json";
        
        // Verzeichnis und Datei erstellen
        fs::create_directories(config_dir);
        std::ofstream test_file(config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.shared_config_file("shared_config.json");
        REQUIRE(result == config_file);
        REQUIRE(fs::exists(result));
        REQUIRE(fs::is_regular_file(result));
    }

    SECTION("shared_config_file wirft wenn Datei nicht existiert")
    {
        REQUIRE_THROWS_AS(env.shared_config_file("nonexistent_shared_config.json"), pfadfinder::file_not_found);
    }

    SECTION("find_shared_config_file gibt gültigen Pfad zurück wenn Datei existiert")
    {
        auto config_dir = backend.base_temp_dir / "var" / "lib" / test_app_name;
        auto config_file = config_dir / "shared_config.json";
        
        // Verzeichnis und Datei erstellen
        fs::create_directories(config_dir);
        std::ofstream test_file(config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.find_shared_config_file("shared_config.json");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == config_file);
        REQUIRE(fs::exists(result.value()));
        REQUIRE(fs::is_regular_file(result.value()));
    }

    SECTION("find_shared_config_file gibt std::nullopt zurück wenn Datei nicht existiert")
    {
        auto result = env.find_shared_config_file("nonexistent_shared_config.json");
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    SECTION("find_config_file gibt Benutzer-Konfigurationsdatei zurück wenn diese existiert")
    {
        auto config_dir = env.create_user_config_dir();
        auto config_file = config_dir / "test_config.json";
        
        // Datei im Benutzer-Konfigurationsverzeichnis erstellen
        std::ofstream test_file(config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.find_config_file("test_config.json");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == config_file);
    }

    SECTION("find_config_file gibt geteilte Konfigurationsdatei zurück wenn Benutzer-Datei nicht existiert")
    {
        auto user_config_dir = env.create_user_config_dir();
        auto shared_config_dir = backend.base_temp_dir / "var" / "lib" / test_app_name;
        auto shared_config_file = shared_config_dir / "test_config.json";
        
        // Datei nur im geteilten Konfigurationsverzeichnis erstellen
        fs::create_directories(shared_config_dir);
        std::ofstream test_file(shared_config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.find_config_file("test_config.json");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == shared_config_file);
    }

    SECTION("find_config_file gibt std::nullopt zurück wenn keine Datei existiert")
    {
        auto result = env.find_config_file("nonexistent_config.json");
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    SECTION("find_config_file gibt Benutzer-Datei zurück wenn beide existieren")
    {
        auto user_config_dir = env.create_user_config_dir();
        auto shared_config_dir = backend.base_temp_dir / "var" / "lib" / test_app_name;
        
        auto user_config_file = user_config_dir / "test_config.json";
        auto shared_config_file = shared_config_dir / "test_config.json";
        
        // Dateien in beiden Verzeichnissen erstellen
        fs::create_directories(shared_config_dir);
        std::ofstream user_file(user_config_file);
        user_file << "user_config" << std::endl;
        user_file.close();
        std::ofstream shared_file(shared_config_file);
        shared_file << "shared_config" << std::endl;
        shared_file.close();
        
        auto result = env.find_config_file("test_config.json");
        REQUIRE(result.has_value());
        // Sollte die Benutzer-Konfigurationsdatei zurückgeben
        REQUIRE(result.value() == user_config_file);
    }

    SECTION("config_files gibt leeren Vektor zurück wenn keine Dateien existieren")
    {
        auto result = env.config_files("nonexistent_config.json");
        REQUIRE(result.empty());
        REQUIRE(result.size() == 0);
    }

    SECTION("config_files gibt Benutzer-Konfigurationsdatei zurück wenn nur diese existiert")
    {
        auto config_dir = env.create_user_config_dir();
        auto config_file = config_dir / "test_config.json";
        
        // Datei im Benutzer-Konfigurationsverzeichnis erstellen
        std::ofstream test_file(config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.config_files("test_config.json");
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == config_file);
    }

    SECTION("config_files gibt geteilte Konfigurationsdatei zurück wenn nur diese existiert")
    {
        auto shared_config_dir = backend.base_temp_dir / "var" / "lib" / test_app_name;
        auto shared_config_file = shared_config_dir / "test_config.json";
        
        // Datei nur im geteilten Konfigurationsverzeichnis erstellen
        fs::create_directories(shared_config_dir);
        std::ofstream test_file(shared_config_file);
        test_file << "{}" << std::endl;
        test_file.close();
        
        auto result = env.config_files("test_config.json");
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == shared_config_file);
    }

    SECTION("config_files gibt beide Konfigurationsdateien zurück wenn beide existieren")
    {
        auto user_config_dir = env.create_user_config_dir();
        auto shared_config_dir = backend.base_temp_dir / "var" / "lib" / test_app_name;
        
        auto user_config_file = user_config_dir / "test_config.json";
        auto shared_config_file = shared_config_dir / "test_config.json";
        
        // Dateien in beiden Verzeichnissen erstellen
        fs::create_directories(shared_config_dir);
        std::ofstream user_file(user_config_file);
        user_file << "user_config" << std::endl;
        user_file.close();
        std::ofstream shared_file(shared_config_file);
        shared_file << "shared_config" << std::endl;
        shared_file.close();
        
        auto result = env.config_files("test_config.json");
        REQUIRE(result.size() == 2);
        // Benutzer-Konfigurationsdatei sollte zuerst sein
        REQUIRE(result[0] == user_config_file);
        // Geteilte Konfigurationsdatei sollte zweitens sein
        REQUIRE(result[1] == shared_config_file);
    }

    // Test cache_dir
    SECTION("cache_dir wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_cache_app", backend);
        REQUIRE_THROWS_AS(env_unique.cache_dir(), pfadfinder::directory_not_found);
    }

    SECTION("cache_dir mit rel_path wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_cache_app", backend);
        REQUIRE_THROWS_AS(env_unique.cache_dir("subdir"), pfadfinder::directory_not_found);
    }

    // Test create_cache_dir
    SECTION("create_cache_dir erstellt Verzeichnis und gibt Pfad zurück")
    {
        auto cache_dir = env.create_cache_dir();
        auto expected = backend.base_temp_dir / "home" / ".cache" / test_app_name;
        REQUIRE(cache_dir == expected);
        REQUIRE(fs::exists(cache_dir));
        REQUIRE(fs::is_directory(cache_dir));
    }

    SECTION("create_cache_dir mit rel_path erstellt Verzeichnis")
    {
        auto cache_dir = env.create_cache_dir("subdir");
        auto expected = backend.base_temp_dir / "home" / ".cache" / test_app_name / "subdir";
        REQUIRE(cache_dir == expected);
        REQUIRE(fs::exists(cache_dir));
        REQUIRE(fs::is_directory(cache_dir));
    }

    SECTION("find_cache_dir gibt gültigen Pfad zurück wenn Verzeichnis existiert")
    {
        auto cache_dir = env.create_cache_dir();
        auto result = env.find_cache_dir();
        REQUIRE(result.has_value());
        REQUIRE(result.value() == cache_dir);
        REQUIRE(fs::exists(result.value()));
        REQUIRE(fs::is_directory(result.value()));
    }

    SECTION("find_cache_dir gibt std::nullopt zurück wenn Verzeichnis nicht existiert")
    {
        test_env_type env_no_create("nonexistent_find_cache_app", backend);
        auto result = env_no_create.find_cache_dir();
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    SECTION("find_cache_dir mit rel_path gibt gültigen Pfad zurück wenn Verzeichnis existiert")
    {
        auto cache_dir = env.create_cache_dir();
        auto subdir = cache_dir / "subdir";
        fs::create_directories(subdir);
        
        auto result = env.find_cache_dir("subdir");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == subdir);
        REQUIRE(fs::exists(result.value()));
        REQUIRE(fs::is_directory(result.value()));
    }

    SECTION("find_cache_dir mit rel_path gibt std::nullopt zurück wenn Verzeichnis nicht existiert")
    {
        auto result = env.find_cache_dir("nonexistent_subdir");
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result == std::nullopt);
    }

    // Test log_dir
    SECTION("log_dir wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_log_app", backend);
        REQUIRE_THROWS_AS(env_unique.log_dir(), pfadfinder::directory_not_found);
    }

    SECTION("log_dir mit rel_path wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_log_app", backend);
        REQUIRE_THROWS_AS(env_unique.log_dir("subdir"), pfadfinder::directory_not_found);
    }

    // Test create_log_dir
    SECTION("create_log_dir erstellt Verzeichnis und gibt Pfad zurück")
    {
        auto log_dir = env.create_log_dir();
        auto expected = backend.base_temp_dir / "home" / ".local" / "state" / test_app_name / "log";
        REQUIRE(log_dir == expected);
        REQUIRE(fs::exists(log_dir));
        REQUIRE(fs::is_directory(log_dir));
    }

    SECTION("create_log_dir mit rel_path erstellt Verzeichnis")
    {
        auto log_dir = env.create_log_dir("subdir");
        auto expected = backend.base_temp_dir / "home" / ".local" / "state" / test_app_name / "log" / "subdir";
        REQUIRE(log_dir == expected);
        REQUIRE(fs::exists(log_dir));
        REQUIRE(fs::is_directory(log_dir));
    }

    // Test temp_dir
    SECTION("temp_dir erstellt Verzeichnis und gibt Pfad zurück")
    {
        auto temp_dir = env.temp_dir(true);
        auto expected = backend.base_temp_dir / "tmp" / test_app_name;
        REQUIRE(temp_dir == expected);
        REQUIRE(fs::exists(temp_dir));
        REQUIRE(fs::is_directory(temp_dir));
    }

    SECTION("temp_dir mit rel_path erstellt Verzeichnis")
    {
        auto temp_dir = env.temp_dir("subdir", true);
        auto expected = backend.base_temp_dir / "tmp" / test_app_name / "subdir";
        REQUIRE(temp_dir == expected);
        REQUIRE(fs::exists(temp_dir));
        REQUIRE(fs::is_directory(temp_dir));
    }

    SECTION("temp_dir ohne create_dir wirft wenn Verzeichnis nicht existiert")
    {
        test_env_type env_unique("nonexistent_temp_app", backend);
        REQUIRE_THROWS_AS(env_unique.temp_dir(false), pfadfinder::directory_not_found);
    }

    // Test shared_config_dir
    SECTION("shared_config_dir wirft immer wenn Verzeichnis nicht existiert")
    {
        test_env_type env_no_create("nonexistent_shared_config", backend);
        REQUIRE_THROWS_AS(env_no_create.shared_config_dir(), pfadfinder::directory_not_found);
    }

    SECTION("shared_config_dir gibt gültigen Pfad zurück")
    {
        auto expected_path = backend.base_temp_dir / "var" / "lib" / test_app_name;
        fs::create_directories(expected_path);
        
        auto config_dir = env.shared_config_dir();
        REQUIRE(config_dir == expected_path);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }

    SECTION("shared_config_dir mit rel_path gibt gültigen Pfad zurück")
    {
        auto expected_path = backend.base_temp_dir / "var" / "lib" / test_app_name / "config";
        fs::create_directories(expected_path);
        
        auto config_dir = env.shared_config_dir("config");
        REQUIRE(config_dir == expected_path);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }

    // Test create_shared_config_dir
    SECTION("create_shared_config_dir erstellt Verzeichnis und gibt Pfad zurück")
    {
        auto config_dir = env.create_shared_config_dir();
        auto expected = backend.base_temp_dir / "var" / "lib" / test_app_name;
        REQUIRE(config_dir == expected);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }

    SECTION("create_shared_config_dir mit rel_path erstellt Verzeichnis")
    {
        auto config_dir = env.create_shared_config_dir("subdir");
        auto expected = backend.base_temp_dir / "var" / "lib" / test_app_name / "subdir";
        REQUIRE(config_dir == expected);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }

    // Test Caching-Verhalten
    SECTION("executable_path gibt konsistente Werte zurück")
    {
        auto path1 = env.executable_path();
        auto path2 = env.executable_path();
        REQUIRE(path1 == path2);
    }

    SECTION("executable_dir gibt konsistente Werte zurück")
    {
        auto dir1 = env.executable_dir();
        auto dir2 = env.executable_dir();
        REQUIRE(dir1 == dir2);
    }

    SECTION("home_dir gibt konsistente Werte zurück")
    {
        auto dir1 = env.home_dir();
        auto dir2 = env.home_dir();
        REQUIRE(dir1 == dir2);
    }

    SECTION("data_dir gibt konsistente Werte zurück")
    {
        auto expected_path = backend.base_temp_dir / "home" / ".local" / "share" / test_app_name;
        fs::create_directories(expected_path);
        auto dir1 = env.data_dir();
        auto dir2 = env.data_dir();
        REQUIRE(dir1 == dir2);
        REQUIRE(dir1 == expected_path);
    }

    SECTION("user_config_dir gibt konsistente Werte zurück")
    {
        auto expected_path = backend.base_temp_dir / "home" / ".config" / test_app_name;
        fs::create_directories(expected_path);
        auto dir1 = env.user_config_dir();
        auto dir2 = env.user_config_dir();
        REQUIRE(dir1 == dir2);
        REQUIRE(dir1 == expected_path);
    }

    SECTION("cache_dir gibt konsistente Werte zurück")
    {
        auto expected_path = backend.base_temp_dir / "home" / ".cache" / test_app_name;
        fs::create_directories(expected_path);
        auto dir1 = env.cache_dir();
        auto dir2 = env.cache_dir();
        REQUIRE(dir1 == dir2);
        REQUIRE(dir1 == expected_path);
    }

    SECTION("log_dir gibt konsistente Werte zurück")
    {
        auto expected_path = backend.base_temp_dir / "home" / ".local" / "state" / test_app_name / "log";
        fs::create_directories(expected_path);
        auto dir1 = env.log_dir();
        auto dir2 = env.log_dir();
        REQUIRE(dir1 == dir2);
        REQUIRE(dir1 == expected_path);
    }

    SECTION("temp_dir gibt konsistente Werte zurück")
    {
        auto dir1 = env.temp_dir();
        auto dir2 = env.temp_dir();
        REQUIRE(dir1 == dir2);
    }

    SECTION("shared_config_dir gibt konsistente Werte zurück")
    {
        auto expected_path = backend.base_temp_dir / "var" / "lib" / test_app_name;
        fs::create_directories(expected_path);
        auto dir1 = env.shared_config_dir();
        auto dir2 = env.shared_config_dir();
        REQUIRE(dir1 == dir2);
    }

    // Test mit verschiedenen app_names
    SECTION("Mehrere Instanzen mit verschiedenen app_names")
    {
        test_env_type env1("app1", backend);
        test_env_type env2("app2", backend);
        
        fs::create_directories(backend.base_temp_dir / "home" / ".local" / "share" / "app1");
        fs::create_directories(backend.base_temp_dir / "home" / ".local" / "share" / "app2");
        
        auto dir1 = env1.data_dir();
        auto dir2 = env2.data_dir();
        
        // Die Verzeichnisse sollten unterschiedlich sein
        REQUIRE(dir1 != dir2);
        REQUIRE(dir1.filename() == "app1");
        REQUIRE(dir2.filename() == "app2");
    }
}
