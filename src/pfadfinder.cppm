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
 *    - user_config_dir()      : Benutzer-spezifisches Konfigurationsverzeichnis (mit create_dir Parameter)
 *    - user_cache_dir()       : Benutzer-spezifisches Cache-Verzeichnis (mit create_dir Parameter)
 *    - user_log_dir()         : Benutzer-spezifisches Log-Verzeichnis für Anwendungsprotokolle (mit create_dir Parameter)
 *    - temp_dir()             : Temporäres Verzeichnis für die Anwendung (mit create_dir Parameter)
 *    - user_dir()             : Home-Verzeichnis des Benutzers
 */

module;

#include <filesystem>
#include <string>
#include <optional>
#include <utility>

#include "config.hpp"

export module pfadfinder;

#if IS_WINDOWS
export import :windows_backend;
#elif IS_MACOS
export import :macos_backend;
#elif IS_LINUX
export import :linux_backend;
#endif

export import :error;
export import :system_environment;

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
         * Unter Windows entspricht dies %ALLUSERSAPPDATA%/<appname>lt;appname<appname>gt;.
         * Unter Linux entspricht dies /var/lib/<appname>lt;appname<appname>gt;.
         * Unter macOS entspricht dies /Library/Application Support/<appname>lt;appname<appname>gt;.
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
         * @brief Gibt das geteilte Cache-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %ALLUSERSAPPDATA%/<appname>lt;appname<appname>gt;/Cache.
         * Unter Linux entspricht dies /var/cache/<appname>lt;appname<appname>gt;.
         * Unter macOS entspricht dies /Library/Caches/<appname>lt;appname<appname>gt;.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis (optional).
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das geteilte Cache-Verzeichnis der Anwendung (Basis oder Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path shared_cache_dir(const fs::path& rel_path = "", bool create_dir = true) const
        {
            auto path = get_shared_cache_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das geteilte Log-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %ALLUSERSAPPDATA%/<appname>lt;appname<appname>gt;/Logs.
         * Unter Linux entspricht dies /var/log/<appname>lt;appname<appname>gt;.
         * Unter macOS entspricht dies /Library/Logs/<appname>lt;appname<appname>gt;.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis (optional).
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das geteilte Log-Verzeichnis der Anwendung (Basis oder Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path shared_log_dir(const fs::path& rel_path = "", bool create_dir = true) const
        {
            auto path = get_shared_log_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das geteilte Konfigurationsverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %ALLUSERSAPPDATA%/<appname>lt;appname<appname>gt;.
         * Unter Linux entspricht dies /etc/<appname>lt;appname<appname>gt;.
         * Unter macOS entspricht dies /Library/Preferences/<appname>lt;appname<appname>gt;.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis (optional).
         * @return fs::path Das geteilte Konfigurationsverzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert.
         * 
         * @note Im Gegensatz zu den meisten anderen Methoden hat diese Methode keine create_dir-Option,
         * da das Linux-Backend nur Lesezugriff auf /etc erlaubt (kleinster gemeinsamer Nenner).
         */
        [[nodiscard]] fs::path shared_config_dir(const fs::path& rel_path = "") const
        {
            auto path = get_shared_config_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>lt;appname<appname>gt;.
         * Unter Linux entspricht dies ~/.local/share/<appname>lt;appname<appname>gt;.
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Application Support/<appname>lt;appname<appname>gt;, ansonsten ~/.local/share/<appname>lt;appname<appname>gt;.
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
         * Unter Windows entspricht dies %APPDATA%/<appname>lt;appname<appname>gt;/<rel_path>.
         * Unter Linux entspricht dies ~/.local/share/<appname>lt;appname<appname>gt;/<rel_path>.
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Application Support/<appname>lt;appname<appname>gt;/<rel_path>, ansonsten ~/.local/share/<appname>lt;appname<appname>gt;/<rel_path>.
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
         * @brief Gibt das Benutzer-spezifische Konfigurationsverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>lt;appname<appname>gt;.
         * Unter Linux entspricht dies ~/.config/<appname>lt;appname<appname>gt; (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Preferences/<appname>lt;appname<appname>gt;, ansonsten ~/.config/<appname>lt;appname<appname>gt;.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-spezifische Konfigurationsverzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_config_dir(bool create_dir = true) const
        {
            auto path = get_user_config_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Konfigurationsverzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>lt;appname<appname>gt;/<rel_path>.
         * Unter Linux entspricht dies ~/.config/<appname>lt;appname<appname>gt;/<rel_path> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Preferences/<appname>lt;appname<appname>gt;/<rel_path>, ansonsten ~/.config/<appname>lt;appname<appname>gt;/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-spezifische Konfigurationsverzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_config_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_user_config_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Cache-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>lt;appname<appname>gt;/Cache.
         * Unter Linux entspricht dies ~/.cache/<appname>lt;appname<appname>gt; (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Caches/<appname>lt;appname<appname>gt;, ansonsten ~/.cache/<appname>lt;appname<appname>gt;.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-spezifische Cache-Verzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_cache_dir(bool create_dir = true) const
        {
            auto path = get_user_cache_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Cache-Verzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>lt;appname<appname>gt;/Cache/<rel_path>.
         * Unter Linux entspricht dies ~/.cache/<appname>lt;appname<appname>gt;/<rel_path> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Caches/<appname>lt;appname<appname>gt;/<rel_path>, ansonsten ~/.cache/<appname>lt;appname<appname>gt;/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-spezifische Cache-Verzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_cache_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_user_cache_dir();
            if (!rel_path.empty())
                path /= rel_path;
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Log-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>lt;appname<appname>gt;/Logs.
         * Unter Linux entspricht dies ~/.local/state/<appname>lt;appname<appname>gt;/log (XDG Base Directory Specification).
         * Unter macOS (Bundle) entspricht dies ~/Library/Logs/<appname>lt;appname<appname>gt;.
         * Unter macOS (CLI) entspricht dies ~/.local/state/<appname>lt;appname<appname>gt;/log.
         * 
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-spezifische Log-Verzeichnis der Anwendung.
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_log_dir(bool create_dir = true) const
        {
            auto path = get_user_log_dir();
            if (create_dir)
                fs::create_directories(path);
            else if (!fs::exists(path) || !fs::is_directory(path))
                throw directory_not_found(path.string());
            return path;
        }

        /**
         * @brief Gibt das Benutzer-spezifische Log-Verzeichnis mit optionalem Unterpfad zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>lt;appname<appname>gt;/Logs/<rel_path>.
         * Unter Linux entspricht dies ~/.local/state/<appname>lt;appname<appname>gt;/log/<rel_path> (XDG Base Directory Specification).
         * Unter macOS (Bundle) entspricht dies ~/Library/Logs/<appname>lt;appname<appname>gt;/<rel_path>.
         * Unter macOS (CLI) entspricht dies ~/.local/state/<appname>lt;appname<appname>gt;/log/<rel_path>.
         * 
         * @param rel_path Relativer Pfad zum Basisverzeichnis.
         * @param create_dir Legt fest, ob das Verzeichnis erstellt werden soll, falls es nicht existiert (optional, Standardwert: true).
         * @return fs::path Das Benutzer-spezifische Log-Verzeichnis der Anwendung (Basis + rel_path).
         * @throws directory_not_found Wenn das Verzeichnis nicht existiert und create_dir false ist.
         */
        [[nodiscard]] fs::path user_log_dir(const fs::path& rel_path, bool create_dir = true) const
        {
            auto path = get_user_log_dir();
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
         * Unter Windows entspricht dies %TEMP%/<appname>lt;appname<appname>gt;.
         * Unter Linux entspricht dies /tmp/<appname>lt;appname<appname>gt; oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname>lt;appname<appname>gt; oder ~/Library/Caches/TemporaryItems/<appname>lt;appname<appname>gt;.
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
         * Unter Windows entspricht dies %TEMP%/<appname>lt;appname<appname>gt;/<rel_path>.
         * Unter Linux entspricht dies /tmp/<appname>lt;appname<appname>gt;/<rel_path> oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname>lt;appname<appname>gt;/<rel_path> oder ~/Library/Caches/TemporaryItems/<appname>lt;appname<appname>gt;/<rel_path>.
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
         * @brief Gibt das gecachte Benutzer-spezifische Konfigurationsverzeichnis zurück.
         * @return fs::path Das gecachte Benutzer-spezifische Konfigurationsverzeichnis.
         */
        [[nodiscard]] fs::path get_user_config_dir() const
        {
            if (!cached_user_config_dir_.has_value())
                cached_user_config_dir_ = system_env_.user_config_dir(executable_dir(), app_name_);
            return *cached_user_config_dir_;
        }

        /**
         * @brief Gibt das gecachte Benutzer-spezifische Cache-Verzeichnis zurück.
         * @return fs::path Das gecachte Benutzer-spezifische Cache-Verzeichnis.
         */
        [[nodiscard]] fs::path get_user_cache_dir() const
        {
            if (!cached_user_cache_dir_.has_value())
                cached_user_cache_dir_ = system_env_.user_cache_dir(executable_dir(), app_name_);
            return *cached_user_cache_dir_;
        }

        /**
         * @brief Gibt das gecachte Benutzer-spezifische Log-Verzeichnis zurück.
         * @return fs::path Das gecachte Benutzer-spezifische Log-Verzeichnis.
         */
        [[nodiscard]] fs::path get_user_log_dir() const
        {
            if (!cached_user_log_dir_.has_value())
                cached_user_log_dir_ = system_env_.user_log_dir(executable_dir(), app_name_);
            return *cached_user_log_dir_;
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

        /**
         * @brief Gibt das gecachte geteilte Cache-Verzeichnis zurück.
         * @return fs::path Das gecachte geteilte Cache-Verzeichnis.
         */
        [[nodiscard]] fs::path get_shared_cache_dir() const
        {
            if (!cached_shared_cache_dir_.has_value())
                cached_shared_cache_dir_ = system_env_.shared_cache_dir(app_name_);
            return *cached_shared_cache_dir_;
        }

        /**
         * @brief Gibt das gecachte geteilte Log-Verzeichnis zurück.
         * @return fs::path Das gecachte geteilte Log-Verzeichnis.
         */
        [[nodiscard]] fs::path get_shared_log_dir() const
        {
            if (!cached_shared_log_dir_.has_value())
                cached_shared_log_dir_ = system_env_.shared_log_dir(app_name_);
            return *cached_shared_log_dir_;
        }

        /**
         * @brief Gibt das gecachte geteilte Konfigurationsverzeichnis zurück.
         * @return fs::path Das gecachte geteilte Konfigurationsverzeichnis.
         */
        [[nodiscard]] fs::path get_shared_config_dir() const
        {
            if (!cached_shared_config_dir_.has_value())
                cached_shared_config_dir_ = system_env_.shared_config_dir(app_name_);
            return *cached_shared_config_dir_;
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
        /** @brief Gecachtes Benutzer-spezifische Konfigurationsverzeichnis. */
        mutable std::optional<fs::path> cached_user_config_dir_;
        /** @brief Gecachtes Benutzer-spezifische Cache-Verzeichnis. */
        mutable std::optional<fs::path> cached_user_cache_dir_;
        /** @brief Gecachtes Benutzer-spezifische Log-Verzeichnis. */
        mutable std::optional<fs::path> cached_user_log_dir_;
        /** @brief Gecachtes temporäres Verzeichnis. */
        mutable std::optional<fs::path> cached_temp_dir_;
        /** @brief Gecachtes Benutzerverzeichnis (Home). */
        mutable std::optional<fs::path> cached_user_dir_;
        /** @brief Gecachtes geteiltes Cache-Verzeichnis. */
        mutable std::optional<fs::path> cached_shared_cache_dir_;
        /** @brief Gecachtes geteiltes Log-Verzeichnis. */
        mutable std::optional<fs::path> cached_shared_log_dir_;
        /** @brief Gecachtes geteiltes Konfigurationsverzeichnis. */
        mutable std::optional<fs::path> cached_shared_config_dir_;
    };

}
