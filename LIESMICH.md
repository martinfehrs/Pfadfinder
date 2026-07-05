# Pfadfinder

Projekt zur plattformübergreifenden Bestimmung gebräuchlicher Anwendungsverzeichnisse.

## Beschreibung

Mittels einer Instanz der Klasse `pfadfinder::application_environment` erhält man
Zugriff auf folgende Betriebssystemabhängigen Pfade und deren Unterpfade.

### Windows

| Methode | Pfad |
|---------|------|
| `executable_path()` | Vollständiger Pfad zur ausführbaren Datei |
| `executable_dir()` | Verzeichnis, das die ausführbare Datei enthält |
| `static_data_dir()` | Binärverzeichnis |
| `shared_data_dir()` | `%ALLUSERSAPPDATA%\<appname>` |
| `shared_config_dir()` | `%ALLUSERSAPPDATA%\<appname>` |
| `shared_cache_dir()` | `%ALLUSERSAPPDATA%\<appname>\Cache` |
| `shared_log_dir()` | `%ALLUSERSAPPDATA%\<appname>\Logs` |
| `user_data_dir()` | `%APPDATA%\<appname>` |
| `config_dir()` | `%APPDATA%\<appname>` |
| `cache_dir()` | `%LOCALAPPDATA%\<appname>\Cache` |
| `log_dir()` | `%LOCALAPPDATA%\<appname>\Logs` |
| `temp_dir()` | `%TEMP%\<appname>` |
| `user_dir()` | `%USERPROFILE%` |

### macOS

| Methode | Bundle | CLI |
|---------|--------|-----|
| `executable_path()` | Vollständiger Pfad zur ausführbaren Datei | Vollständiger Pfad zur ausführbaren Datei |
| `executable_dir()` | Verzeichnis, das die ausführbare Datei enthält | Verzeichnis, das die ausführbare Datei enthält |
| `static_data_dir()` | `Resources`-Verzeichnis | Abgeleitet vom Binärpfad |
| `shared_data_dir()` | `/Library/Application Support/<appname>` | `/Library/Application Support/<appname>` |
| `shared_config_dir()` | `/Library/Preferences/<appname>` | `/Library/Preferences/<appname>` |
| `shared_cache_dir()` | `/Library/Caches/<appname>` | `/Library/Caches/<appname>` |
| `shared_log_dir()` | `/Library/Logs/<appname>` | `/Library/Logs/<appname>` |
| `user_data_dir()` | `~/Library/Application Support/<appname>` | `~/.local/share/<appname>` |
| `config_dir()` | `~/Library/Preferences/<appname>` | `~/.config/<appname>` |
| `cache_dir()` | `~/Library/Caches/<appname>` | `~/.cache/<appname>` |
| `log_dir()` | `~/Library/Logs/<appname>` | `~/.local/state/<appname>/log` |
| `temp_dir()` | `~/Library/Caches/TemporaryItems/<appname>` | `/tmp/<appname>` |
| `user_dir()` | `$HOME` | `$HOME` |

### Linux

| Methode | Pfad |
|---------|------|
| `executable_path()` | Vollständiger Pfad zur ausführbaren Datei |
| `executable_dir()` | Verzeichnis, das die ausführbare Datei enthält |
| `static_data_dir()` | Abgeleitet vom Binärpfad (z. B. `/usr/bin/myapp` → `/usr/share/myapp`) |
| `shared_data_dir()` | `/var/lib/<appname>` |
| `shared_config_dir()` | `/etc/<appname>` |
| `shared_cache_dir()` | `/var/cache/<appname>` |
| `shared_log_dir()` | `/var/log/<appname>` |
| `user_data_dir()` | `~/.local/share/<appname>` (XDG-Standard) |
| `config_dir()` | `~/.config/<appname>` (XDG-Standard) |
| `cache_dir()` | `~/.cache/<appname>` (XDG-Standard) |
| `log_dir()` | `~/.local/state/<appname>/log` (XDG Base Directory Specification) |
| `temp_dir()` | `/tmp/<appname>` oder systemweites Temp-Verzeichnis |
| `user_dir()` | `$HOME` |

## Dokumentation

- [🇩🇪 Deutsche Dokumentation](https://martinfehrs.github.io/Pfadfinder/de/)
- [🇬🇧 English Documentation](https://martinfehrs.github.io/Pfadfinder/en/)

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
