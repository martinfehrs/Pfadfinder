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
 *    - log_directory()      : Log-Verzeichnis für Anwendungsprotokolle
 *    - temp_directory()      : Temporäres Verzeichnis für die Anwendung
 *    - user_directory()      : Home-Verzeichnis des Benutzers
 * 
 * 2. Fehlerbehandlung:
 *    - enum class error      : Fehlertypen für alle Pfadfunktionen (snake_case)
 *    - error_message()       : Menschlesbare Fehlermeldungen für error-Werte
 * 
 * Alle Pfadfunktionen geben std::expected<fs::path, error> zurück.
 */

module;

#include "config.hpp"

#if IS_WINDOWS
#include <windows.h>
#elif IS_MACOSX
#include <mach-o/dyld.h>
#include <limits.h>
#elif IS_LINUX
#include <unistd.h>
#include <limits.h>
#endif

#include <cstdlib>
#include <expected>
#include <filesystem>
#include <string>

export module pfadfinder;

namespace fs = std::filesystem;

namespace pfadfinder
{

    /**
     * @brief Fehlertypen für Pfadfinder-Operationen.
     */
    export enum class error
    {
        // Plattform-spezifische Fehler
        platform_not_supported,

        // Windows-spezifisch
        windows_get_module_file_name_failed,
        appdata_not_set,
        localappdata_not_set,

        // macOS-spezifisch
        macos_get_executable_path_failed,
        macos_realpath_failed,

        // Linux-spezifisch
        linux_readlink_failed,

        // Umgebungsvariablen
        home_not_set
    };

    /**
     * @brief Gibt eine menschenlesbare Fehlermeldung für einen Fehlercode zurück.
     * @param err Der Fehlercode.
     * @return const char* Die Fehlermeldung.
     */
    export const char* error_message(error err) noexcept
    {
        switch (err)
        {
            case error::platform_not_supported:
                return "Platform not supported";
            case error::windows_get_module_file_name_failed:
                return "GetModuleFileNameW failed";
            case error::appdata_not_set:
                return "APPDATA environment variable not set";
            case error::localappdata_not_set:
                return "LOCALAPPDATA environment variable not set";
            case error::macos_get_executable_path_failed:
                return "_NSGetExecutablePath failed";
            case error::macos_realpath_failed:
                return "realpath failed";
            case error::linux_readlink_failed:
                return "readlink /proc/self/exe failed";
            case error::home_not_set:
                return "HOME environment variable not set";
        }
        return "Unknown error";
    }

    /**
     * @brief Stellt Methoden zur Bestimmung verschiedener Verzeichnisse einer Anwendung bereit.
     * 
     * Diese Klasse kapselt die Logik zur Ermittlung von Pfaden wie dem Executable-Pfad,
     * Datenverzeichnis, Konfigurationsverzeichnis und Cache-Verzeichnis für verschiedene
     * Plattformen (Windows, Linux, macOS).
     * 
     * Alle Methoden geben std::expected<fs::path, error> zurück,
     * wobei error eine typsichere Aufzählung ist, die die möglichen Fehlerfälle
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
         * @return std::expected<fs::path, error> Der vollständige Pfad zur
         *         ausführbaren Datei oder ein Fehlercode.
         */
        std::expected<fs::path, error> executable_path() const
        {
            if (!cached_executable_path_.has_value())
            {
#if IS_WINDOWS
                // Windows-Implementierung
                wchar_t path[MAX_PATH] = {0};
                if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
                    cached_executable_path_ = std::unexpected(error::windows_get_module_file_name_failed);
                else
                    cached_executable_path_ = fs::path(path);

#elif IS_MACOSX
                // macOS-Implementierung
                char path[PATH_MAX] = {0};
                uint32_t size = sizeof(path);
                if (_NSGetExecutablePath(path, &size) != 0)
                    cached_executable_path_ = std::unexpected(error::macos_get_executable_path_failed);
                else
                {
                    // Symbolische Links auflösen, um den tatsächlichen Pfad zu erhalten
                    char real_path[PATH_MAX] = {0};
                    if (realpath(path, real_path) == nullptr)
                        cached_executable_path_ = std::unexpected(error::macos_realpath_failed);
                    else
                        cached_executable_path_ = fs::path(real_path);
                }

#elif IS_LINUX
                // Linux-Implementierung
                char path[PATH_MAX] = {0};
                ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
                if (len == -1)
                    cached_executable_path_ = std::unexpected(error::linux_readlink_failed);
                else
                {
                    path[len] = '\0';
                    cached_executable_path_ = fs::path(path);
                }

#else
                // Fallback für andere Plattformen
                cached_executable_path_ = std::unexpected(error::platform_not_supported);
#endif
            }
            return *cached_executable_path_;
        }

        /**
         * @brief Gibt das Verzeichnis der ausführbaren Datei zurück.
         * @return std::expected<fs::path, error> Das Verzeichnis, das
         *         die ausführbare Datei enthält oder ein Fehlercode.
         */
        std::expected<fs::path, error> executable_directory() const
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
         * @return std::expected<fs::path, error> Das Datenverzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error> data_directory() const
        {
            auto exe_dir_result = executable_directory();
            if (!exe_dir_result)
                return std::unexpected(exe_dir_result.error());
            auto exe_dir = *exe_dir_result;

#if IS_WINDOWS
            // Windows: Datenverzeichnis ist das Binärverzeichnis
            return exe_dir / app_name_;

#elif IS_MACOSX
            // macOS: Prüfen, ob wir in einem Bundle sind
            std::string exe_dir_str = exe_dir.string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                // Bundle: von .../Contents/MacOS/ zu .../Contents/Resources/
                return exe_dir.parent_path().parent_path() / "Resources" / app_name_;
            else
                // Nicht gebündelt: ähnlich wie Linux
                return exe_dir.parent_path() / "share" / app_name_;

#elif IS_LINUX
            // Linux: von /usr/bin/myapp zu /usr/share/myapp
            return exe_dir.parent_path() / "share" / app_name_;

#else
            // Fallback für andere Plattformen
            return std::unexpected(error::platform_not_supported);
#endif
        }

        /**
         * @brief Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %APPDATA%/<appname>.
         * Unter Linux entspricht dies ~/.local/share/<appname>.
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Application Support/<appname>, ansonsten ~/.local/share/<appname>.
         * 
         * @return std::expected<fs::path, error> Das Benutzer-Datenverzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error> user_data_directory() const
        {
            if (!cached_user_data_directory_.has_value())
            {
#if IS_WINDOWS
                // Windows: %APPDATA%/<appname>
                const char* appdata = std::getenv("APPDATA");
                if (!appdata)
                    cached_user_data_directory_ = std::unexpected(error::appdata_not_set);
                else
                    cached_user_data_directory_ = fs::path(appdata) / app_name_;

#elif IS_MACOSX
                // macOS: Prüfen, ob wir in einem Bundle sind
                auto exe_dir_result = executable_directory();
                if (!exe_dir_result)
                    cached_user_data_directory_ = std::unexpected(exe_dir_result.error());
                else
                {
                    std::string exe_dir_str = exe_dir_result->string();
                    if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                    {
                        // Bundle: ~/Library/Application Support/<appname>
                        const char* home = std::getenv("HOME");
                        if (!home)
                            cached_user_data_directory_ = std::unexpected(error::home_not_set);
                        else
                            cached_user_data_directory_ =
                                fs::path(home) / "Library" / "Application Support" / app_name_;
                    }
                    else
                    {
                        // Nicht gebündelt: ~/.local/share/<appname>
                        const char* home = std::getenv("HOME");
                        if (!home)
                            cached_user_data_directory_ = std::unexpected(error::home_not_set);
                        else
                            cached_user_data_directory_ = fs::path(home) / ".local" / "share" / app_name_;
                    }
                }

#elif IS_LINUX
                // Linux: ~/.local/share/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    cached_user_data_directory_ = std::unexpected(error::home_not_set);
                else
                    cached_user_data_directory_ = fs::path(home) / ".local" / "share" / app_name_;

#else
                // Fallback für andere Plattformen
                cached_user_data_directory_ = std::unexpected(error::platform_not_supported);
#endif
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
         * @return std::expected<fs::path, error> Das Konfigurationsverzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error> config_directory() const
        {
#if IS_WINDOWS
            // Windows: %APPDATA%/<appname>
            const char* appdata = std::getenv("APPDATA");
            if (!appdata)
                return std::unexpected(error::appdata_not_set);
            return fs::path(appdata) / app_name_;

#elif IS_MACOSX
            // macOS: Prüfen, ob wir in einem Bundle sind
            auto exe_dir_result = executable_directory();
            if (!exe_dir_result)
                return std::unexpected(exe_dir_result.error());
            std::string exe_dir_str = exe_dir_result->string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Preferences/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    return std::unexpected(error::home_not_set);
                return fs::path(home) / "Library" / "Preferences" / app_name_;
            }
            else
            {
                // Nicht gebündelt: ~/.config/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    return std::unexpected(error::home_not_set);
                return fs::path(home) / ".config" / app_name_;
            }

#elif IS_LINUX
            // Linux: ~/.config/<appname> (XDG Base Directory Specification)
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error::home_not_set);
            return fs::path(home) / ".config" / app_name_;

#else
            // Fallback für andere Plattformen
            return std::unexpected(error::platform_not_supported);
#endif
        }

        /**
         * @brief Gibt das Cache-Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %LOCALAPPDATA%/<appname>/Cache.
         * Unter Linux entspricht dies ~/.cache/<appname> (XDG-Standard).
         * Unter macOS entspricht dies bei gebündelten Anwendungen
         * ~/Library/Caches/<appname>, ansonsten ~/.cache/<appname>.
         * 
         * @return std::expected<fs::path, error> Das Cache-Verzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error> cache_directory() const
        {
            if (!cached_cache_directory_.has_value())
            {
#if IS_WINDOWS
                // Windows: %LOCALAPPDATA%/<appname>/Cache
                const char* localappdata = std::getenv("LOCALAPPDATA");
                if (!localappdata)
                    cached_cache_directory_ = std::unexpected(error::localappdata_not_set);
                else
                    cached_cache_directory_ = fs::path(localappdata) / app_name_ / "Cache";

#elif IS_MACOSX
                // macOS: Prüfen, ob wir in einem Bundle sind
                auto exe_dir_result = executable_directory();
                if (!exe_dir_result)
                    cached_cache_directory_ = std::unexpected(exe_dir_result.error());
                else
                {
                    std::string exe_dir_str = exe_dir_result->string();
                    if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
                    {
                        // Bundle: ~/Library/Caches/<appname>
                        const char* home = std::getenv("HOME");
                        if (!home)
                            cached_cache_directory_ = std::unexpected(error::home_not_set);
                        else
                            cached_cache_directory_ = fs::path(home) / "Library" / "Caches" / app_name_;
                    }
                    else
                    {
                        // Nicht gebündelt: ~/.cache/<appname>
                        const char* home = std::getenv("HOME");
                        if (!home)
                            cached_cache_directory_ = std::unexpected(error::home_not_set);
                        else
                            cached_cache_directory_ = fs::path(home) / ".cache" / app_name_;
                    }
                }

#elif IS_LINUX
                // Linux: ~/.cache/<appname> (XDG Base Directory Specification)
                const char* home = std::getenv("HOME");
                if (!home)
                    cached_cache_directory_ = std::unexpected(error::home_not_set);
                else
                    cached_cache_directory_ = fs::path(home) / ".cache" / app_name_;

#else
                // Fallback für andere Plattformen
                cached_cache_directory_ = std::unexpected(error::platform_not_supported);
#endif
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
         * @return std::expected<fs::path, error> Das Log-Verzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error> log_directory() const
        {
#if IS_WINDOWS
            // Windows: %LOCALAPPDATA%/<appname>/Logs
            const char* localappdata = std::getenv("LOCALAPPDATA");
            if (!localappdata)
                return std::unexpected(error::localappdata_not_set);
            return fs::path(localappdata) / app_name_ / "Logs";

#elif IS_MACOSX
            // macOS: Prüfen, ob wir in einem Bundle sind
            auto exe_dir_result = executable_directory();
            if (!exe_dir_result)
                return std::unexpected(exe_dir_result.error());
            std::string exe_dir_str = exe_dir_result->string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Logs/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    return std::unexpected(error::home_not_set);
                return fs::path(home) / "Library" / "Logs" / app_name_;
            }
            else
            {
                // Nicht gebündelt: ~/.local/state/<appname>/log (XDG-konform)
                const char* home = std::getenv("HOME");
                if (!home)
                    return std::unexpected(error::home_not_set);
                return fs::path(home) / ".local" / "state" / app_name_ / "log";
            }

#elif IS_LINUX
            // Linux: ~/.local/state/<appname>/log (XDG Base Directory Specification)
            const char* home = std::getenv("HOME");
            if (!home)
                return std::unexpected(error::home_not_set);
            return fs::path(home) / ".local" / "state" / app_name_ / "log";

#else
            // Fallback für andere Plattformen
            return std::unexpected(error::platform_not_supported);
#endif
        }

        /**
         * @brief Gibt das temporäre Verzeichnis der Anwendung zurück.
         * 
         * Unter Windows entspricht dies %TEMP%/<appname>.
         * Unter Linux entspricht dies /tmp/<appname> oder dem systemweiten Temp-Verzeichnis.
         * Unter macOS entspricht dies /tmp/<appname> oder ~/Library/Caches/TemporaryItems/<appname>.
         * 
         * @return std::expected<fs::path, error> Das temporäre Verzeichnis
         *         der Anwendung oder ein Fehlercode.
         */
        std::expected<fs::path, error> temp_directory() const
        {
#if IS_WINDOWS
            // Windows: %TEMP%/<appname>
            const char* temp = std::getenv("TEMP");
            if (!temp)
                return std::unexpected(error::home_not_set); // Fallback: Home nicht gesetzt
            return fs::path(temp) / app_name_;

#elif IS_MACOSX
            // macOS: Prüfen, ob wir in einem Bundle sind
            auto exe_dir_result = executable_directory();
            if (!exe_dir_result)
                return std::unexpected(exe_dir_result.error());
            std::string exe_dir_str = exe_dir_result->string();
            if (exe_dir_str.find("Contents/MacOS") != std::string::npos)
            {
                // Bundle: ~/Library/Caches/TemporaryItems/<appname>
                const char* home = std::getenv("HOME");
                if (!home)
                    return std::unexpected(error::home_not_set);
                return fs::path(home) / "Library" / "Caches" / "TemporaryItems" / app_name_;
            }
            else
                // Nicht gebündelt: /tmp/<appname>
                return fs::temp_directory_path() / app_name_;

#elif IS_LINUX
            // Linux: /tmp/<appname> (XDG Base Directory Specification)
            return fs::temp_directory_path() / app_name_;

#else
            // Fallback für andere Plattformen
            return std::unexpected(error::platform_not_supported);
#endif
        }

        /**
         * @brief Gibt das Home-Verzeichnis des Benutzers zurück.
         * 
         * Unter Windows entspricht dies %USERPROFILE%.
         * Unter Linux und macOS entspricht dies $HOME.
         * 
         * @return std::expected<fs::path, error> Das Home-Verzeichnis
         *         des Benutzers oder ein Fehlercode.
         */
        std::expected<fs::path, error> user_directory() const
        {
            if (!cached_user_directory_.has_value())
            {
#if IS_WINDOWS
                // Windows: %USERPROFILE%
                const char* userprofile = std::getenv("USERPROFILE");
                if (!userprofile)
                    cached_user_directory_ = std::unexpected(error::home_not_set);
                else
                    cached_user_directory_ = fs::path(userprofile);

#elif IS_MACOSX || defined(__linux__)
                // Linux/macOS: $HOME
                const char* home = std::getenv("HOME");
                if (!home)
                    cached_user_directory_ = std::unexpected(error::home_not_set);
                else
                    cached_user_directory_ = fs::path(home);

#else
                // Fallback für andere Plattformen
                cached_user_directory_ = std::unexpected(error::platform_not_supported);
#endif
            }
            return *cached_user_directory_;
        }

    private:
        std::string app_name_;

        // Cache für die berechneten Pfade (Lazy Initialization)
        mutable std::optional<std::expected<fs::path, error>> cached_executable_path_;
        mutable std::optional<std::expected<fs::path, error>> cached_executable_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_data_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_user_data_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_config_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_cache_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_log_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_temp_directory_;
        mutable std::optional<std::expected<fs::path, error>> cached_user_directory_;
    };

}
