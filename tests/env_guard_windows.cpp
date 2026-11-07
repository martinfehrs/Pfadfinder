/**
 * @file env_guard_windows.cpp
 * @brief Windows-Implementierung von env_guard
 * @author Martin Fehrs
 */

module;

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

module env_guard;

namespace pfadfinder::test
{
    env_guard::env_guard(const char* name) : name(name)
    {
        DWORD size = GetEnvironmentVariableA(name, nullptr, 0);
        if (size > 0)
        {
            was_set = true;
            original.resize(size - 1);
            GetEnvironmentVariableA(name, original.data(), size);
        }
    }

    env_guard::~env_guard()
    {
        SetEnvironmentVariableA(name.c_str(), was_set ? original.c_str() : nullptr);
    }

    void env_guard::unset()
    {
        SetEnvironmentVariableA(name.c_str(), nullptr);
    }
}
