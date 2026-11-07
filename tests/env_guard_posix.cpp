/**
 * @file env_guard_posix.cpp
 * @brief POSIX-Implementierung von env_guard (Linux und macOS)
 * @author Martin Fehrs
 */

module;

#include <cstdlib>

#include <unistd.h>

module env_guard;

namespace pfadfinder::test
{
    env_guard::env_guard(const char* name) : name(name)
    {
        if (const char* val = std::getenv(name))
        {
            was_set = true;
            original = val;
        }
    }

    env_guard::~env_guard()
    {
        if (was_set)
            setenv(name.c_str(), original.c_str(), 1);
        else
            unsetenv(name.c_str());
    }

    void env_guard::unset()
    {
        unsetenv(name.c_str());
    }
}
