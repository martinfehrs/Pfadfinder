# Pfadfinder

Projekt zur Bestimmung verschiedener Verzeichnisse einer laufenden Anwendung.

## Beschreibung

Dieses Projekt stellt die Klasse `pfadfinder::application_environment` bereit,
 die Methoden zur Ermittlung verschiedener Verzeichnispfade für eine Anwendung
 auf verschiedenen Plattformen (Windows, Linux, macOS) bereitstellt.

Die Implementierung unterstützt:
- Windows (mittels `GetModuleFileNameW`, `GetModuleFileName`)
- macOS (mittels `_NSGetExecutablePath` und `realpath`)
- Linux (mittels `/proc/self/exe`)

## Klasse: `application_environment`

Die Hauptklasse des Moduls, die alle Pfadfunktionen als Methoden bereitstellt.

### Konstruktor

**Parameter:**
- `app_name` (erforderlich): Der Name der Anwendung, der für alle
  Verzeichnispfade verwendet wird.

### Fehlerbehandlung

Alle Pfadfunktionen geben `std::expected<fs::path, error>` zurück, wobei `error`
 eine Aufzählung ist, die plattformspezifische Fehlerfälle beschreibt:

- `platform_not_supported` - Plattform wird nicht unterstützt
- `windows_get_module_file_name_failed` - GetModuleFileNameW scheiterte
- `appdata_not_set` - Umgebungsvariable APPDATA nicht gesetzt
- `localappdata_not_set` - Umgebungsvariable LOCALAPPDATA nicht gesetzt
- `macos_get_executable_path_failed` - _NSGetExecutablePath scheiterte
- `macos_realpath_failed` - realpath scheiterte
- `linux_readlink_failed` - readlink /proc/self/exe scheiterte
- `home_not_set` - Umgebungsvariable HOME nicht gesetzt

### Methoden

#### `executable_path()`
Gibt den vollständigen Pfad zur ausführbaren Datei zurück.

**Rückgabewert:** `std::expected<fs::path, error>` - Der vollständige Pfad zur
ausführbaren Datei oder ein Fehlercode.

#### `executable_directory()`
Gibt das Verzeichnis zurück, das die ausführbare Datei enthält.

**Rückgabewert:** `std::expected<fs::path, error>` - Das Verzeichnis der
ausführbaren Datei oder ein Fehlercode.

#### `data_directory()`
Gibt das systemweite Datenverzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt das Binärverzeichnis mit app_name Unterverzeichnis zurück
  (z. B. `C:\\App\\meine_app`)
- **Linux:** Leitet das share-Verzeichnis aus dem Binärverzeichnis ab
  (z. B. `/usr/share/meine_app`)
- **macOS Bundle:** Gibt das Resources-Verzeichnis zurück
  (z. B. `MeineApp.app/Contents/Resources/`)
- **macOS CLI:** Ähnlich wie Linux (z. B. `/usr/local/share/meine_app`)

**Rückgabewert:** `std::expected<fs::path, error>` - Das Datenverzeichnis
oder ein Fehlercode.

#### `user_data_directory()`
Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%\<appname>` zurück
  (z. B. `C:\\Users\\Benutzer\\AppData\\Roaming\\meine_app`)
- **Linux:** Gibt `~/.local/share/<appname>` zurück
  (z. B. `/home/benutzer/.local/share/meine_app`)
- **macOS Bundle:** Gibt `~/Library/Application Support/<appname>` zurück
- **macOS CLI:** Gibt `~/.local/share/<appname>` zurück

**Rückgabewert:** `std::expected<fs::path, error>` - Das Benutzer-Datenverzeichnis
oder ein Fehlercode.

#### `config_directory()`
Gibt das Konfigurationsverzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%\<appname>` zurück
- **Linux:** Gibt `~/.config/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Preferences/<appname>` zurück
- **macOS CLI:** Gibt `~/.config/<appname>` zurück

**Rückgabewert:** `std::expected<fs::path, error>` - Das Konfigurationsverzeichnis
oder ein Fehlercode.

#### `cache_directory()`
Gibt das Cache-Verzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%\<appname>\Cache` zurück
- **Linux:** Gibt `~/.cache/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Caches/<appname>` zurück
- **macOS CLI:** Gibt `~/.cache/<appname>` zurück

**Rückgabewert:** `std::expected<fs::path, error>` - Das Cache-Verzeichnis
oder ein Fehlercode.

#### `log_directory()`
Gibt das Log-Verzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%\<appname>\Logs` zurück
- **Linux:** Gibt `~/.local/state/<appname>/log` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Logs/<appname>` zurück
- **macOS CLI:** Gibt `~/.local/state/<appname>/log` zurück

**Rückgabewert:** `std::expected<fs::path, error>` - Das Log-Verzeichnis
oder ein Fehlercode.

#### `temp_directory()`
Gibt das temporäre Verzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%TEMP%\<appname>` zurück
- **Linux:** Gibt `/tmp/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Caches/TemporaryItems/<appname>` zurück
- **macOS CLI:** Gibt `/tmp/<appname>` zurück

**Rückgabewert:** `std::expected<fs::path, error>` - Das temporäre Verzeichnis
oder ein Fehlercode.

#### `user_directory()`
Gibt das Home-Verzeichnis des Benutzers zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%USERPROFILE%` zurück
- **Linux und macOS:** Gibt `$HOME` zurück

**Rückgabewert:** `std::expected<fs::path, error>` - Das Home-Verzeichnis
oder ein Fehlercode.

## Verwendungsbeispiel

```cpp
import std;
import pfadfinder;

int main()
{
    // Erstelle eine Umgebung für die Anwendung "MeineApp"
    pfadfinder::application_environment env("MeineApp");
    
    // Ermittle verschiedene Verzeichnisse
    auto exec_result = env.executable_path();
    if (exec_result)
        std::println("Executable: {}", exec_result->string());
    else
        std::println(stderr, "Error: {}", pfadfinder::error_message(exec_result.error()));
    
    auto exec_dir_result = env.executable_directory();
    if (exec_dir_result)
        std::println("Executable Dir: {}", exec_dir_result->string());
    else
        std::println(stderr, "Error: {}", pfadfinder::error_message(exec_dir_result.error()));
    
    auto data_dir_result = env.data_directory();
    if (data_dir_result)
        std::println("Data Dir: {}", data_dir_result->string());
    else
        std::println(stderr, "Error: {}", pfadfinder::error_message(data_dir_result.error()));
    
    auto user_data_dir_result = env.user_data_directory();
    if (user_data_dir_result)
        std::println("User Data Dir: {}", user_data_dir_result->string());
    else
        std::println(stderr, "Error: {}", pfadfinder::error_message(user_data_dir_result.error()));
    
    auto config_dir_result = env.config_directory();
    if (config_dir_result)
        std::println("Config Dir: {}", config_dir_result->string());
    else
        std::println(stderr, "Error: {}", pfadfinder::error_message(config_dir_result.error()));
    
    auto cache_dir_result = env.cache_directory();
    if (cache_dir_result)
        std::println("Cache Dir: {}", cache_dir_result->string());
    else
        std::println(stderr, "Error: {}", pfadfinder::error_message(cache_dir_result.error()));
    
    return 0;
}
```

## Voraussetzungen

- C++23
- CMake 4.3 oder höher
- Compiler mit C++23 Modul-Unterstützung (GCC 15, Clang 17, MSVC 19.40+)

## Build

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G Ninja ..
cmake --build .
```

## Tests ausführen

```bash
cd build
./tests/test_pfadfinder
```

## Projektstruktur

```
Pfadfinder/
├── .github/
│   └── workflows/
│       └── cmake.yml    # GitHub Actions CI-Konfiguration
├── CMakeLists.txt       # Haupt-CMake-Konfiguration
├── CMakePresets.json    # CMake Presets (Ninja als Standard)
├── LIESMICH.md          # Diese Datei
├── config.hpp.in        # Vorlage für Plattform-Makros
├── src/
│   └── pfadfinder.cppm  # C++23 Schnittstellenmodul
└── tests/
    ├── CMakeLists.txt   # Test-Konfiguration
    └── test_pfadfinder.cpp  # Testfälle mit CATCH2
```

## Autor

Martin Fehrs
