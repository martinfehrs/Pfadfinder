/**
 * @file
 * @brief Klasse für Systemumgebungs-Pfadermittlung (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#include <filesystem>
#include <optional>
#include <string>

export module pfadfinder:system_environment;

export import :errors;

namespace fs = std::filesystem;

namespace pfadfinder
{
    /**
     * @brief Klasse für Systemumgebungs-Pfadermittlung.
     *
     * Diese Klasse bietet plattformspezifische Implementierungen zur Pfadermittlung.
     * Die konkreten Methodendefinitionen werden durch plattformspezifische Implementierungsmodule bereitgestellt.
     */
    export class default_system_environment
    {
    public:
        /** @brief Virtueller Destruktor für korrekte Ableitung. */
        virtual ~default_system_environment() = default;

        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return std::optional<fs::path> Der absolute Pfad zur ausführbaren Datei oder std::nullopt.
         */
        [[nodiscard]] virtual std::optional<fs::path> executable_path() const;

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * 
         * Unter Linux: /usr/share/\<appname\> (abgeleitet vom Binärverzeichnis).
         * Unter macOS: Resources-Verzeichnis bei gebündelten Anwendungen, sonst share-Verzeichnis.
         * Unter Windows: Gleich dem Binärverzeichnis.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis.
         */
        [[nodiscard]] virtual fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * 
         * Unter Linux: ~/.local/share/\<appname\>.
         * Unter macOS: ~/Library/Application Support/\<appname\> (Bundle) oder ~/.local/share/\<appname\> (CLI).
         * Unter Windows: \%APPDATA\%/\<appname\>.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return std::optional<fs::path> Das Benutzer-Datenverzeichnis oder std::nullopt.
         */
        [[nodiscard]] virtual std::optional<fs::path> data_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-spezifische Konfigurationsverzeichnis zurück.
         * 
         * Unter Linux: ~/.config/\<appname\>.
         * Unter macOS: ~/Library/Preferences/\<appname\> (Bundle) oder ~/.config/\<appname\> (CLI).
         * Unter Windows: \%APPDATA\%/\<appname\>.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return std::optional<fs::path> Das Benutzer-spezifische Konfigurationsverzeichnis oder std::nullopt.
         * @note Umgebungsvariablen zur Pfadüberschreibung werden nicht berücksichtigt.
         */
        [[nodiscard]] virtual std::optional<fs::path> user_config_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-spezifische Cache-Verzeichnis zurück.
         * 
         * Unter Linux: ~/.cache/\<appname\>.
         * Unter macOS: ~/Library/Caches/\<appname\> (Bundle) oder ~/.cache/\<appname\> (CLI).
         * Unter Windows: \%LOCALAPPDATA\%/\<appname\>/Cache.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return std::optional<fs::path> Das Benutzer-spezifische Cache-Verzeichnis oder std::nullopt.
         * @note Umgebungsvariablen zur Pfadüberschreibung werden nicht berücksichtigt.
         */
        [[nodiscard]] virtual std::optional<fs::path> cache_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-spezifische Log-Verzeichnis zurück.
         * 
         * Unter Linux: ~/.local/state/\<appname\>/log.
         * Unter macOS: ~/Library/Logs/\<appname\> (Bundle) oder ~/.local/state/\<appname\>/log (CLI).
         * Unter Windows: \%LOCALAPPDATA\%/\<appname\>/Logs.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return std::optional<fs::path> Das Benutzer-spezifische Log-Verzeichnis oder std::nullopt.
         * @note Umgebungsvariablen zur Pfadüberschreibung werden nicht berücksichtigt.
         */
        [[nodiscard]] virtual std::optional<fs::path> log_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * 
         * Unter Linux: /tmp/\<appname\> oder systemweites Temp-Verzeichnis.
         * Unter macOS: /tmp/\<appname\> oder ~/Library/Caches/TemporaryItems/\<appname\>.
         * Unter Windows: \%TEMP\%/\<appname\>.
         * 
         * @param app_name Der Name der Anwendung.
         * @return std::optional<fs::path> Das temporäre Verzeichnis oder std::nullopt.
         * @note Umgebungsvariablen zur Pfadüberschreibung werden nicht berücksichtigt.
         */
        [[nodiscard]] virtual std::optional<fs::path> temp_dir(const std::string& app_name) const;

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return std::optional<fs::path> Das Home-Verzeichnis oder std::nullopt.
         */
        [[nodiscard]] virtual std::optional<fs::path> home_dir() const;

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * 
         * Unter Linux: /etc/\<appname\>.
         * Unter macOS: /Library/Preferences/\<appname\>.
         * Unter Windows: \%ALLUSERSAPPDATA\%/\<appname\>.
         * 
         * @param app_name Der Name der Anwendung.
         * @return std::optional<fs::path> Das geteilte Konfigurationsverzeichnis oder std::nullopt.
         * 
         * @note Linux-Backend erlaubt nur Lesen von /etc (kleinster gemeinsamer Nenner).
         */
        [[nodiscard]] virtual std::optional<fs::path> shared_config_dir(const std::string& app_name) const;
    };
}
