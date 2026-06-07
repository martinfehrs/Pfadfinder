/**
 * @file
 * @brief Plattformspezifische Implementierung für macOS (pfadfinder:system_backend)
 * @author Martin Fehrs
 */

module;

#include <mach-o/dyld.h>

#include <filesystem>
#include <climits>
#include <string>

export module pfadfinder:system_backend;

import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    // Ausnahmen für macOS
    /**
     * @brief Ausnahme, die geworfen wird, wenn _NSGetExecutablePath() fehlschlägt.
     */
    export struct get_executable_path_failed : error
    {
        get_executable_path_failed() : error("_NSGetExecutablePath failed") {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn realpath() fehlschlägt.
     */
    export struct realpath_failed : error
    {
        realpath_failed() : error("realpath failed") {}
    };

    /**
     * @brief Enthält plattformspezifische Methoden für macOS zur Pfadermittlung.
     * 
     * Diese Klasse implementiert alle statischen Methoden zur Bestimmung
     * von Systempfaden spezifisch für das macOS-Betriebssystem.
     * Sie unterstützt sowohl gebündelte Anwendungen (Bundle) als auch
     * Kommandozeilen-Tools (CLI).
     */
    export struct system_environment
    {
        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws get_executable_path_failed Wenn _NSGetExecutablePath fehlschlägt.
         * @throws realpath_failed Wenn realpath fehlschlägt.
         */
        static fs::path executable_path()
        {
            char path[PATH_MAX] = {0};
            uint32_t size = sizeof(path);
            if (_NSGetExecutablePath(path, &size) != 0)
                throw get_executable_path_failed();
            
            // Symbolische Links auflösen, um den tatsächlichen Pfad zu erhalten
            char real_path[PATH_MAX] = {0};
            if (realpath(path, real_path) == nullptr)
                throw realpath_failed();
            
            return fs::path(real_path);
        }

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis.
         *         Bundle: <appname>.app/Contents/Resources/<appname>
         *         CLI: <exe_dir>/../share/<appname>
         */
        static fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            // macOS: Prüfen, ob wir in einem Bundle sind
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: von .../Contents/MacOS/ zu .../Contents/Resources/
                return exe_dir.parent_path().parent_path() / "Resources" / app_name;
            }
            else
            {
                // Nicht gebündelt: ähnlich wie Linux
                return exe_dir.parent_path() / "share" / app_name;
            }
        }

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter macOS).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis (/Library/Application Support/<appname>).
         */
        static fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name)
        {
            // macOS: /Library/Application Support/<appname>
            return fs::path("/Library/Application Support") / app_name;
        }

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis.
         *         Bundle: ~/Library/Application Support/<appname>
         *         CLI: ~/.local/share/<appname>
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path user_data_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Application Support/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Application Support" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.local/share/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".local" / "share" / app_name;
            }
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Konfigurationsverzeichnis.
         *         Bundle: ~/Library/Preferences/<appname>
         *         CLI: ~/.config/<appname>
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path config_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Preferences/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Preferences" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.config/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".config" / app_name;
            }
        }

        /**
         * @brief Gibt das Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Cache-Verzeichnis.
         *         Bundle: ~/Library/Caches/<appname>
         *         CLI: ~/.cache/<appname>
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path cache_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Caches/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Caches" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.cache/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".cache" / app_name;
            }
        }

        /**
         * @brief Gibt das Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Log-Verzeichnis.
         *         Bundle: ~/Library/Logs/<appname>
         *         CLI: ~/.local/state/<appname>/log (XDG-konform)
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        static fs::path log_dir(const fs::path& exe_dir, const std::string& app_name)
        {
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Logs/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / "Library" / "Logs" / app_name;
            }
            else
            {
                // Nicht gebündelt: ~/.local/state/<appname>/log (XDG-konform)
                const char* home = std::getenv("HOME");
                if (!home)
                    throw home_not_set();
                return fs::path(home) / ".local" / "state" / app_name / "log";
            }
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
