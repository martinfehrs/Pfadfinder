/**
 * @file
 * @brief Abstrakte Basisklasse für System-Backends (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#include <filesystem>
#include <string>

export module pfadfinder:system_environment;

namespace fs = std::filesystem;

namespace pfadfinder
{
    /**
     * @brief Abstrakte Basisklasse für Systemumgebungs-Backends.
     *
     * Diese Klasse definiert die Schnittstelle für plattformspezifische
     * Implementierungen zur Pfadermittlung. Alle konkreten Backends
     * (Linux, Windows, macOS) müssen von dieser Klasse ableiten und
     * alle virtuellen Methoden implementieren.
     *
     * Die Klasse ist als Interface konzipiert - alle Methoden sind
     * rein virtuell (ab C++20 mit = 0) und es gibt keinen Zustand.
     * Dies ermöglicht einfache Mock-Implementierungen für Tests.
     */
    export class system_environment
    {
    public:
        /** @brief Virtueller Destruktor für korrekte Ableitung. */
        virtual ~system_environment() = default;

        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws Plattformspezifische Ausnahmen bei Fehlern.
         */
        [[nodiscard]] virtual fs::path executable_path() const = 0;

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis.
         */
        [[nodiscard]] virtual fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name) const = 0;

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis.
         */
        [[nodiscard]] virtual fs::path shared_data_dir(const fs::path& exe_dir, const std::string& app_name) const = 0;

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis.
         */
        [[nodiscard]] virtual fs::path user_data_dir(const fs::path& exe_dir, const std::string& app_name) const = 0;

        /**
         * @brief Gibt das Konfigurationsverzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Konfigurationsverzeichnis.
         */
        [[nodiscard]] virtual fs::path config_dir(const fs::path& exe_dir, const std::string& app_name) const = 0;

        /**
         * @brief Gibt das Cache-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Cache-Verzeichnis.
         */
        [[nodiscard]] virtual fs::path cache_dir(const fs::path& exe_dir, const std::string& app_name) const = 0;

        /**
         * @brief Gibt das Log-Verzeichnis zurück.
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Log-Verzeichnis.
         */
        [[nodiscard]] virtual fs::path log_dir(const fs::path& exe_dir, const std::string& app_name) const = 0;

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis.
         */
        [[nodiscard]] virtual fs::path temp_dir(const std::string& app_name) const = 0;

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis.
         */
        [[nodiscard]] virtual fs::path user_dir() const = 0;

        /**
         * @brief Gibt das geteilte Cache-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Cache-Verzeichnis.
         */
        [[nodiscard]] virtual fs::path shared_cache_dir(const std::string& app_name) const = 0;

        /**
         * @brief Gibt das geteilte Log-Verzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Log-Verzeichnis.
         */
        [[nodiscard]] virtual fs::path shared_log_dir(const std::string& app_name) const = 0;

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Konfigurationsverzeichnis.
         */
        [[nodiscard]] virtual fs::path shared_config_dir(const std::string& app_name) const = 0;
    };
}
