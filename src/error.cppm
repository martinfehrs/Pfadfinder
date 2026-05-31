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
    export struct error : std::runtime_error
    {
        explicit error(const char* message) : std::runtime_error(message) {}
    };

}
