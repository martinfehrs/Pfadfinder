/**
 * @file
 * @brief Windows-spezifische Umgebungsvariablen-Funktionen
 * @author Martin Fehrs
 */

module;

#include <filesystem>
#include <stdexcept>
#include <string>
#include <windows.h>

export module platform;

namespace platform
{
    /**
     * @brief Setzt eine Umgebungsvariable unter Windows
     * @param name Name der Umgebungsvariable
     * @param value Neuer Wert
     */
    export void setenv(const std::string& name, const std::string& value)
    {
        SetEnvironmentVariableA(name.c_str(), value.c_str());
    }

    /**
     * @brief Entfernt eine Umgebungsvariable unter Windows
     * @param name Name der Umgebungsvariable
     */
    export void unsetenv(const std::string& name)
    {
        SetEnvironmentVariableA(name.c_str(), nullptr);
    }

    /**
     * @brief Gibt das Benutzerverzeichnis zurück
     * @return std::filesystem::path Das Benutzerverzeichnis
     * @throws std::runtime_error falls die Umgebungsvariable USERPROFILE nicht gesetzt ist
     */
    [[nodiscard]] export std::filesystem::path home_dir()
    {
        const char* home = std::getenv("USERPROFILE");

        if (!home)
            throw std::runtime_error{ "USERPROFILE environment variable not set" };

        return std::filesystem::path{ home };
    }
}
