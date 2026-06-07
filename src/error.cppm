/**
 * @file
 * @brief Fehlerbehandlung (pfadfinder:error)
 * @author Martin Fehrs
 */

module;

#include <stdexcept>
#include <string>

export module pfadfinder:error;

namespace pfadfinder
{
    /**
     * @brief Basisklasse für alle pfadfinder-spezifischen Ausnahmen.
     * 
     * Erbt von std::runtime_error und dient als Basis für alle
     * Ausnahmetypen in diesem Modul.
     */
    export struct error : std::runtime_error
    {
        explicit error(const char* message) : std::runtime_error(message) {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn eine Datei nicht gefunden wurde.
     */
    export struct file_not_found : error
    {
        explicit file_not_found(const std::string& path) : error(("File not found: " + path).c_str()) {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn ein Verzeichnis nicht gefunden wurde.
     */
    export struct directory_not_found : error
    {
        explicit directory_not_found(const std::string& path) : error(("Directory not found: " + path).c_str()) {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn das Home-Verzeichnis nicht gesetzt ist.
     */
    export struct home_not_set : error
    {
        home_not_set() : error("Home directory not set") {}
    };

}
