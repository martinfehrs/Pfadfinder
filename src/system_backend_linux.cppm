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

export module pfadfinder:system_backend;

import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für Linux
    /**
     * @brief Ausnahme, die geworfen wird, wenn der Aufruf von readlink() fehlschlägt.
     */
    export struct readlink_failed : error
    {
        readlink_failed() : error("readlink failed") {}
    };

    /**
     * @brief Enthält plattformspezifische Methoden für Linux zur Pfadermittlung.
     * 
     * Diese Klasse implementiert alle statischen Methoden zur Bestimmung
     * von Systempfaden spezifisch für das Linux-Betriebssystem.
     */
    export struct system_environment
    {
        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws readlink_failed Wenn der readlink-Aufruf fehlschlägt.
         */
        static fs::path executable_path()
        {
            char path[PATH_MAX] = {0};
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
            if (len == -1)
                throw readlink_failed();
            path[len] = '\0';
            return fs::path(path);
        }

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis (/usr/share/<appname>).
         */
        static fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            // Linux: von /usr/bin/myapp zu /usr/share/myapp
            return exe_dir.parent_path() / "share" / app_name;
        }

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis (/var/lib/<appname>).
         */
        static fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            // Linux: /var/lib/<appname>
            return fs::path("/var/lib") / app_name;
        }

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis (~/.local/share/<appname>).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path user_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".local" / "share" / app_name;
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Konfigurationsverzeichnis (~/.config/<appname>).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path config_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".config" / app_name;
        }

        /**
         * @brief Gibt das Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Cache-Verzeichnis (~/.cache/<appname>).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path cache_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".cache" / app_name;
        }

        /**
         * @brief Gibt das Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter Linux).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Log-Verzeichnis (~/.local/state/<appname>/log).
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path log_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home) / ".local" / "state" / app_name / "log";
        }

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis (system temp dir / <appname>).
         */
        static fs::path temp_dir(const std::string& app_name)
        {
            return fs::temp_directory_path() / app_name;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis des Benutzers.
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path user_dir()
        {
            const char* home = std::getenv("HOME");
            if (!home)
                throw home_not_set();
            return fs::path(home);
        }
    };

}
