/**
 * @file scoped_environment.cppm
 * @brief Modul mit RAII-Klasse zum temporären Umleiten von Umgebungsvariablen für Tests.
 * @author Martin Fehrs
 */

module;

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

export module scoped_environment;

/**
 * @brief RAII-Klasse zum temporären Ändern von Umgebungsvariablen.
 *
 * Diese Klasse ermittelt beim Erstellen den aktuellen Wert einer oder mehrerer
 * Umgebungsvariablen, ändert diese und stellt die ursprünglichen Werte beim
 * Zerstören wieder her. Dies ermöglicht sichere Integrationstests, die
 * Umgebungsvariablen umleiten, ohne das System zu beeinträchtigen.
 *
 * @note Nicht thread-sicher. Jede Instanz sollte in genau einem Thread verwendet werden.
 * @note Die Änderungen gelten für den gesamten Prozess (nicht nur den Thread).
 */
export class scoped_environment
{

public:

    /**
     * @brief Leitet eine Umgebungsvariable temporär um.
     * @param name Name der Umgebungsvariable.
     * @param value Neuer Wert für die Variable.
     */
    explicit scoped_environment(const std::string& name, const std::string& value)
    {
        // aktuellen Wert speichern
        const char* old_value = std::getenv(name.c_str());
        original_values_.emplace_back(name, old_value);
        
        // neuen Wert setzen
#if defined(_WIN32)
        SetEnvironmentVariableA(name.c_str(), value.c_str());
#else
        setenv(name.c_str(), value.c_str(), 1);
#endif
    }

    /**
     * @brief Stellt die ursprünglichen Umgebungsvariablen wieder her.
     */
    ~scoped_environment() noexcept
    {
        for (auto it = original_values_.rbegin(); it != original_values_.rend(); ++it)
        {
            const auto& [name, old_value] = *it;
            if (old_value)
            {
#if defined(_WIN32)
                SetEnvironmentVariableA(name.c_str(), old_value);
#else
                setenv(name.c_str(), old_value, 1);
#endif
            }
            else
            {
#if defined(_WIN32)
                SetEnvironmentVariableA(name.c_str(), nullptr);
#else
                unsetenv(name.c_str());
#endif
            }
        }
    }

    scoped_environment(const scoped_environment&) = delete;
    scoped_environment& operator=(const scoped_environment&) = delete;

private:

    std::vector<std::pair<std::string, const char*>> original_values_;

};
