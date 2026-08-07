/**
 * @file
 * @brief Klasse für Systemumgebungs-Pfadermittlung (pfadfinder:system_environment)
 * @author Martin Fehrs
 */

module;

#include <filesystem>
#include <string>

export module pfadfinder:system_environment;

export import :error;

namespace fs = std::filesystem;

namespace pfadfinder
{
    /**
     * @brief Klasse für Systemumgebungs-Pfadermittlung.
     *
     * Diese Klasse bietet plattformspezifische Implementierungen zur Pfadermittlung.
     * Die konkreten Methodendefinitionen werden durch Implementierungsmodule bereitgestellt,
     * die von CMake basierend auf der Zielplattform ausgewählt werden.
     */
    export class system_environment
    {
    public:
        /** @brief Virtueller Destruktor für korrekte Ableitung. */
        virtual ~system_environment() = default;

        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return fs::path Der absolute Pfad zur ausführbaren Datei.
         * @throws indeterminable_exe_path Wenn der Pfad zur ausführbaren Datei nicht ermittelt werden kann.
         */
        [[nodiscard]] virtual fs::path executable_path() const;

        /**
         * @brief Gibt das statische Datenverzeichnis zurück.
         * 
         * Unter Linux: /usr/share/<appname> (abgeleitet vom Binärverzeichnis).
         * Unter macOS: Resources-Verzeichnis bei gebündelten Anwendungen, sonst share-Verzeichnis.
         * Unter Windows: Gleich dem Binärverzeichnis.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das statische Datenverzeichnis.
         */
        [[nodiscard]] virtual fs::path static_data_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das geteilte Datenverzeichnis zurück.
         * 
         * Unter Linux: /var/lib/<appname>.
         * Unter macOS: /Library/Application Support/<appname>.
         * Unter Windows: %ALLUSERSAPPDATA%/<appname>.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Datenverzeichnis.
         */
        [[nodiscard]] virtual fs::path shared_data_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-Datenverzeichnis zurück.
         * 
         * Unter Linux: ~/.local/share/<appname>.
         * Unter macOS: ~/Library/Application Support/<appname> (Bundle) oder ~/.local/share/<appname> (CLI).
         * Unter Windows: %APPDATA%/<appname>.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-Datenverzeichnis.
         * @throws environment_variable_not_set Wenn eine benötigte Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path user_data_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-spezifische Konfigurationsverzeichnis zurück.
         * 
         * Unter Linux: ~/.config/<appname> (XDG-Standard).
         * Unter macOS: ~/Library/Preferences/<appname> (Bundle) oder ~/.config/<appname> (CLI).
         * Unter Windows: %APPDATA%/<appname>.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Konfigurationsverzeichnis.
         * @throws environment_variable_not_set Wenn eine benötigte Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path user_config_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-spezifische Cache-Verzeichnis zurück.
         * 
         * Unter Linux: ~/.cache/<appname> (XDG-Standard).
         * Unter macOS: ~/Library/Caches/<appname> (Bundle) oder ~/.cache/<appname> (CLI).
         * Unter Windows: %LOCALAPPDATA%/<appname>/Cache.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Cache-Verzeichnis.
         * @throws environment_variable_not_set Wenn eine benötigte Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path user_cache_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das Benutzer-spezifische Log-Verzeichnis zurück.
         * 
         * Unter Linux: ~/.local/state/<appname>/log (XDG Base Directory Specification).
         * Unter macOS: ~/Library/Logs/<appname> (Bundle) oder ~/.local/state/<appname>/log (CLI).
         * Unter Windows: %LOCALAPPDATA%/<appname>/Logs.
         * 
         * @param exe_dir Das Verzeichnis der ausführbaren Datei.
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das Benutzer-spezifische Log-Verzeichnis.
         * @throws environment_variable_not_set Wenn eine benötigte Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path user_log_dir(const fs::path& exe_dir, const std::string& app_name) const;

        /**
         * @brief Gibt das temporäre Verzeichnis zurück.
         * 
         * Unter Linux: /tmp/<appname> oder systemweites Temp-Verzeichnis.
         * Unter macOS: /tmp/<appname> oder ~/Library/Caches/TemporaryItems/<appname>.
         * Unter Windows: %TEMP%/<appname>.
         * 
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das temporäre Verzeichnis.
         * @throws environment_variable_not_set Wenn eine benötigte Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path temp_dir(const std::string& app_name) const;

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * @return fs::path Das Home-Verzeichnis.
         * @throws environment_variable_not_set Wenn die HOME-Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path user_dir() const;

        /**
         * @brief Gibt das geteilte Cache-Verzeichnis zurück.
         * 
         * Unter Linux: /var/cache/<appname>.
         * Unter macOS: /Library/Caches/<appname>.
         * Unter Windows: %ALLUSERSAPPDATA%/<appname>/Cache.
         * 
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Cache-Verzeichnis.
         * @throws environment_variable_not_set Wenn eine benötigte Umgebungsvariable nicht gesetzt ist.
         */
        [[nodiscard]] virtual fs::path shared_cache_dir(const std::string& app_name) const;

        /**
         * @brief Gibt das geteilte Log-Verzeichnis zurück.
         * 
         * Unter Linux: /var/log/<appname>.
         * Unter macOS: /Library/Logs/<appname>.
         * Unter Windows: %ALLUSERSAPPDATA%/<appname>/Logs.
         * 
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Log-Verzeichnis.
         */
        [[nodiscard]] virtual fs::path shared_log_dir(const std::string& app_name) const;

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis zurück.
         * 
         * Unter Linux: /etc/<appname>.
         * Unter macOS: /Library/Preferences/<appname>.
         * Unter Windows: %ALLUSERSAPPDATA%/<appname>.
         * 
         * @param app_name Der Name der Anwendung.
         * @return fs::path Das geteilte Konfigurationsverzeichnis.
         * 
         * @note Linux-Backend erlaubt nur Lesen von /etc (kleinster gemeinsamer Nenner).
         */
        [[nodiscard]] virtual fs::path shared_config_dir(const std::string& app_name) const;
    };
}
