/**
 * @file
 * @brief  Plattformübergreifende Bestimmung von Anwendungsverzeichnissen
 * @author Martin Fehrs
 *
 * Dieses Modul bietet:
 * 
 * 1. Die Klasse pfadfinder::application_environment mit Methoden zur Ermittlung
 *    plattformspezifischer Verzeichnispfade für Anwendungen (Windows, Linux, macOS):
 *    - executable_path()      : Vollständiger Pfad zur ausführbaren Datei
 *    - executable_directory() : Verzeichnis der ausführbaren Datei
 *    - data_directory()       : Systemweites Datenverzeichnis
 *    - user_data_directory()  : Benutzer-spezifisches Datenverzeichnis
 *    - config_directory()     : Konfigurationsverzeichnis
 *    - cache_directory()      : Cache-Verzeichnis
 *    - log_directory()        : Log-Verzeichnis für Anwendungsprotokolle
 *    - temp_directory()       : Temporäres Verzeichnis für die Anwendung
 *    - user_directory()       : Home-Verzeichnis des Benutzers
 */

module;

#include <cstdlib>
#include <filesystem>
#include <string>
#include <optional>

export module pfadfinder;

export import :system_backend;
export import :error;

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
     * Alle Methoden geben fs::path zurück und können Ausnahmen werfen.
     * 
     * Die Ergebnisse der Methoden werden als Objektvariablen gecacht, um wiederholte
     * Berechnungen zu vermeiden.
     */
    export class application_environment
    {

    public:
  
        /**
         * @brief Erstellt eine neue application_environment Instanz mit leerem App-Namen.
         */
        application_environment() = default;

        /**
         * @brief Erstellt eine neue application_environment Instanz.
         * @param app_name Der Name der Anwendung, der für Verzeichnispfade verwendet wird.
         */
        explicit application_environment(std::string app_name)
            : app_name_(std::move(app_name))
        {}

        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
         *        Setzt app_name_ auf den Dateinamen (ohne Dateierweiterung), falls app_name_ leer ist.
         * @return fs::path Der vollständige Pfad zur ausführbaren Datei.
         * @throws Ausnahmen je nach Plattform (z.B. readlink_failed, get_module_file_name_failed)
         */
        [[nodiscard]] fs::path executable_path() const
        {
            if (!cached_executable_path_.has_value())
            {
                auto path = system_env_.executable_path();
                // Setze app_name_ auf den Dateinamen ohne Dateierweiterung, falls noch nicht gesetzt
                if (app_name_.empty())
                    app_name_ = path.stem().string();

                cached_executable_path_ = path;
            }

            return *cached_executable_path_;
        }

        /**
         * @brief Gibt das Verzeichnis der ausführbaren Datei zurück.
         * @return fs::path Das Verzeichnis, das die ausführbare Datei enthält.
         */
        [[nodiscard]] fs::path executable_directory() const
        {
            if (!cached_executable_directory_.has_value())
                cached_executable_directory_ = executable_path().parent_path();

            return *cached_executable_directory_;
        }

        /**
         * @brief Gibt das Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies dem Binärverzeichnis.
         * Unter Linux wird das share-Verzeichnis aus dem Binärverzeichnis abgeleitet
         * (z. B. /usr/bin/myapp → /usr/share/myapp).
         * Unter macOS wird bei gebündelten Anwendungen das Resources-Verzeichnis
         * zurückgegeben, ansonsten ähnlich wie Linux das share-Verzeichnis.
         * 
         * @return fs::path Das Datenverzeichnis der Anwendung.
         */
        [[nodiscard]] fs::path data_directory() const
        {
            if (!cached_data_directory_.has_value())
                cached_data_directory_ = system_env_.data_directory(executable_directory(), app_name_);
            
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
         * @param create Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.
         * @return fs::path Das Benutzer-Datenverzeichnis der Anwendung.
         */
        [[nodiscard]] fs::path user_data_directory(bool create = true) const
        {
            if (!cached_user_data_directory_.has_value())
                cached_user_data_directory_ = system_env_.user_data_directory(executable_directory(), app_name_);

            if (create && !cached_user_data_directory_->empty())
                fs::create_directories(*cached_user_data_directory_);

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
         * @param create Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.
         * @return fs::path Das Konfigurationsverzeichnis der Anwendung.
         */
        [[nodiscard]] fs::path config_directory(bool create = true) const
        {
            if (!cached_config_directory_.has_value())
                cached_config_directory_ = system_env_.config_directory(executable_directory(), app_name_);

            if (create && !cached_config_directory_->empty())
                fs::create_directories(*cached_config_directory_);

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
         * @param create Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.
         * @return fs::path Das Cache-Verzeichnis der Anwendung.
         */
        [[nodiscard]] fs::path cache_directory(bool create = true) const
        {
            if (!cached_cache_directory_.has_value())
                cached_cache_directory_ = system_env_.cache_directory(executable_directory(), app_name_);

            if (create && !cached_cache_directory_->empty())
                fs::create_directories(*cached_cache_directory_);

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
         * @param create Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.
         * @return fs::path Das Log-Verzeichnis der Anwendung.
         */
        [[nodiscard]] fs::path log_directory(bool create = true) const
        {
            if (!cached_log_directory_.has_value())
                cached_log_directory_ = system_env_.log_directory(executable_directory(), app_name_);

            if (create && !cached_log_directory_->empty())
                fs::create_directories(*cached_log_directory_);

            return *cached_log_directory_;
        }

        /**
         * @brief Gibt das temporäre Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %TEMP%/<appname>.
         * Unter Linux entspricht dies /tmp/<appname> oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname> oder ~/Library/Caches/TemporaryItems/<appname>.
         * 
         * @param create Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.
         * @return fs::path Das temporäre Verzeichnis der Anwendung.
         */
        [[nodiscard]] fs::path temp_directory(bool create = true) const
        {
            if (!cached_temp_directory_.has_value())
                cached_temp_directory_ = system_env_.temp_directory(app_name_);

            if (create && !cached_temp_directory_->empty())
                fs::create_directories(*cached_temp_directory_);

            return *cached_temp_directory_;
        }

        /**
         * @brief Gibt den absoluten Pfad zu einer Datei im Datenverzeichnis zurück.
         * 
         * Sucht nach der durch rel_path angegebenen Datei im durch data_directory()
         * zurückgegebenen Verzeichnis.
         * 
         * @param rel_path Relativer Pfad zur Datei innerhalb des Datenverzeichnisses.
         * @return fs::path Absoluter Pfad zur Datei.
         * @throws file_not_found Wenn die Datei nicht gefunden wurde.
         */
        [[nodiscard]] fs::path data_file(const fs::path& rel_path) const
        {
            auto file_path = data_directory() / rel_path;
            if (!fs::exists(file_path) || !fs::is_regular_file(file_path))
                throw file_not_found(file_path.string());
            return file_path;
        }

        /**
         * @brief Gibt den absoluten Pfad zu einer Datei im Benutzer-Datenverzeichnis zurück.
         * 
         * Sucht nach der durch rel_path angegebenen Datei im durch user_data_directory()
         * zurückgegebenen Verzeichnis.
         * 
         * @param rel_path Relativer Pfad zur Datei innerhalb des Benutzer-Datenverzeichnisses.
         * @return fs::path Absoluter Pfad zur Datei.
         * @throws file_not_found Wenn die Datei nicht gefunden wurde.
         */
        [[nodiscard]] fs::path user_data_file(const fs::path& rel_path) const
        {
            auto file_path = user_data_directory() / rel_path;
            if (!fs::exists(file_path) || !fs::is_regular_file(file_path))
                throw file_not_found(file_path.string());
            return file_path;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * 
         * Unter Windows entspricht dies %USERPROFILE%.
         * Unter Linux und macOS entspricht dies $HOME.
         * 
         * @return fs::path Das Home-Verzeichnis des Benutzers.
         */
        [[nodiscard]] fs::path user_directory() const
        {
            if (!cached_user_directory_.has_value())
                cached_user_directory_ = system_env_.user_directory();

            return *cached_user_directory_;
        }

    private:

        mutable std::string app_name_;
        [[no_unique_address]] system_environment system_env_;

        // Cache für die berechneten Pfade (Lazy Initialization)
        mutable std::optional<fs::path> cached_executable_path_;
        mutable std::optional<fs::path> cached_executable_directory_;
        mutable std::optional<fs::path> cached_data_directory_;
        mutable std::optional<fs::path> cached_user_data_directory_;
        mutable std::optional<fs::path> cached_config_directory_;
        mutable std::optional<fs::path> cached_cache_directory_;
        mutable std::optional<fs::path> cached_log_directory_;
        mutable std::optional<fs::path> cached_temp_directory_;
        mutable std::optional<fs::path> cached_user_directory_;
    };

}
