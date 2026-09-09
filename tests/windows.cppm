/**
 * @file
 * @brief Windows-spezifische Umgebungsvariablen-Funktionen
 * @author Martin Fehrs
 */

module;

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
}
