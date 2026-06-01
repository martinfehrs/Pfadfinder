# Pfadfinder

Projekt zur Bestimmung verschiedener Verzeichnisse einer laufenden Anwendung.

## Beschreibung

Dieses Projekt stellt die Klasse `pfadfinder::application_environment` bereit,
 die Methoden zur Ermittlung verschiedener Verzeichnispfade für eine Anwendung
 auf verschiedenen Plattformen (Windows, Linux, macOS) bereitstellt.

Die Implementierung unterstützt:
- Windows (mittels `GetModuleFileNameW`)
- macOS (mittels `_NSGetExecutablePath` und `realpath`)
- Linux (mittels `/proc/self/exe`)

## Klasse: `application_environment`

Die Hauptklasse des Moduls, die alle Pfadfunktionen als Methoden bereitstellt.

### Konstruktor

**Parameter:**
- `app_name` (optional): Der Name der Anwendung, der für alle
  Verzeichnispfade verwendet wird. Wird kein Name angegeben, wird der Dateiname
  der ausführbaren Datei (ohne Endung) als Anwendungsname verwendet.

### Fehlerbehandlung

Alle Pfadfunktionen geben `fs::path` zurück und können Ausnahmen werfen, wenn
 ein Fehler auftritt. Die folgenden Ausnahmen können geworfen werden:

- `home_not_set`                - Umgebungsvariable HOME nicht gesetzt
- `readlink_failed`             - readlink /proc/self/exe scheiterte (Linux)
- `appdata_not_set`             - Umgebungsvariable APPDATA nicht gesetzt (Windows)
- `localappdata_not_set`        - Umgebungsvariable LOCALAPPDATA nicht gesetzt (Windows)
- `get_module_file_name_failed` - GetModuleFileNameW scheiterte (Windows)
- `get_executable_path_failed`  - _NSGetExecutablePath scheiterte (macOS)
- `realpath_failed`             - realpath scheiterte (macOS)
- `file_not_found`              - Datei nicht gefunden

Alle Ausnahmen sind von `pfadfinder::error` abgeleitet, die ihrerseits
 von `std::runtime_error` abgeleitet ist.

### Methoden

#### `executable_path()`
Gibt den vollständigen Pfad zur ausführbaren Datei zurück.

**Rückgabewert:** `fs::path` - Der vollständige Pfad zur ausführbaren Datei.
**Ausnahmen:** Plattformspezifische Ausnahmen (siehe Fehlerbehandlung).

#### `executable_directory()`
Gibt das Verzeichnis zurück, das die ausführbare Datei enthält.

**Rückgabewert:** `fs::path` - Das Verzeichnis der ausführbaren Datei.

#### `data_directory()`
Gibt das systemweite Datenverzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `<Binärverzeichnis>` zurück (z. B. `C:\\App`)
- **Linux:** Leitet das share-Verzeichnis aus dem Binärverzeichnis ab
  (z. B. `/usr/share/meine_app`)
- **macOS Bundle:** Gibt das Resources-Verzeichnis zurück
  (z. B. `MeineApp.app/Contents/Resources/`)
- **macOS CLI:** Ähnlich wie Linux (z. B. `/usr/local/share/meine_app`)

**Rückgabewert:** `fs::path` - Das Datenverzeichnis.

#### `user_data_directory(bool create = true)`
Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.

**Parameter:**
- `create`: Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%\<appname>` zurück
  (z. B. `C:\\Users\\Benutzer\\AppData\\Roaming\\meine_app`)
- **Linux:** Gibt `~/.local/share/<appname>` zurück
  (z. B. `/home/benutzer/.local/share/meine_app`)
- **macOS Bundle:** Gibt `~/Library/Application Support/<appname>` zurück
- **macOS CLI:** Gibt `~/.local/share/<appname>` zurück

**Rückgabewert:** `fs::path` - Das Benutzer-Datenverzeichnis.

#### `config_directory(bool create = true)`
Gibt das Konfigurationsverzeichnis der Anwendung zurück.

**Parameter:**
- `create`: Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%\<appname>` zurück
- **Linux:** Gibt `~/.config/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Preferences/<appname>` zurück
- **macOS CLI:** Gibt `~/.config/<appname>` zurück

**Rückgabewert:** `fs::path` - Das Konfigurationsverzeichnis.

#### `cache_directory(bool create = true)`
Gibt das Cache-Verzeichnis der Anwendung zurück.

**Parameter:**
- `create`: Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%\<appname>\\Cache` zurück
- **Linux:** Gibt `~/.cache/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Caches/<appname>` zurück
- **macOS CLI:** Gibt `~/.cache/<appname>` zurück

**Rückgabewert:** `fs::path` - Das Cache-Verzeichnis.

#### `log_directory(bool create = true)`
Gibt das Log-Verzeichnis der Anwendung zurück.

**Parameter:**
- `create`: Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%\<appname>\\Logs` zurück
- **Linux:** Gibt `~/.local/state/<appname>/log` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Logs/<appname>` zurück
- **macOS CLI:** Gibt `~/.local/state/<appname>/log` zurück

**Rückgabewert:** `fs::path` - Das Log-Verzeichnis.

#### `temp_directory(bool create = true)`
Gibt das temporäre Verzeichnis der Anwendung zurück.

**Parameter:**
- `create`: Wenn true, wird das Verzeichnis erstellt, falls es nicht existiert.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%TEMP%\<appname>` zurück
- **Linux:** Gibt `/tmp/<appname>` zurück
- **macOS Bundle:** Gibt `~/Library/Caches/TemporaryItems/<appname>` zurück
- **macOS CLI:** Gibt `/tmp/<appname>` zurück

**Rückgabewert:** `fs::path` - Das temporäre Verzeichnis.

#### `data_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im Datenverzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `data_directory()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des Datenverzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `user_data_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im Benutzer-Datenverzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `user_data_directory()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des Benutzer-Datenverzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `user_directory()`
Gibt das Home-Verzeichnis des Benutzers zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%USERPROFILE%` zurück
- **Linux und macOS:** Gibt `$HOME` zurück

**Rückgabewert:** `fs::path` - Das Home-Verzeichnis.

## Verwendungsbeispiel

```cpp
import std;
import pfadfinder;

int main()
{
    // Erstelle eine Umgebung
    pfadfinder::application_environment env;
    
    try
    {
        // Ermittle verschiedene Verzeichnisse
        std::println("Executable: {}", env.executable_path().string());
        std::println("Executable Dir: {}", env.executable_directory().string());
        std::println("Data Dir: {}", env.data_directory().string());
        std::println("User Data Dir: {}", env.user_data_directory().string());
        std::println("Config Dir: {}", env.config_directory().string());
        std::println("Cache Dir: {}", env.cache_directory().string());
        std::println("Log Dir: {}", env.log_directory().string());
        std::println("Temp Dir: {}", env.temp_directory().string());
        std::println("User Dir: {}", env.user_directory().string());
    }
    catch (const pfadfinder::error& e)
    {
        std::println(stderr, "Fehler: {}", e.what());
        return 1;
    }
    
    return 0;
}
```

## Voraussetzungen

- C++23
- CMake 3.28 oder höher
- Compiler mit C++-Modul-Unterstützung (GCC 15, Clang 19, MSVC 19.40+)

## Build

Unter Linux:

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G "Ninja Multi-Config" ..
cmake --build . --config Release
```

Unter Windows mit Visual Studio 2022:

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## Tests ausführen

```bash
cd build
./tests/test_pfadfinder
```

## Projektstruktur

```
Pfadfinder/
├── CMakeLists.txt                  # Haupt-CMake-Konfiguration
├── LIESMICH.md                     # Diese Datei
├── src/
│   ├── error.cppm                  # Fehlerbehandlung (Ausnahmeklassen)
│   ├── pfadfinder.cppm             # Hauptmodul
│   ├── system_backend_linux.cppm   # Linux-spezifische Implementierung
│   ├── system_backend_windows.cppm # Windows-spezifische Implementierung
│   └── system_backend_macos.cppm   # macOS-spezifische Implementierung
└── tests/
    ├── CMakeLists.txt              # Test-Konfiguration
    └── test_pfadfinder.cpp         # Testfälle mit CATCH2
```

## Autor

Martin Fehrs
