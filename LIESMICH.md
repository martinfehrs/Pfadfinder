# Pfadfinder

Projekt zur plattformübergreifenden Bestimmung gebräuchlicher Anwendungsverzeichnisse.

## Beschreibung

Mittels einer Instanz der Klasse `pfadfinder::application_environment` erhält man
Zugriff auf folgende Betriebssystemabhängigen Pfade und deren Unterpfade.

### Windows

| Methode | Pfad |
|---------|------|
| `executable_path()` | Vollständiger Pfad zur ausführbaren Datei |
| `find_executable_path()` | Wie `executable_path()`, gibt `std::optional` zurück |
| `executable_dir()` | Verzeichnis, das die ausführbare Datei enthält |
| `find_executable_dir()` | Wie `executable_dir()`, gibt `std::optional` zurück |
| `static_data_dir()` | Binärverzeichnis |
| `find_static_data_dir()` | Wie `static_data_dir()`, gibt `std::optional` zurück |
| `shared_config_dir()` | `%ALLUSERSAPPDATA%\<appname>` |
| `find_shared_config_dir()` | Wie `shared_config_dir()`, gibt `std::optional` zurück |
| `create_shared_config_dir()` | Erstellt `%ALLUSERSAPPDATA%\<appname>` |
| `data_dir()` | `%APPDATA%\<appname>` |
| `find_data_dir()` | Wie `data_dir()`, gibt `std::optional` zurück |
| `create_data_dir()` | Erstellt `%APPDATA%\<appname>` |
| `user_config_dir()` | `%APPDATA%\<appname>` |
| `find_user_config_dir()` | Wie `user_config_dir()`, gibt `std::optional` zurück |
| `create_user_config_dir()` | Erstellt `%APPDATA%\<appname>` |
| `user_config_file()` | `%APPDATA%\<appname>\<datei>` (mit Dateiexistenzprüfung) |
| `find_user_config_file()` | Wie `user_config_file()`, gibt `std::optional` zurück |
| `shared_config_file()` | `%ALLUSERSAPPDATA%\<appname>\<datei>` (mit Dateiexistenzprüfung) |
| `find_shared_config_file()` | Wie `shared_config_file()`, gibt `std::optional` zurück |
| `find_config_file()` | Sucht Benutzer- dann geteilte Konfigurationsdatei (gibt `std::optional` zurück) |
| `config_files()` | Gibt alle verfügbaren Konfigurationsdateien zurück (0, 1 oder 2) |
| `cache_dir()` | `%LOCALAPPDATA%\<appname>\Cache` |
| `find_cache_dir()` | Wie `cache_dir()`, gibt `std::optional` zurück |
| `create_cache_dir()` | Erstellt `%LOCALAPPDATA%\<appname>\Cache` |
| `log_dir()` | `%LOCALAPPDATA%\<appname>\Logs` |
| `find_log_dir()` | Wie `log_dir()`, gibt `std::optional` zurück |
| `create_log_dir()` | Erstellt `%LOCALAPPDATA%\<appname>\Logs` |
| `temp_dir()` | `%TEMP%\<appname>` |
| `find_temp_dir()` | Wie `temp_dir()`, gibt `std::optional` zurück |
| `create_temp_dir()` | Erstellt `%TEMP%\<appname>` |
| `home_dir()` | `%USERPROFILE%` |
| `find_home_dir()` | Wie `home_dir()`, gibt `std::optional` zurück |

### macOS

| Methode | Bundle | CLI |
|---------|--------|-----|
| `executable_path()` | Vollständiger Pfad zur ausführbaren Datei | Vollständiger Pfad zur ausführbaren Datei |
| `find_executable_path()` | Wie `executable_path()`, gibt `std::optional` zurück | Wie `executable_path()`, gibt `std::optional` zurück |
| `executable_dir()` | Verzeichnis, das die ausführbare Datei enthält | Verzeichnis, das die ausführbare Datei enthält |
| `find_executable_dir()` | Wie `executable_dir()`, gibt `std::optional` zurück | Wie `executable_dir()`, gibt `std::optional` zurück |
| `static_data_dir()` | `Resources`-Verzeichnis | Abgeleitet vom Binärpfad |
| `find_static_data_dir()` | Wie `static_data_dir()`, gibt `std::optional` zurück | Wie `static_data_dir()`, gibt `std::optional` zurück |
| `shared_config_dir()` | `/Library/Preferences/<appname>` | `/Library/Preferences/<appname>` |
| `find_shared_config_dir()` | Wie `shared_config_dir()`, gibt `std::optional` zurück | Wie `shared_config_dir()`, gibt `std::optional` zurück |
| `create_shared_config_dir()` | Erstellt `/Library/Preferences/<appname>` | Erstellt `/Library/Preferences/<appname>` |
| `data_dir()` | `~/Library/Application Support/<appname>` | `~/.local/share/<appname>` |
| `find_data_dir()` | Wie `data_dir()`, gibt `std::optional` zurück | Wie `data_dir()`, gibt `std::optional` zurück |
| `create_data_dir()` | Erstellt `~/Library/Application Support/<appname>` | Erstellt `~/.local/share/<appname>` |
| `user_config_dir()` | `~/Library/Preferences/<appname>` | `~/.config/<appname>` |
| `find_user_config_dir()` | Wie `user_config_dir()`, gibt `std::optional` zurück | Wie `user_config_dir()`, gibt `std::optional` zurück |
| `create_user_config_dir()` | Erstellt `~/Library/Preferences/<appname>` | Erstellt `~/.config/<appname>` |
| `user_config_file()` | `~/Library/Preferences/<appname>/<datei>` (mit Dateiexistenzprüfung) | `~/.config/<appname>/<datei>` (mit Dateiexistenzprüfung) |
| `find_user_config_file()` | Wie `user_config_file()`, gibt `std::optional` zurück | Wie `user_config_file()`, gibt `std::optional` zurück |
| `shared_config_file()` | `/Library/Preferences/<appname>/<datei>` (mit Dateiexistenzprüfung) | `/Library/Preferences/<appname>/<datei>` (mit Dateiexistenzprüfung) |
| `find_shared_config_file()` | Wie `shared_config_file()`, gibt `std::optional` zurück | Wie `shared_config_file()`, gibt `std::optional` zurück |
| `find_config_file()` | Sucht Benutzer- dann geteilte Konfigurationsdatei (gibt `std::optional` zurück) | Sucht Benutzer- dann geteilte Konfigurationsdatei (gibt `std::optional` zurück) |
| `config_files()` | Gibt alle verfügbaren Konfigurationsdateien zurück (0, 1 oder 2) | Gibt alle verfügbaren Konfigurationsdateien zurück (0, 1 oder 2) |
| `cache_dir()` | `~/Library/Caches/<appname>` | `~/.cache/<appname>` |
| `find_cache_dir()` | Wie `cache_dir()`, gibt `std::optional` zurück | Wie `cache_dir()`, gibt `std::optional` zurück |
| `create_cache_dir()` | Erstellt `~/Library/Caches/<appname>` | Erstellt `~/.cache/<appname>` |
| `log_dir()` | `~/Library/Logs/<appname>` | `~/.local/state/<appname>/log` |
| `find_log_dir()` | Wie `log_dir()`, gibt `std::optional` zurück | Wie `log_dir()`, gibt `std::optional` zurück |
| `create_log_dir()` | Erstellt `~/Library/Logs/<appname>` | Erstellt `~/.local/state/<appname>/log` |
| `temp_dir()` | `~/Library/Caches/TemporaryItems/<appname>` | `/tmp/<appname>` |
| `find_temp_dir()` | Wie `temp_dir()`, gibt `std::optional` zurück | Wie `temp_dir()`, gibt `std::optional` zurück |
| `create_temp_dir()` | Erstellt `~/Library/Caches/TemporaryItems/<appname>` | Erstellt `/tmp/<appname>` |
| `home_dir()` | `$HOME` | `$HOME` |
| `find_home_dir()` | Wie `home_dir()`, gibt `std::optional` zurück | Wie `home_dir()`, gibt `std::optional` zurück |

### Linux

| Methode | Pfad |
|---------|------|
| `executable_path()` | Vollständiger Pfad zur ausführbaren Datei |
| `find_executable_path()` | Wie `executable_path()`, gibt `std::optional` zurück |
| `executable_dir()` | Verzeichnis, das die ausführbare Datei enthält |
| `find_executable_dir()` | Wie `executable_dir()`, gibt `std::optional` zurück |
| `static_data_dir()` | Abgeleitet vom Binärpfad (z. B. `/usr/bin/myapp` → `/usr/share/myapp`) |
| `find_static_data_dir()` | Wie `static_data_dir()`, gibt `std::optional` zurück |
| `shared_config_dir()` | `/etc/<appname>` |
| `find_shared_config_dir()` | Wie `shared_config_dir()`, gibt `std::optional` zurück |
| `create_shared_config_dir()` | Erstellt `/etc/<appname>` |
| `data_dir()` | `~/.local/share/<appname>` (XDG-Standard) |
| `find_data_dir()` | Wie `data_dir()`, gibt `std::optional` zurück |
| `create_data_dir()` | Erstellt `~/.local/share/<appname>` (XDG-Standard) |
| `user_config_dir()` | `~/.config/<appname>` (XDG-Standard) |
| `find_user_config_dir()` | Wie `user_config_dir()`, gibt `std::optional` zurück |
| `create_user_config_dir()` | Erstellt `~/.config/<appname>` (XDG-Standard) |
| `user_config_file()` | `~/.config/<appname>/<datei>` (mit Dateiexistenzprüfung) |
| `find_user_config_file()` | Wie `user_config_file()`, gibt `std::optional` zurück |
| `shared_config_file()` | `/etc/<appname>/<datei>` (mit Dateiexistenzprüfung) |
| `find_shared_config_file()` | Wie `shared_config_file()`, gibt `std::optional` zurück |
| `find_config_file()` | Sucht Benutzer- dann geteilte Konfigurationsdatei (gibt `std::optional` zurück) |
| `config_files()` | Gibt alle verfügbaren Konfigurationsdateien zurück (0, 1 oder 2) |
| `cache_dir()` | `~/.cache/<appname>` (XDG-Standard) |
| `find_cache_dir()` | Wie `cache_dir()`, gibt `std::optional` zurück |
| `create_cache_dir()` | Erstellt `~/.cache/<appname>` (XDG-Standard) |
| `log_dir()` | `~/.local/state/<appname>/log` (XDG Base Directory Specification) |
| `find_log_dir()` | Wie `log_dir()`, gibt `std::optional` zurück |
| `create_log_dir()` | Erstellt `~/.local/state/<appname>/log` (XDG Base Directory Specification) |
| `temp_dir()` | `/tmp/<appname>` oder systemweites Temp-Verzeichnis |
| `find_temp_dir()` | Wie `temp_dir()`, gibt `std::optional` zurück |
| `create_temp_dir()` | Erstellt `/tmp/<appname>` oder systemweites Temp-Verzeichnis |
| `home_dir()` | `$HOME` |
| `find_home_dir()` | Wie `home_dir()`, gibt `std::optional` zurück |

## Verschiedensprachige Dokumentation

- [🇩🇪 Deutsche Dokumentation](https://martinfehrs.github.io/Pfadfinder/de/)
- [🇬🇧 English Documentation](https://martinfehrs.github.io/Pfadfinder/en/)

## Voraussetzungen

- C++20
- CMake 3.28 oder höher
- Compiler mit C++-Modul-Unterstützung (GCC 15, Clang 19, MSVC 19.40+)

## Erstellung

Zuerst das Conan-Profil erstellen:

```bash
conan profile detect --force
```

Dann die Abhängigkeiten installieren und das Projekt bauen:

### Unter MacOS mit Clang:

**Hinweis:** Apple Clang unterstützt aktuell keine ausreichende C++-Modulunterstützung. Daher wird LLVM Clang verwendet.

Zuerst LLVM installieren:

```bash
brew install llvm@22
```

Dann die Umgebungsvariablen setzen:

```bash
LLVM_PREFIX=$(brew --prefix llvm@22)
export CC=$LLVM_PREFIX/bin/clang
export CXX=$LLVM_PREFIX/bin/clang++
export PATH="$LLVM_PREFIX/bin:$PATH"
```

Dann Conan und CMake ausführen:

```bash
conan install . --build=missing -s:a compiler=clang -s:a compiler.version=22 -s:a compiler.libcxx=libc++ -s:a compiler.cppstd=20 -s:a build_type=Release -c tools.cmake.cmaketoolchain:generator="Ninja"
cmake --preset=conan-release
cmake --build --preset=conan-release
```

### Unter Linux:

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -s:a build_type=Release -c tools.cmake.cmaketoolchain:generator="Ninja"
cmake --preset=conan-release
cmake --build --preset=conan-release
```

### Unter Windows mit Visual Studio 2022:

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -c tools.cmake.cmaketoolchain:generator="Visual Studio 17 2022"
cmake --preset=conan-default
cmake --build --preset=conan-release
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
    std::println("Home Dir:      {}", env.home_dir().string());
    std::println("Data Dir:      {}", env.data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
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
    std::println("Home Dir:      {}", env.home_dir().string());
    std::println("Data Dir:      {}", env.data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
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

struct my_custom_environment : pfadfinder::system_environment
{
    [[nodiscard]] std::optional<fs::path> user_config_dir(const fs::path&, const std::string& app_name) const override
    {
        auto home = system_environment::home_dir();
        if (!home.has_value())
            return std::nullopt;
        return *home / std::format(".{}", app_name);
    }

    [[nodiscard]] std::optional<fs::path> data_dir(const fs::path&, const std::string& app_name) const override
    {
        auto home = system_environment::home_dir();
        if (!home.has_value())
            return std::nullopt;
        return *home / std::format(".{}", app_name);
    }

    [[nodiscard]] std::optional<fs::path> cache_dir(const fs::path&, const std::string& app_name) const override
    {
        auto home = system_environment::home_dir();
        if (!home.has_value())
            return std::nullopt;
        return *home / std::format(".{}", app_name);
    }
};

int main() try
{
    pfadfinder::application_environment env{ my_custom_environment{} };

    // Ermittle verschiedene Verzeichnisse
    std::println("Executable:    {}", env.executable_path().string());
    std::println("Home Dir:      {}", env.home_dir().string());
    std::println("Data Dir:      {}", env.data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
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
cmake --build --preset=conan-release --target test
```

## Autor

Martin Fehrs
