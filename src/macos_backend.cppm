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

export module pfadfinder:macos_backend;

import :system_environment;
import :error;

namespace fs = std::filesystem;

namespace pfadfinder::inline macos_backend
{
    /**
     * @brief Ausnahme, die geworfen wird, wenn _NSGetExecutablePath() fehlschlägt.
     */
    export struct get_executable_path_failed : error
    {
        get_executable_path_failed()
            : error{ "_NSGetExecutablePath failed" }
        {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn realpath() fehlschlägt.
     */
    export struct realpath_failed : error
    {
        realpath_failed()
            : error{ "realpath failed" }
        {}
    };

    /**
     * @brief Enthält plattformspezifische Methoden für macOS zur Pfadermittlung.
     * 
     * Diese Klasse implementiert die system_environment-Schnittstelle
     * spezifisch für das macOS-Betriebssystem.
     * Sie unterstützt sowohl gebündelte Anwendungen (Bundle) als auch
     * Kommandozeilen-Tools (CLI).
     */
    export struct default_system_environment : system_environment
    {
        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws get_executable_path_failed Wenn _NSGetExecutablePath fehlschlägt.
         * @throws realpath_failed Wenn realpath fehlschlägt.
         */
        [[nodiscard]] fs::path executable_path() const override
        {
            char path[PATH_MAX]{};
            uint32_t size = sizeof(path);

            if (_NSGetExecutablePath(path, &size) != 0)
                throw get_executable_path_failed{};
            
            char real_path[PATH_MAX]{};

            if (realpath(path, real_path) == nullptr)
                throw realpath_failed{};
            
            return fs::path{ real_path };
        }

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis.
         */
        [[nodiscard]] fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            const auto exe_dir_str = exe_dir.string();

            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                return exe_dir.parent_path().parent_path() / "Resources" / app_name;
            else
                return exe_dir.parent_path() / "share" / app_name;
        }

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei (nicht verwendet unter macOS).
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis.
         */
        [[nodiscard]] fs::path shared_data_dir(const fs::path& /*exe_dir*/, const std::string& app_name) const override
        {
            return fs::path{ "/Library/Application Support" } / app_name;
        }

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis.
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path user_data_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            const auto exe_dir_str = exe_dir.string();

            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                return fs::path{ home } / "Library" / "Application Support" / app_name;
            else
                return fs::path{ home } / ".local" / "share" / app_name;
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Konfigurationsverzeichnis.
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path config_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            const auto exe_dir_str = exe_dir.string();

            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                return fs::path{ home } / "Library" / "Preferences" / app_name;
            else
                return fs::path{ home } / ".config" / app_name;
        }

        /**
         * @brief Gibt das Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Cache-Verzeichnis.
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path cache_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            const auto exe_dir_str = exe_dir.string();

            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                return fs::path{ home } / "Library" / "Caches" / app_name;
            else
                return fs::path{ home } / ".cache" / app_name;
        }

        /**
         * @brief Gibt das Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Log-Verzeichnis.
         * @throws home_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] fs::path log_dir(const fs::path& exe_dir, const std::string& app_name) const override
        {
            const auto exe_dir_str = exe_dir.string();

            const char* home = std::getenv("HOME");

            if (!home)
                throw home_not_set{};

            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                return fs::path{ home } / "Library" / "Logs" / app_name;
            else
                return fs::path{ home } / ".local" / "state" / app_name / "log";
        }

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis.
         */
        [[nodiscard]] fs::path temp_dir(const std::string& app_name) const override
        {
            return fs::temp_directory_path() / app_name;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis.
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
         * @return fs::path Das geteilte Cache-Verzeichnis.
         */
        [[nodiscard]] fs::path shared_cache_dir(const std::string& app_name) const override
        {
            return fs::path{ "/Library/Caches" } / app_name;
        }

        /**
         * @brief Gibt das geteilte Log-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Log-Verzeichnis.
         */
        [[nodiscard]] fs::path shared_log_dir(const std::string& app_name) const override
        {
            return fs::path{ "/Library/Logs" } / app_name;
        }

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Konfigurationsverzeichnis.
         */
        [[nodiscard]] fs::path shared_config_dir(const std::string& app_name) const override
        {
            return fs::path{ "/Library/Preferences" } / app_name;
        }
    };
}