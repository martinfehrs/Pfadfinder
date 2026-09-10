/**
 * @file
 * @brief Unix-spezifische Umgebungsvariablen-Funktionen (Linux/macOS)
 * @author Martin Fehrs
 */

module;

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

export module platform;

namespace platform
{
    /**
     * @brief Setzt eine Umgebungsvariable unter Unix
     * @param name Name der Umgebungsvariable
     * @param value Neuer Wert
     */
    export void setenv(const std::string& name, const std::string& value)
    {
        ::setenv(name.c_str(), value.c_str(), 1);
    }

    /**
     * @brief Entfernt eine Umgebungsvariable unter Unix
     * @param name Name der Umgebungsvariable
     */
    export void unsetenv(const std::string& name)
    {
        ::unsetenv(name.c_str());
    }

    /**
     * @brief Gibt das Benutzerverzeichnis zurück
     * @return std::filesystem::path Das Benutzerverzeichnis
     * @throws std::runtime_error falls die Umgebungsvariable HOME nicht gesetzt ist
     */
    [[nodiscard]] export std::filesystem::path home_dir()
    {
        const char* home = std::getenv("HOME");

        if (!home)
            throw std::runtime_error{ "HOME environment variable not set" };

        return std::filesystem::path{ home };
    }
}
