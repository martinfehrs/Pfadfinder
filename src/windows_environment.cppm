/**
 * @file
 * @brief Plattformspezifische Implementierung für Windows (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <filesystem>
#include <string>

export module pfadfinder:system_backend;

import :system_environment;
import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für Windows
    /**
     * @brief Ausnahme, die geworfen wird, wenn die APPDATA-Umgebungsvariable nicht gesetzt ist.
     */
    export struct appdata_not_set : error
    {
        appdata_not_set()
            : error{ "APPDATA environment variable not set" }
        {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn die LOCALAPPDATA-Umgebungsvariable nicht gesetzt ist.
     */
    export struct localappdata_not_set : error
    {
        localappdata_not_set()
            : error{ "LOCALAPPDATA environment variable not set" }
        {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
     */
    export struct allusersappdata_not_set : error
    {
        allusersappdata_not_set()
            : error{ "ALLUSERSAPPDATA environment variable not set" }
        {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn GetModuleFileNameW() fehlschlägt.
     */
    export struct get_module_file_name_failed : error
    {
        get_module_file_name_failed()
            : error{ "GetModuleFileNameW failed" }
        {}
    };

    /**
     * @brief Enthält plattformspezifische Methoden für Windows zur Pfadermittlung.
     * 
     * Diese Klasse implementiert die system_environment-Schnittstelle
     * spezifisch für das Windows-Betriebssystem.
     */
    export struct default_system_environment : system_environment
    {
        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws get_module_file_name_failed Wenn GetModuleFileNameW fehlschlägt.
         */
        [[nodiscard]] fs::path executable_path() const override
        {
            wchar_t path[MAX_PATH]{};

            if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
                throw get_module_file_name_failed{};

            return fs::path{ path };
        }

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis (gleich dem Binärverzeichnis unter Windows).
         */
        [[nodiscard]] fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            // Windows: Datenverzeichnis ist das Binärverzeichnis
            return exe_dir;
        }

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis (%ALLUSERSAPPDATA%/<appname>).
         * @throws allusersappdata_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            // Windows: %ALLUSERSAPPDATA%/<appname>
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw allusersappdata_not_set{};

            return fs::path{ allusersappdata } / app_name;
        }

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis (%APPDATA%/<appname>).
         * @throws appdata_not_set Wenn die APPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            const char* appdata = std::getenv("APPDATA");

            if (!appdata)
                throw appdata_not_set{};

            return fs::path{ appdata } / app_name;
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Konfigurationsverzeichnis (%APPDATA%/<appname>).
         * @throws appdata_not_set Wenn die APPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path config_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            const char* appdata = std::getenv("APPDATA");

            if (!appdata)
                throw appdata_not_set{};

            return fs::path{ appdata } / app_name;
        }

        /**
         * @brief Gibt das Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Cache-Verzeichnis (%LOCALAPPDATA%/<appname>/Cache).
         * @throws localappdata_not_set Wenn die LOCALAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path cache_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");

            if (!localappdata)
                throw localappdata_not_set{};

            return fs::path{ localappdata } / app_name / "Cache";
        }

        /**
         * @brief Gibt das Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Log-Verzeichnis (%LOCALAPPDATA%/<appname>/Logs).
         * @throws localappdata_not_set Wenn die LOCALAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path log_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");

            if (!localappdata)
                throw localappdata_not_set{};

            return fs::path{ localappdata } / app_name / "Logs";
        }

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis (%TEMP%/<appname>).
         * @throws home_not_set Wenn die TEMP-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path temp_dir(const std::string& app_name) const override
        {
            const char* temp = std::getenv("TEMP");

            if (!temp)
                throw home_not_set{};

            return fs::path{ temp } / app_name;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis des Benutzers (%USERPROFILE%).
         * @throws home_not_set Wenn die USERPROFILE-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_dir() const override
        {
            const char* userprofile = std::getenv("USERPROFILE");

            if (!userprofile)
                throw home_not_set{};

            return fs::path{ userprofile };
        }

        /**
         * @brief Gibt das geteilte Cache-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Cache-Verzeichnis (%ALLUSERSAPPDATA%/<appname>/Cache).
         * @throws allusersappdata_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_cache_dir(const std::string& app_name) const override
        {
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw allusersappdata_not_set{};

            return fs::path{ allusersappdata } / app_name / "Cache";
        }

        /**
         * @brief Gibt das geteilte Log-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Log-Verzeichnis (%ALLUSERSAPPDATA%/<appname>/Logs).
         * @throws allusersappdata_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_log_dir(const std::string& app_name) const override
        {
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw allusersappdata_not_set{};

            return fs::path{ allusersappdata } / app_name / "Logs";
        }

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Konfigurationsverzeichnis (%ALLUSERSAPPDATA%/<appname>).
         * @throws allusersappdata_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_config_dir(const std::string& app_name) const override
        {
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw allusersappdata_not_set{};

            return fs::path{ allusersappdata } / app_name;
        }
    };

}
