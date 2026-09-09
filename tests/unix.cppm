/**
 * @file
 * @brief Unix-spezifische Umgebungsvariablen-Funktionen (Linux/macOS)
 * @author Martin Fehrs
 */

module;

#include <cstdlib>
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
}
