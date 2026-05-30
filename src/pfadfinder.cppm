/**
 * @file
 * @brief  Plattformübergreifende Bestimmung von Anwendungsverzeichnissen
 * @author Martin Fehrs
 *
 * Dieses Modul bietet:
 * 
 * 1. Die Klasse pfadfinder::application_environment mit Methoden zur Ermittlung
 *    plattformspezifischer Verzeichnispfade für Anwendungen (Windows, Linux, macOS):
 *    - executable_path()     : Vollständiger Pfad zur ausführbaren Datei
 *    - executable_directory(): Verzeichnis der ausführbaren Datei
 *    - data_directory()      : Systemweites Datenverzeichnis
 *    - user_data_directory() : Benutzer-spezifisches Datenverzeichnis
 *    - config_directory()    : Konfigurationsverzeichnis
 *    - cache_directory()     : Cache-Verzeichnis
 *    - log_directory()       : Log-Verzeichnis für Anwendungsprotokolle
 *    - temp_directory()      : Temporäres Verzeichnis für die Anwendung
 *    - user_directory()      : Home-Verzeichnis des Benutzers
 * 
 * 2. Fehlerbehandlung:
 *    - enum class error_code : Fehlertypen für alle Pfadfunktionen (von Backend-Partition bereitgestellt)
 *    - error_message()       : Menschlesbare Fehlermeldungen für error_code-Werte
 * 
 * Alle Pfadfunktionen geben std::expected<fs::path, error_code> zurück.
 */

module;

#include <cstdlib>
#include <expected>
#include <filesystem>
#include <string>

export module pfadfinder;

export import :system_backend;

namespace fs = std::filesystem;

namespace pfadfinder
{

    /**
     * @brief Stellt Methoden zur Bestimmung verschiedener Verzeichnisse einer Anwendung bereit.
     * 
     * Diese Klasse kapselt die Logik zur Ermittlung von Pfaden wie dem Executable-Pfad,
     * Datenverzeichnis, Konfigurationsverzeichnis und Cache-Verzeichnis für verschiedene
     * Plattformen (Windows, Linux, macOS).
     * 
     * Alle Methoden geben std::expected<fs::path, error_code> zurück,
     * wobei error_code eine typsichere Aufzählung ist, die die möglichen Fehlerfälle
     * beschreibt.
     * 
     * Die Ergebnisse der Methoden werden als Objektvariablen gecacht, um wiederholte
     * Berechnungen zu vermeiden.
     */
    export class application_environment
    {
    public:
        /**
         * @brief Erstellt eine neue application_environment Instanz.
         * @param app_name Der Name der Anwendung, der für Verzeichnispfade verwendet wird.
         */
        explicit application_environment(std::string app_name)
            : app_name_(std::move(app_name))
        {}

        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         * @return std::expected<fs::path, error_code> Der vollständige Pfad zur
         *         ausführbaren Datei oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> executable_path() const
        {
            if (!cached_executable_path_.has_value())
            {
                auto result = get_executable_path();
                if (!result)
                    cached_executable_path_ = std::unexpected(result.error());
                else
                    cached_executable_path_ = result;
            }
            return *cached_executable_path_;
        }

        /**
         * @brief Gibt das Verzeichnis der ausführbaren Datei zurück.
         * @return std::expected<fs::path, error_code> Das Verzeichnis, das
         *         die ausführbare Datei enthält oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> executable_directory() const
        {
            if (!cached_executable_directory_.has_value())
            {
                auto result = executable_path();
                if (!result)
                    cached_executable_directory_ = std::unexpected(result.error());
                else
                    cached_executable_directory_ = result->parent_path();
            }
            return *cached_executable_directory_;
        }

        /**
         * @brief Gibt das Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies dem Binärverzeichnis (ggf. mit Unterverzeichnis).
         * Unter Linux wird das share-Verzeichnis aus dem Binärverzeichnis abgeleitet
         * (z. B. /usr/bin/myapp → /usr/share/myapp).
         * Unter macOS wird bei gebündelten Anwendungen das Resources-Verzeichnis
         * zurückgegeben, ansonsten ähnlich wie Linux das share-Verzeichnis.
         * 
         * @return std::expected<fs::path, error_code> Das Datenverzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> data_directory() const
        {
            if (cached_data_directory_.has_value())
                return *cached_data_directory_;
   
            if (auto exe_dir = executable_directory(); !exe_dir)
            {
                cached_data_directory_ = std::unexpected(exe_dir.error());
            }
            else
            {
                if (auto result = get_data_directory(*exe_dir); !result)
                    cached_data_directory_ = std::unexpected(result.error());
                else
                    cached_data_directory_ = *result / app_name_;
            }
                
            return *cached_data_directory_;
        }

        /**
         * @brief Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>.
         * Unter Linux entspricht dies ~/.local/share/<appname>.
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Application Support/<appname>, ansonsten ~/.local/share/<appname>.
         * 
         * @return std::expected<fs::path, error_code> Das Benutzer-Datenverzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> user_data_directory() const
        {
            if (!cached_user_data_directory_.has_value())
            {
                auto exe_dir_result = executable_directory();
                if (!exe_dir_result)
                {
                    cached_user_data_directory_ = std::unexpected(exe_dir_result.error());
                }
                else
                {
                    auto exe_dir = *exe_dir_result;
                    auto result = get_user_data_directory(exe_dir, app_name_);
                    if (!result)
                        cached_user_data_directory_ = std::unexpected(result.error());
                    else
                        cached_user_data_directory_ = result;
                }
            }
            return *cached_user_data_directory_;
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>.
         * Unter Linux entspricht dies ~/.config/<appname> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Preferences/<appname>, ansonsten ~/.config/<appname>.
         * 
         * @return std::expected<fs::path, error_code> Das Konfigurationsverzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> config_directory() const
        {
            if (!cached_config_directory_.has_value())
            {
                auto exe_dir_result = executable_directory();
                if (!exe_dir_result)
                {
                    cached_config_directory_ = std::unexpected(exe_dir_result.error());
                }
                else
                {
                    auto exe_dir = *exe_dir_result;
                    auto result = get_config_directory(exe_dir, app_name_);
                    if (!result)
                        cached_config_directory_ = std::unexpected(result.error());
                    else
                        cached_config_directory_ = result;
                }
            }
            return *cached_config_directory_;
        }

        /**
         * @brief Gibt das Cache-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Cache.
         * Unter Linux entspricht dies ~/.cache/<appname> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Caches/<appname>, ansonsten ~/.cache/<appname>.
         * 
         * @return std::expected<fs::path, error_code> Das Cache-Verzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> cache_directory() const
        {
            if (!cached_cache_directory_.has_value())
            {
                auto exe_dir_result = executable_directory();
                if (!exe_dir_result)
                {
                    cached_cache_directory_ = std::unexpected(exe_dir_result.error());
                }
                else
                {
                    auto exe_dir = *exe_dir_result;
                    auto result = get_cache_directory(exe_dir, app_name_);
                    if (!result)
                        cached_cache_directory_ = std::unexpected(result.error());
                    else
                        cached_cache_directory_ = result;
                }
            }
            return *cached_cache_directory_;
        }

        /**
         * @brief Gibt das Log-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Logs.
         * Unter Linux entspricht dies ~/.local/state/<appname>/log (XDG Base Directory Specification).
         * Unter macOS (Bundle) entspricht dies ~/Library/Logs/<appname>.
         * Unter macOS (CLI) entspricht dies ~/.local/state/<appname>/log.
         * 
         * @return std::expected<fs::path, error_code> Das Log-Verzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> log_directory() const
        {
            if (!cached_log_directory_.has_value())
            {
                auto exe_dir_result = executable_directory();
                if (!exe_dir_result)
                {
                    cached_log_directory_ = std::unexpected(exe_dir_result.error());
                }
                else
                {
                    auto exe_dir = *exe_dir_result;
                    auto result = get_log_directory(exe_dir, app_name_);
                    if (!result)
                        cached_log_directory_ = std::unexpected(result.error());
                    else
                        cached_log_directory_ = result;
                }
            }
            return *cached_log_directory_;
        }

        /**
         * @brief Gibt das temporäre Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %TEMP%/<appname>.
         * Unter Linux entspricht dies /tmp/<appname> oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname> oder ~/Library/Caches/TemporaryItems/<appname>.
         * 
         * @return std::expected<fs::path, error_code> Das temporäre Verzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> temp_directory() const
        {
            if (!cached_temp_directory_.has_value())
            {
                auto result = get_system_temp_directory();
                if (!result)
                    cached_temp_directory_ = std::unexpected(result.error());
                else
                    cached_temp_directory_ = *result / app_name_;
            }
            return *cached_temp_directory_;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * 
         * Unter Windows entspricht dies %USERPROFILE%.
         * Unter Linux und macOS entspricht dies $HOME.
         * 
         * @return std::expected<fs::path, error_code> Das Home-Verzeichnis
         *         des Benutzers oder ein Fehlercode.
         */
        std::expected<fs::path, error_code> user_directory() const
        {
            if (!cached_user_directory_.has_value())
            {
                auto result = get_user_directory();
                if (!result)
                    cached_user_directory_ = std::unexpected(result.error());
                else
                    cached_user_directory_ = result;
            }
            return *cached_user_directory_;
        }

    private:
        std::string app_name_;

        // Cache für die berechneten Pfade (Lazy Initialization)
        mutable std::optional<std::expected<fs::path, error_code>> cached_executable_path_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_executable_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_data_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_user_data_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_config_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_cache_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_log_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_temp_directory_;
        mutable std::optional<std::expected<fs::path, error_code>> cached_user_directory_;
    };

}

