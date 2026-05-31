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
    export struct pathfinder_error : std::runtime_error
    {
        explicit pathfinder_error(const char* message) : std::runtime_error(message) {}
    };

}
