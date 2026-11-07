/**
 * @file env_guard.cppm
 * @brief RAII-Hilfsklasse zum Sichern und Wiederherstellen von Umgebungsvariablen in Tests
 * @author Martin Fehrs
 */

module;

#include <string>

export module env_guard;

export namespace pfadfinder::test
{
    class env_guard
    {
    public:
        explicit env_guard(const char* name);
        ~env_guard();

        env_guard(const env_guard&) = delete;
        env_guard& operator=(const env_guard&) = delete;
        env_guard(env_guard&&) = delete;
        env_guard& operator=(env_guard&&) = delete;

        void unset();

    private:
        std::string name;
        std::string original;
        bool was_set = false;
    };
}
