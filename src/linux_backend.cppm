/**
 * @file
 * @brief Plattformspezifische Implementierung für Linux (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <unistd.h>

#include <filesystem>
#include <climits>
#include <string>

export module pfadfinder:linux_backend;

import :system_environment;
import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    /**
     * @brief Ausnahme, die geworfen wird, wenn der Aufruf von readlink() fehlschlägt.
     */
    export struct readlink_failed : error
    {
        readlink_failed()
            : error{ "readlink failed" }
        {}
    };

    /**
     * @brief Enthält plattformspezifische Methoden für Linux zur Pfadermittlung.
     * 
     * Diese Klasse implementiert die system_environment-Schnittstelle
     * spezifisch für das Linux-Betriebssystem.
     */
    export struct linux_system_environment : system_environment
    {
        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws readlink_failed Wenn der readlink-Aufruf fehlschlägt.
         */
        [[nodiscard]] fs::path executable_path() const override
        {
            char path[PATH_MAX]{};
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
 
            if (len == -1)
                throw readlink_failed{};
 
            path[len] = '\0';

            return fs::path{ path };
        }

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis (/usr/share/&lt;appname&gt;).
         */
        [[nodiscard]] fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            // Linux: von /usr/bin/myapp zu /usr/share/myapp
            return exe_dir.parent_path() / "share" / app_name;
        }

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis (/var/lib/&lt;appname&gt;).
         */
        [[nodiscard]] fs::path shared_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            // Linux: /var/lib/&lt;appname&gt;
            return fs::path{ "/var/lib" } / app_name;
        }

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis (~/.local/share/&lt;appname&gt;).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_data_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* home = std::getenv("HOME");
 
            if (!home)
                throw home_not_set{};
 
            return fs::path{ home } / ".local" / "share" / app_name;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Konfigurationsverzeichnis (~/.config/&lt;appname&gt;).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_config_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            return fs::path{ home } / ".config" / app_name;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Cache-Verzeichnis (~/.cache/&lt;appname&gt;).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_cache_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            return fs::path{ home } / ".cache" / app_name;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Log-Verzeichnis (~/.local/state/&lt;appname&gt;/log).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_log_dir([[maybe_unused]] const fs::path& exe_dir, const std::string& app_name) const override
        {
            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            return fs::path{ home } / ".local" / "state" / app_name / "log";
        }

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis (system temp dir / &lt;appname&gt;).
         */
        [[nodiscard]] fs::path temp_dir(const std::string& app_name) const override
        {
            return fs::temp_directory_path() / app_name;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis des Benutzers.
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_dir() const override
        {
            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            return fs::path{ home };
        }

        /**
         * @brief Gibt das geteilte Cache-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Cache-Verzeichnis (/var/cache/&lt;appname&gt;).
         */
        [[nodiscard]] fs::path shared_cache_dir(const std::string& app_name) const override
        {
            return fs::path{ "/var/cache" } / app_name;
        }

        /**
         * @brief Gibt das geteilte Log-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Log-Verzeichnis (/var/log/&lt;appname&gt;).
         */
        [[nodiscard]] fs::path shared_log_dir(const std::string& app_name) const override
        {
            return fs::path{ "/var/log" } / app_name;
        }

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Konfigurationsverzeichnis (/etc/&lt;appname&gt;).
         * 
         * @note Linux-Backend erlaubt nur Lesen von /etc (kleinster gemeinsamer Nenner).
         */
        [[nodiscard]] fs::path shared_config_dir(const std::string& app_name) const override
        {
            return fs::path{ "/etc" } / app_name;
        }
    };
}