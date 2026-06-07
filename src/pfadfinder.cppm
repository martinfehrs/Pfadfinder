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
 *    - executable_dir()       : Verzeichnis der ausführbaren Datei
 *    - static_data_dir()      : Systemweites statisches Datenverzeichnis
 *    - shared_data_dir()      : Systemweites geteiltes Datenverzeichnis
 *    - user_data_dir()        : Benutzer-spezifisches Datenverzeichnis (mit create_dir Parameter)
 *    - config_dir()           : Konfigurationsverzeichnis (mit create_dir Parameter)
 *    - cache_dir()            : Cache-Verzeichnis (mit create_dir Parameter)
 *    - log_dir()              : Log-Verzeichnis für Anwendungsprotokolle (mit create_dir Parameter)
 *    - temp_dir()             : Temporäres Verzeichnis für die Anwendung (mit create_dir Parameter)
 *    - user_dir()             : Home-Verzeichnis des Benutzers
 *    - static_data_file()     : Absoluter Pfad zu einer Datei im statischen Datenverzeichnis
 *    - user_data_file()       : Absoluter Pfad zu einer Datei im Benutzer-Datenverzeichnis
 *    - cache_file()           : Absoluter Pfad zu einer Datei im Cache-Verzeichnis
 *    - log_file()             : Absoluter Pfad zu einer Datei im Log-Verzeichnis
 *    - temp_file()            : Absoluter Pfad zu einer Datei im temporären Verzeichnis
 */

module;

#include <filesystem>
#include <string>
#include <optional>
#include <utility>

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
     * Die Ergebnisse der Methoden werden als Objektvariablen gecacht, um wiederholte
     * Berechnungen zu vermeiden.
     * 
     * @tparam SystemEnvironment Typ des Backend-Implementierung, Standardmäßig pfadfinder::default_system_environment.
     * 
     * @note Wenn kein app_name angegeben wird, wird der Dateiname der ausführbaren Datei verwendet.
     */
    export template <typename SystemEnvironment = default_system_environment>
    class application_environment
    {

    public:
  
        /**
         * @brief Erstellt eine neue application_environment Instanz.
         * @param app_name Der Name der Anwendung (optional).
         *                 Wenn nicht angegeben, wird der Dateiname der ausführbaren Datei verwendet.
         * @param system_env Backend-Implementierung für Pfadermittlung.
         */
        explicit application_environment(std::string app_name = "", SystemEnvironment system_env = SystemEnvironment{})
            : app_name_(std::move(app_name)), system_env_(std::move_if_noexcept(system_env))
        {}

        /**
         * @brief Gibt den vollständigen Pfad zur ausführbaren Datei zurück.
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
        [[nodiscard]] fs::path executable_dir() const
        {
            if (!cached_executable_dir_.has_value())
                cached_executable_dir_ = executable_path().parent_path();

            return *cached_executable_dir_;
        }

        /**
         * @brief Gibt das statische Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies dem Binärverzeichnis.
         * Unter Linux wird das share-Verzeichnis aus dem Binärverzeichnis abgeleitet
         * (z. B. /usr/bin/myapp → /usr/share/myapp).
         * Unter macOS wird bei gebündelten Anwendungen das Resources-Verzeichnis
         * zurückgegeben, ansonsten ähnlich wie Linux das share-Verzeichnis.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis (optional).
         * @return fs::path Das statische Datenverzeichnis der Anwendung (Basis oder Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert.
         */
        [[nodiscard]] fs::path static_data_dir(const fs::path& rel_path = "") const
        {
            auto path = get_static_data_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das geteilte Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %ALLUSERSAPPDATA%/<appname>.
         * Unter Linux entspricht dies /var/lib/<appname>.
         * Unter macOS entspricht dies /Library/Application Support/<appname>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis (optional).
         * @return fs::path Das geteilte Datenverzeichnis der Anwendung (Basis oder Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert.
         */
        [[nodiscard]] fs::path shared_data_dir(const fs::path& rel_path = "") const
        {
            auto path = get_shared_data_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>.
         * Unter Linux entspricht dies ~/.local/share/<appname>.
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Application Support/<appname>, ansonsten ~/.local/share/<appname>.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-Datenverzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_data_dir(bool create_dir = true) const
        {
            auto path = get_user_data_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das benutzerspezifische Datenverzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>/<rel_path>.
         * Unter Linux entspricht dies ~/.local/share/<appname>/<rel_path>.
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Application Support/<appname>/<rel_path>, ansonsten ~/.local/share/<appname>/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-Datenverzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_data_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_user_data_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>.
         * Unter Linux entspricht dies ~/.config/<appname> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Preferences/<appname>, ansonsten ~/.config/<appname>.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Konfigurationsverzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path config_dir(bool create_dir = true) const
        {
            auto path = get_config_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Konfigurationsverzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>/<rel_path>.
         * Unter Linux entspricht dies ~/.config/<appname>/<rel_path> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Preferences/<appname>/<rel_path>, ansonsten ~/.config/<appname>/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Konfigurationsverzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path config_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_config_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Cache-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Cache.
         * Unter Linux entspricht dies ~/.cache/<appname> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Caches/<appname>, ansonsten ~/.cache/<appname>.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Cache-Verzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path cache_dir(bool create_dir = true) const
        {
            auto path = get_cache_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Cache-Verzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Cache/<rel_path>.
         * Unter Linux entspricht dies ~/.cache/<appname>/<rel_path> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Caches/<appname>/<rel_path>, ansonsten ~/.cache/<appname>/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Cache-Verzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path cache_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_cache_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Log-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Logs.
         * Unter Linux entspricht dies ~/.local/state/<appname>/log (XDG Base Directory Specification).
         * Unter macOS (Bundle) entspricht dies ~/Library/Logs/<appname>.
         * Unter macOS (CLI) entspricht dies ~/.local/state/<appname>/log.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Log-Verzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path log_dir(bool create_dir = true) const
        {
            auto path = get_log_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Log-Verzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Logs/<rel_path>.
         * Unter Linux entspricht dies ~/.local/state/<appname>/log/<rel_path> (XDG Base Directory Specification).
         * Unter macOS (Bundle) entspricht dies ~/Library/Logs/<appname>/<rel_path>.
         * Unter macOS (CLI) entspricht dies ~/.local/state/<appname>/log/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Log-Verzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path log_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_log_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das temporäre Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %TEMP%/<appname>.
         * Unter Linux entspricht dies /tmp/<appname> oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname> oder ~/Library/Caches/TemporaryItems/<appname>.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das temporäre Verzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path temp_dir(bool create_dir = true) const
        {
            auto path = get_temp_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das temporäre Verzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %TEMP%/<appname>/<rel_path>.
         * Unter Linux entspricht dies /tmp/<appname>/<rel_path> oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname>/<rel_path> oder ~/Library/Caches/TemporaryItems/<appname>/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das temporäre Verzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path temp_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_temp_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * 
         * Unter Windows entspricht dies %USERPROFILE%.
         * Unter Linux und macOS entspricht dies $HOME.
         * 
         * @return fs::path Das Home-Verzeichnis des Benutzers.
         */
        [[nodiscard]] fs::path user_dir() const
        {
            if (!cached_user_dir_.has_value())
                cached_user_dir_ = system_env_.user_dir();

            return *cached_user_dir_;
        }

    private:

        /**
         * @brief Gibt das gecachte statische Datenverzeichnis zurück.
         * @return fs::path Das gecachte statische Datenverzeichnis.
         */
        [[nodiscard]] fs::path get_static_data_dir() const
        {
            if (!cached_static_data_dir_.has_value())
                cached_static_data_dir_ = system_env_.static_data_dir(executable_dir(), app_name_);
            return *cached_static_data_dir_;
        }

        /**
         * @brief Gibt das gecachte geteilte Datenverzeichnis zurück.
         * @return fs::path Das gecachte geteilte Datenverzeichnis.
         */
        [[nodiscard]] fs::path get_shared_data_dir() const
        {
            if (!cached_shared_data_dir_.has_value())
                cached_shared_data_dir_ = system_env_.shared_data_dir(executable_dir(), app_name_);
            return *cached_shared_data_dir_;
        }

        /**
         * @brief Gibt das gecachte Benutzer-Datenverzeichnis zurück.
         * @return fs::path Das gecachte Benutzer-Datenverzeichnis.
         */
        [[nodiscard]] fs::path get_user_data_dir() const
        {
            if (!cached_user_data_dir_.has_value())
                cached_user_data_dir_ = system_env_.user_data_dir(executable_dir(), app_name_);
            return *cached_user_data_dir_;
        }

        /**
         * @brief Gibt das gecachte Konfigurationsverzeichnis zurück.
         * @return fs::path Das gecachte Konfigurationsverzeichnis.
         */
        [[nodiscard]] fs::path get_config_dir() const
        {
            if (!cached_config_dir_.has_value())
                cached_config_dir_ = system_env_.config_dir(executable_dir(), app_name_);
            return *cached_config_dir_;
        }

        /**
         * @brief Gibt das gecachte Cache-Verzeichnis zurück.
         * @return fs::path Das gecachte Cache-Verzeichnis.
         */
        [[nodiscard]] fs::path get_cache_dir() const
        {
            if (!cached_cache_dir_.has_value())
                cached_cache_dir_ = system_env_.cache_dir(executable_dir(), app_name_);
            return *cached_cache_dir_;
        }

        /**
         * @brief Gibt das gecachte Log-Verzeichnis zurück.
         * @return fs::path Das gecachte Log-Verzeichnis.
         */
        [[nodiscard]] fs::path get_log_dir() const
        {
            if (!cached_log_dir_.has_value())
                cached_log_dir_ = system_env_.log_dir(executable_dir(), app_name_);
            return *cached_log_dir_;
        }

        /**
         * @brief Gibt das gecachte temporäre Verzeichnis zurück.
         * @return fs::path Das gecachte temporäre Verzeichnis.
         */
        [[nodiscard]] fs::path get_temp_dir() const
        {
            if (!cached_temp_dir_.has_value())
                cached_temp_dir_ = system_env_.temp_dir(app_name_);
            return *cached_temp_dir_;
        }

        /** @brief Der Name der Anwendung. */
        mutable std::string app_name_;

        /** @brief Plattformspezifische Implementierung für Pfadermittlung. */
        [[no_unique_address]] SystemEnvironment system_env_;

        // Cache für die berechneten Pfade (Lazy Initialization)
        /** @brief Gecachter Pfad zur ausführbaren Datei. */
        mutable std::optional<fs::path> cached_executable_path_;
        /** @brief Gecachtes Verzeichnis der ausführbaren Datei. */
        mutable std::optional<fs::path> cached_executable_dir_;
        /** @brief Gecachtes statisches Datenverzeichnis. */
        mutable std::optional<fs::path> cached_static_data_dir_;
        /** @brief Gecachtes geteiltes Datenverzeichnis. */
        mutable std::optional<fs::path> cached_shared_data_dir_;
        /** @brief Gecachtes Benutzer-Datenverzeichnis. */
        mutable std::optional<fs::path> cached_user_data_dir_;
        /** @brief Gecachtes Konfigurationsverzeichnis. */
        mutable std::optional<fs::path> cached_config_dir_;
        /** @brief Gecachtes Cache-Verzeichnis. */
        mutable std::optional<fs::path> cached_cache_dir_;
        /** @brief Gecachtes Log-Verzeichnis. */
        mutable std::optional<fs::path> cached_log_dir_;
        /** @brief Gecachtes temporäres Verzeichnis. */
        mutable std::optional<fs::path> cached_temp_dir_;
        /** @brief Gecachtes Benutzerverzeichnis (Home). */
        mutable std::optional<fs::path> cached_user_dir_;
    };

}
