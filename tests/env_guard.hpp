/**
 * @file env_guard.hpp
 * @brief RAII-Hilfsklasse zum Sichern und Wiederherstellen von Umgebungsvariablen in Tests
 * @author Martin Fehrs
 */

#ifndef PFADFINDER_TESTS_ENV_GUARD_HPP
#define PFADFINDER_TESTS_ENV_GUARD_HPP

#include <string>

namespace pfadfinder::test
{
    class env_guard
    {
    public:
        explicit env_guard(const char* name);
        ~env_guard();

        void unset();

    private:
        std::string name_;
        std::string original_;
        bool was_set_ = false;
    };
}

#endif
