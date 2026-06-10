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

// Mock-Backend für Integrationstests
// Dieses Backend leitet alle Pfade in ein temporäres Verzeichnis um,
// damit wir die Verzeichnisfunktionen ohne Admin-Rechte testen können.
namespace test_backend
{
    struct test_system_environment
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
        
        fs::path executable_path() const
        {
            return base_temp_dir / "bin" / "test_app";
        }
        
        fs::path static_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const
        {
            return base_temp_dir / "share" / app_name;
        }
        
        fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const
        {
            return base_temp_dir / "var" / "lib" / app_name;
        }
        
        fs::path user_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const
        {
            return base_temp_dir / "home" / ".local" / "share" / app_name;
        }
        
        fs::path config_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const
        {
            return base_temp_dir / "home" / ".config" / app_name;
        }
        
        fs::path cache_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const
        {
            return base_temp_dir / "home" / ".cache" / app_name;
        }
        
        fs::path log_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const
        {
            return base_temp_dir / "home" / ".local" / "state" / app_name / "log";
        }
        
        fs::path temp_dir(const std::string& app_name) const
        {
            return base_temp_dir / "tmp" / app_name;
        }
        
        fs::path user_dir() const
        {
            return base_temp_dir / "home";
        }
    };
}

// Tests für das pfadfinder-Modul
/**
 * @brief Testfall für alle Pfadfunktionen der application_environment-Klasse.
 * 
 * Dieser Testfall prüft alle öffentlichen Methoden zur Pfadermittlung.
 */
TEST_CASE("pfadfinder::application_environment: Pfadfunktionen", "[integration]") {
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

#if defined(_WIN32)
    SECTION("static_data_dir entspricht Binärverzeichnis unter Windows") {
        // Unter Windows: static_data_dir() = executable_dir()
        auto static_dir = env.static_data_dir();
        auto expected = env.executable_dir();
        REQUIRE(static_dir == expected);
        // Verzeichnis existiert, da executable_dir existiert
        REQUIRE(fs::exists(static_dir));
    }
#endif

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
    
    SECTION("user_data_dir erstellt Verzeichnis und gibt Pfad zurück") {
        auto user_dir = env.user_data_dir(true);
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.is_absolute());
        REQUIRE(user_dir.filename() == test_app_name);
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
        // Aufräumen
        fs::remove_all(user_dir);
    }
    
    SECTION("user_data_dir mit rel_path erstellt Verzeichnis") {
        auto user_dir = env.user_data_dir("subdir", true);
        REQUIRE_FALSE(user_dir.empty());
        REQUIRE(user_dir.filename() == "subdir");
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
        // Aufräumen
        fs::remove_all(user_dir.parent_path());
    }
    
    SECTION("config_dir erstellt Verzeichnis und gibt Pfad zurück") {
        auto config_dir = env.config_dir(true);
        REQUIRE_FALSE(config_dir.empty());
        REQUIRE(config_dir.is_absolute());
        REQUIRE(config_dir.filename() == test_app_name);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
        // Aufräumen
        fs::remove_all(config_dir);
    }
    
    SECTION("cache_dir erstellt Verzeichnis und gibt Pfad zurück") {
        auto cache_dir = env.cache_dir(true);
        REQUIRE_FALSE(cache_dir.empty());
        REQUIRE(cache_dir.is_absolute());
        REQUIRE(fs::exists(cache_dir));
        REQUIRE(fs::is_directory(cache_dir));
        // Aufräumen
        fs::remove_all(cache_dir);
    }
    
    SECTION("log_dir erstellt Verzeichnis und gibt Pfad zurück") {
        auto log_dir = env.log_dir(true);
        REQUIRE_FALSE(log_dir.empty());
        REQUIRE(log_dir.is_absolute());
        REQUIRE(fs::exists(log_dir));
        REQUIRE(fs::is_directory(log_dir));
        // Aufräumen
        fs::remove_all(log_dir);
    }
    
    SECTION("temp_dir erstellt Verzeichnis und gibt Pfad zurück") {
        auto temp_dir = env.temp_dir(true);
        REQUIRE_FALSE(temp_dir.empty());
        REQUIRE(temp_dir.is_absolute());
        REQUIRE(temp_dir.filename() == test_app_name);
        REQUIRE(fs::exists(temp_dir));
        REQUIRE(fs::is_directory(temp_dir));
        // Aufräumen
        fs::remove_all(temp_dir);
    }
    
    SECTION("temp_dir liegt im System-Temp-Verzeichnis") {
        auto temp_dir = env.temp_dir();
        auto system_temp = fs::temp_directory_path();
        REQUIRE(temp_dir.string().find(system_temp.string()) != std::string::npos);
        // Aufräumen
        fs::remove_all(temp_dir);
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
    
    // Tests für create_dir=false - Verzeichnis existiert nicht
    SECTION("user_data_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment env_unique("test_app_unique_12345");
        REQUIRE_THROWS_AS(env_unique.user_data_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("config_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment env_unique("test_app_unique_12345");
        REQUIRE_THROWS_AS(env_unique.config_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("cache_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment env_unique("test_app_unique_12345");
        REQUIRE_THROWS_AS(env_unique.cache_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("log_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment env_unique("test_app_unique_12345");
        REQUIRE_THROWS_AS(env_unique.log_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("temp_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment env_unique("test_app_unique_12345");
        REQUIRE_THROWS_AS(env_unique.temp_dir(false), pfadfinder::directory_not_found);
    }
}

// Tests mit Mock-Backend
/**
 * @brief Unit-Tests für static_data_dir und shared_data_dir mit Mock-Backend.
 * 
 * Diese Tests verwenden ein Mock-Backend, das alle Pfade in ein temporäres Verzeichnis
 * umleitet, damit wir die Funktionen ohne Admin-Rechte testen können.
 */
TEST_CASE("pfadfinder: Mock-Backend Tests", "[unit]") {
    using test_env_type = pfadfinder::application_environment<test_backend::test_system_environment>;
    const std::string test_app_name = "integration_test_app";
    
    // Erstelle ein Mock-Backend und eine Environment-Instanz
    test_backend::test_system_environment backend;
    test_env_type env(test_app_name, backend);
    
    SECTION("static_data_dir gibt gültigen Pfad zurück wenn Verzeichnis existiert") {
        // Verzeichnis erstellen
        auto expected_path = backend.base_temp_dir / "share" / test_app_name;
        fs::create_directories(expected_path);
        
        auto data_dir = env.static_data_dir();
        REQUIRE(data_dir == expected_path);
        REQUIRE(fs::exists(data_dir));
        REQUIRE(fs::is_directory(data_dir));
    }
    
    SECTION("static_data_dir mit rel_path gibt gültigen Pfad zurück") {
        // Verzeichnis erstellen
        auto expected_path = backend.base_temp_dir / "share" / test_app_name / "data";
        fs::create_directories(expected_path);
        
        auto data_dir = env.static_data_dir("data");
        REQUIRE(data_dir == expected_path);
        REQUIRE(fs::exists(data_dir));
        REQUIRE(fs::is_directory(data_dir));
    }
    
    SECTION("static_data_dir wirft wenn Verzeichnis nicht existiert") {
        auto expected_path = backend.base_temp_dir / "share" / test_app_name;
        // Sicherstellen, dass Verzeichnis nicht existiert
        fs::remove_all(expected_path);
        
        REQUIRE_THROWS_AS(env.static_data_dir(), pfadfinder::directory_not_found);
    }
    
    SECTION("shared_data_dir gibt gültigen Pfad zurück wenn Verzeichnis existiert") {
        // Verzeichnis erstellen
        auto expected_path = backend.base_temp_dir / "var" / "lib" / test_app_name;
        fs::create_directories(expected_path);
        
        auto shared_dir = env.shared_data_dir();
        REQUIRE(shared_dir == expected_path);
        REQUIRE(fs::exists(shared_dir));
        REQUIRE(fs::is_directory(shared_dir));
    }
    
    SECTION("shared_data_dir mit rel_path gibt gültigen Pfad zurück") {
        // Verzeichnis erstellen
        auto expected_path = backend.base_temp_dir / "var" / "lib" / test_app_name / "shared";
        fs::create_directories(expected_path);
        
        auto shared_dir = env.shared_data_dir("shared");
        REQUIRE(shared_dir == expected_path);
        REQUIRE(fs::exists(shared_dir));
        REQUIRE(fs::is_directory(shared_dir));
    }
    
    SECTION("shared_data_dir wirft wenn Verzeichnis nicht existiert") {
        auto expected_path = backend.base_temp_dir / "var" / "lib" / test_app_name;
        // Sicherstellen, dass Verzeichnis nicht existiert
        fs::remove_all(expected_path);
        
        REQUIRE_THROWS_AS(env.shared_data_dir(), pfadfinder::directory_not_found);
    }
    
    // Tests für andere Verzeichnismethoden mit Mock-Backend
    SECTION("user_data_dir mit rel_path erstellt Verzeichnis") {
        auto user_dir = env.user_data_dir("subdir", true);
        auto expected = backend.base_temp_dir / "home" / ".local" / "share" / test_app_name / "subdir";
        REQUIRE(user_dir == expected);
        REQUIRE(fs::exists(user_dir));
        REQUIRE(fs::is_directory(user_dir));
    }
    
    SECTION("config_dir mit rel_path erstellt Verzeichnis") {
        auto config_dir = env.config_dir("subdir", true);
        auto expected = backend.base_temp_dir / "home" / ".config" / test_app_name / "subdir";
        REQUIRE(config_dir == expected);
        REQUIRE(fs::exists(config_dir));
        REQUIRE(fs::is_directory(config_dir));
    }
    
    SECTION("cache_dir mit rel_path erstellt Verzeichnis") {
        auto cache_dir = env.cache_dir("subdir", true);
        auto expected = backend.base_temp_dir / "home" / ".cache" / test_app_name / "subdir";
        REQUIRE(cache_dir == expected);
        REQUIRE(fs::exists(cache_dir));
        REQUIRE(fs::is_directory(cache_dir));
    }
    
    SECTION("log_dir mit rel_path erstellt Verzeichnis") {
        auto log_dir = env.log_dir("subdir", true);
        auto expected = backend.base_temp_dir / "home" / ".local" / "state" / test_app_name / "log" / "subdir";
        REQUIRE(log_dir == expected);
        REQUIRE(fs::exists(log_dir));
        REQUIRE(fs::is_directory(log_dir));
    }
    
    SECTION("temp_dir mit rel_path erstellt Verzeichnis") {
        auto temp_dir = env.temp_dir("subdir", true);
        auto expected = backend.base_temp_dir / "tmp" / test_app_name / "subdir";
        REQUIRE(temp_dir == expected);
        REQUIRE(fs::exists(temp_dir));
        REQUIRE(fs::is_directory(temp_dir));
    }
    
    // Tests für create_dir=false mit nicht existierenden Verzeichnissen
    SECTION("user_data_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment<test_backend::test_system_environment> env_no_create("nonexistent_app", backend);
        REQUIRE_THROWS_AS(env_no_create.user_data_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("config_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment<test_backend::test_system_environment> env_no_create("nonexistent_app", backend);
        REQUIRE_THROWS_AS(env_no_create.config_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("cache_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        pfadfinder::application_environment<test_backend::test_system_environment> env_no_create("nonexistent_app", backend);
        REQUIRE_THROWS_AS(env_no_create.cache_dir(false), pfadfinder::directory_not_found);
    }
    
    SECTION("static_data_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        REQUIRE_THROWS_AS(env.static_data_dir(), pfadfinder::directory_not_found);
    }
    
    SECTION("shared_data_dir wirft wenn create_dir=false und Verzeichnis nicht existiert") {
        REQUIRE_THROWS_AS(env.shared_data_dir(), pfadfinder::directory_not_found);
    }
    
    // Caching-Verhalten prüfen (Wert bleibt konsistent)
    SECTION("executable_path gibt konsistente Werte zurück") {
        auto path1 = env.executable_path();
        auto path2 = env.executable_path();
        REQUIRE(path1 == path2);
    }
    
    SECTION("executable_dir gibt konsistente Werte zurück") {
        auto dir1 = env.executable_dir();
        auto dir2 = env.executable_dir();
        REQUIRE(dir1 == dir2);
    }
    
    SECTION("user_dir gibt konsistente Werte zurück") {
        auto dir1 = env.user_dir();
        auto dir2 = env.user_dir();
        REQUIRE(dir1 == dir2);
    }
}
