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
    env_guard::env_guard(const char* name) : name_(name)
    {
        DWORD size = GetEnvironmentVariableA(name, nullptr, 0);
        if (size > 0)
        {
            was_set_ = true;
            original_.resize(size - 1);
            GetEnvironmentVariableA(name, original_.data(), size);
        }
    }

    env_guard::~env_guard()
    {
        SetEnvironmentVariableA(name_.c_str(), was_set_ ? original_.c_str() : nullptr);
    }

    void env_guard::unset()
    {
        SetEnvironmentVariableA(name_.c_str(), nullptr);
    }
}
