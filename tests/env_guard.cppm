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

        void unset();

    private:
        std::string name_;
        std::string original_;
        bool was_set_ = false;
    };
}
