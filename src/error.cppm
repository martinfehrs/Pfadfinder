/**
 * @file
 * @brief Fehlerbehandlung (pfadfinder:error)
 * @author Martin Fehrs
 */

module;

#include <stdexcept>
#include <string>
#include <format>

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

        explicit error(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn eine Datei nicht gefunden wurde.
     */
    export struct file_not_found : error
    {
        explicit file_not_found(const std::string& path) : error(std::format("File not found: {}", path)) {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn ein Verzeichnis nicht gefunden wurde.
     */
    export struct directory_not_found : error
    {
        explicit directory_not_found(const std::string& path) : error(std::format("Directory not found: {}", path)) {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn der Pfad zur ausführbaren Datei nicht ermittelt werden kann.
     */
    export struct indeterminable_exe_path : error
    {
        indeterminable_exe_path() : error("Indeterminable exe path") {}
    };

    /**
     * @brief Ausnahme, die geworfen wird, wenn eine Umgebungsvariable nicht gesetzt ist.
     */
    export struct environment_variable_not_set : error
    {
        explicit environment_variable_not_set(const std::string& var_name) : error(std::format("Environment variable not set: {}", var_name)) {}
    };

}
