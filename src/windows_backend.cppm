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

export module pfadfinder:windows_backend;

import :system_environment;
import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{

    /**
     * @brief Enthält plattformspezifische Methoden für Windows zur Pfadermittlung.
     * 
     * Diese Klasse implementiert die system_environment-Schnittstelle
     * spezifisch für das Windows-Betriebssystem.
     */
    export struct windows_system_environment : system_environment
    {
        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws indeterminable_exe_path Wenn der Pfad zur ausführbaren Datei nicht ermittelt werden kann.
         */
        [[nodiscard]] fs::path executable_path() const override
        {
            wchar_t path[MAX_PATH]{};

            if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
                throw indeterminable_exe_path{};

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
         * @return fs::path Das geteilte Datenverzeichnis (%ALLUSERSAPPDATA%/&lt;appname&gt;).
         * @throws environment_variable_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            // Windows: %ALLUSERSAPPDATA%/&lt;appname&gt;
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

            return fs::path{ allusersappdata } / app_name;
        }

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis (%APPDATA%/&lt;appname&gt;).
         * @throws environment_variable_not_set Wenn die APPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* appdata = std::getenv("APPDATA");

            if (!appdata)
                throw environment_variable_not_set{ "APPDATA" };

            return fs::path{ appdata } / app_name;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Konfigurationsverzeichnis (%APPDATA%/&lt;appname&gt;).
         * @throws environment_variable_not_set Wenn die APPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_config_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* appdata = std::getenv("APPDATA");

            if (!appdata)
                throw environment_variable_not_set{ "APPDATA" };

            return fs::path{ appdata } / app_name;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Cache-Verzeichnis (%LOCALAPPDATA%/&lt;appname&gt;/Cache).
         * @throws environment_variable_not_set Wenn die LOCALAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_cache_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");

            if (!localappdata)
                throw environment_variable_not_set{ "LOCALAPPDATA" };

            return fs::path{ localappdata } / app_name / "Cache";
        }

        /**
         * @brief Gibt das Benutzer-spezifische Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Windows).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Log-Verzeichnis (%LOCALAPPDATA%/&lt;appname&gt;/Logs).
         * @throws environment_variable_not_set Wenn die LOCALAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_log_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* localappdata = std::getenv("LOCALAPPDATA");

            if (!localappdata)
                throw environment_variable_not_set{ "LOCALAPPDATA" };

            return fs::path{ localappdata } / app_name / "Logs";
        }

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis (%TEMP%/&lt;appname&gt;).
         * @throws environment_variable_not_set Wenn die TEMP-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path temp_dir(const std::string& app_name) const override
        {
            const char* temp = std::getenv("TEMP");

            if (!temp)
                throw environment_variable_not_set{ "TEMP" };

            return fs::path{ temp } / app_name;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis des Benutzers (%USERPROFILE%).
         * @throws environment_variable_not_set Wenn die USERPROFILE-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_dir() const override
        {
            const char* userprofile = std::getenv("USERPROFILE");

            if (!userprofile)
                throw environment_variable_not_set{ "USERPROFILE" };

            return fs::path{ userprofile };
        }

        /**
         * @brief Gibt das geteilte Cache-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Cache-Verzeichnis (%ALLUSERSAPPDATA%/&lt;appname&gt;/Cache).
         * @throws environment_variable_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_cache_dir(const std::string& app_name) const override
        {
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

            return fs::path{ allusersappdata } / app_name / "Cache";
        }

        /**
         * @brief Gibt das geteilte Log-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Log-Verzeichnis (%ALLUSERSAPPDATA%/&lt;appname&gt;/Logs).
         * @throws environment_variable_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_log_dir(const std::string& app_name) const override
        {
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

            return fs::path{ allusersappdata } / app_name / "Logs";
        }

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Konfigurationsverzeichnis (%ALLUSERSAPPDATA%/&lt;appname&gt;).
         * @throws environment_variable_not_set Wenn die ALLUSERSAPPDATA-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path shared_config_dir(const std::string& app_name) const override
        {
            const char* allusersappdata = std::getenv("ALLUSERSAPPDATA");

            if (!allusersappdata)
                throw environment_variable_not_set{ "ALLUSERSAPPDATA" };

            return fs::path{ allusersappdata } / app_name;
        }
    };

}
