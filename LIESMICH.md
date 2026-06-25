# Pfadfinder

Projekt zur plattformübergreifenden Bestimmung gebräuchlicher Anwendungsverzeichnisse.

## Beschreibung

Mittels einer Instanz der Klasse `pfadfinder::application_environment` erhält man
Zugriff auf folgende Betriebssystemabhängigen Pfade und deren Unterpfade:

- Benutzerspezifische Konfiguration
- Benutzerspezifische Cache
- Benutzerspezifische Daten
- Benutzerübergreifende Konfiguration
- Benuterübergreifender Cache
- Benutzerübergreifende Daten
- Statsiche Anwendungsdaten
- Benutzerverzeichnis
- Temporäre Daten

## Unterstützte Plattformen

- Windows
- macOS
- Linux

## Voraussetzungen

- C++23
- CMake 3.28 oder höher
- Compiler mit C++-Modul-Unterstützung (GCC 15, Clang 19, MSVC 19.40+)

## Erstellung

### Unter MacOS und Linux:

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G "Ninja Multi-Config" ..
cmake --build . --config Release
```

### Unter Windows mit Visual Studio 2022:

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## Verwendungsbeispiele

### Ohne Anpassungen

```cpp
import std;
import pfadfinder;

int main() try
{
    pfadfinder::application_environment env{};

    std::println("Executable:    {}", env.executable_path().string());
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.config_dir.string());
    std::println("Cache Dir:     {}", env.cache_dir.string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Fehler: {}", e.what());
    return 1;    
}
```

### Mit benutzerdefiniertem Anwendungsnamen

```cpp
import std;
import pfadfinder;

int main() try
{
    pfadfinder::application_environment env{ "my_app" };

    std::println("Executable:    {}", env.executable_path().string());
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.config_dir.string());
    std::println("Cache Dir:     {}", env.cache_dir.string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Fehler: {}", e.what());
    return 1;    
}
```

### Mit angepasster Systemumgebung

```cpp
import std;
import pfadfinder;

namespace fs = std::filesystem;

struct my_custom_environment : pfadfinder::default_system_environment
{
    [[nodiscard]] fs::path config_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }

    [[nodiscard]] fs::path user_data_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }

    [[nodiscard]] fs::path cache_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }
};

int main() try
{
    pfadfinder::application_environment env{ my_custom_environment{} };

    // Ermittle verschiedene Verzeichnisse
    std::println("Executable:    {}", env.executable_path().string());
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.config_dir.string());
    std::println("Cache Dir:     {}", env.cache_dir.string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Fehler: {}", e.what());
    return 1;    
}
```

## Testausführung 

```bash
cd build
./tests/test_pfadfinder
```

## Autor

Martin Fehrs
