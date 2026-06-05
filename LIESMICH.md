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
- `allusersappdata_not_set`     - Umgebungsvariable ALLUSERSAPPDATA nicht gesetzt (Windows)
- `get_module_file_name_failed` - GetModuleFileNameW scheiterte (Windows)
- `get_executable_path_failed`  - _NSGetExecutablePath scheiterte (macOS)
- `realpath_failed`             - realpath scheiterte (macOS)
- `file_not_found`              - Datei nicht gefunden
- `directory_not_found`         - Verzeichnis nicht gefunden

Alle Ausnahmen sind von `pfadfinder::error` abgeleitet, die ihrerseits
 von `std::runtime_error` abgeleitet ist.

### Methoden

#### `executable_path()`
Gibt den vollständigen Pfad zur ausführbaren Datei zurück.

**Rückgabewert:** `fs::path` - Der vollständige Pfad zur ausführbaren Datei.
**Ausnahmen:** Plattformspezifische Ausnahmen (siehe Fehlerbehandlung).

#### `executable_dir()`
Gibt das Verzeichnis der ausführbaren Datei zurück.

**Rückgabewert:** `fs::path` - Das Verzeichnis der ausführbaren Datei.

#### `static_data_dir(const fs::path& rel_path = "")`
Gibt das systemweite statische Datenverzeichnis der Anwendung zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `<Binärverzeichnis>\\<appname>` zurück (z. B. `C:\\App\\meine_app`)
- **Linux:** Leitet das share-Verzeichnis aus dem Binärverzeichnis ab
  (z. B. `/usr/share/meine_app`)
- **macOS Bundle:** Gibt das Resources-Verzeichnis zurück
  (z. B. `MeineApp.app/Contents/Resources/`)
- **macOS CLI:** Ähnlich wie Linux (z. B. `/usr/local/share/meine_app`)

**Rückgabewert:** `fs::path` - Das statische Datenverzeichnis (Basis oder Basis + rel_path).
**Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert.

#### `shared_data_dir(const fs::path& rel_path = "")`
Gibt das systemweite geteilte Datenverzeichnis der Anwendung zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%ALLUSERSAPPDATA%\\<appname>` zurück
- **Linux:** Gibt `/var/lib/<appname>` zurück
- **macOS:** Gibt `/Library/Application Support/<appname>` zurück

**Rückgabewert:** `fs::path` - Das geteilte Datenverzeichnis (Basis oder Basis + rel_path).
**Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert.

#### `user_data_dir` (überladen)
Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück. Erstellt das Verzeichnis optional, falls es nicht existiert.

**Überladungen:**

1. **`user_data_dir(bool create_dir = true)`**
   
   Gibt das Basis-Benutzer-Datenverzeichnis zurück.

   **Parameter:**
   - `create_dir`: Legt fest, ob das Verzeichnis erstellt werden soll (Default: `true`).

   **Plattform-spezifisches Verhalten:**
   - **Windows:** Gibt `%APPDATA%\<appname>` zurück
     (z. B. `C:\\Users\\Benutzer\\AppData\\Roaming\\meine_app`)
   - **Linux:** Gibt `~/.local/share/<appname>` zurück
     (z. B. `/home/benutzer/.local/share/meine_app`)
   - **macOS Bundle:** Gibt `~/Library/Application Support/<appname>` zurück
   - **macOS CLI:** Gibt `~/.local/share/<appname>` zurück

   **Rückgabewert:** `fs::path` - Das Benutzer-Datenverzeichnis.
   **Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert und `create_dir=false`.

2. **`user_data_dir(const fs::path& rel_path, bool create_dir = true)`**
   
   Gibt das Benutzer-Datenverzeichnis inkl. Unterpfad zurück. Erstellt das Verzeichnis optional, falls es nicht existiert.

   **Parameter:**
   - `rel_path`: Relativer Pfad zum Basis-Verzeichnis.
   - `create_dir`: Legt fest, ob das Verzeichnis erstellt werden soll (Default: `true`).

   **Plattform-spezifisches Verhalten:**
   - **Windows:** Gibt `%APPDATA%\<appname>\<rel_path>` zurück
   - **Linux:** Gibt `~/.local/share/<appname>/<rel_path>` zurück
   - **macOS Bundle:** Gibt `~/Library/Application Support/<appname>/<rel_path>` zurück
   - **macOS CLI:** Gibt `~/.local/share/<appname>/<rel_path>` zurück

   **Rückgabewert:** `fs::path` - Das Benutzer-Datenverzeichnis (Basis + rel_path).
   **Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert und `create_dir=false`.

#### `config_dir(const fs::path& rel_path = "")`
Gibt das Konfigurationsverzeichnis der Anwendung zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%\<appname>` zurück
- **Linux:** Gibt `~/.config/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Preferences/<appname>` zurück
- **macOS CLI:** Gibt `~/.config/<appname>` zurück

**Rückgabewert:** `fs::path` - Das Konfigurationsverzeichnis (Basis oder Basis + rel_path).
**Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert.

#### `create_config_dir(const fs::path& rel_path = "")`
Erstellt das Konfigurationsverzeichnis (inkl. rel_path) falls nicht vorhanden und gibt es zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Erstellt `%APPDATA%\<appname>\<rel_path>`
- **Linux:** Erstellt `~/.config/<appname>/<rel_path>`
- **macOS Bundle:** Erstellt `~/Library/Preferences/<appname>/<rel_path>`
- **macOS CLI:** Erstellt `~/.config/<appname>/<rel_path>`

**Rückgabewert:** `fs::path` - Das Konfigurationsverzeichnis (Basis + rel_path).

#### `cache_dir(const fs::path& rel_path = "")`
Gibt das Cache-Verzeichnis der Anwendung zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%\<appname>\\Cache` zurück
- **Linux:** Gibt `~/.cache/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Caches/<appname>` zurück
- **macOS CLI:** Gibt `~/.cache/<appname>` zurück

**Rückgabewert:** `fs::path` - Das Cache-Verzeichnis (Basis oder Basis + rel_path).
**Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert.

#### `create_cache_dir(const fs::path& rel_path = "")`
Erstellt das Cache-Verzeichnis (inkl. rel_path) falls nicht vorhanden und gibt es zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Erstellt `%LOCALAPPDATA%\<appname>\\Cache\<rel_path>`
- **Linux:** Erstellt `~/.cache/<appname>/<rel_path>`
- **macOS Bundle:** Erstellt `~/Library/Caches/<appname>/<rel_path>`
- **macOS CLI:** Erstellt `~/.cache/<appname>/<rel_path>`

**Rückgabewert:** `fs::path` - Das Cache-Verzeichnis (Basis + rel_path).

#### `log_dir(const fs::path& rel_path = "")`
Gibt das Log-Verzeichnis der Anwendung zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%\<appname>\\Logs` zurück
- **Linux:** Gibt `~/.local/state/<appname>/log` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Logs/<appname>` zurück
- **macOS CLI:** Gibt `~/.local/state/<appname>/log` zurück

**Rückgabewert:** `fs::path` - Das Log-Verzeichnis (Basis oder Basis + rel_path).
**Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert.

#### `create_log_dir(const fs::path& rel_path = "")`
Erstellt das Log-Verzeichnis (inkl. rel_path) falls nicht vorhanden und gibt es zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Erstellt `%LOCALAPPDATA%\<appname>\\Logs\<rel_path>`
- **Linux:** Erstellt `~/.local/state/<appname>/log/<rel_path>`
- **macOS Bundle:** Erstellt `~/Library/Logs/<appname>/<rel_path>`
- **macOS CLI:** Erstellt `~/.local/state/<appname>/log/<rel_path>`

**Rückgabewert:** `fs::path` - Das Log-Verzeichnis (Basis + rel_path).

#### `temp_dir(const fs::path& rel_path = "")`
Gibt das temporäre Verzeichnis der Anwendung zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%TEMP%\<appname>` zurück
- **Linux:** Gibt `/tmp/<appname>` zurück
- **macOS Bundle:** Gibt `~/Library/Caches/TemporaryItems/<appname>` zurück
- **macOS CLI:** Gibt `/tmp/<appname>` zurück

**Rückgabewert:** `fs::path` - Das temporäre Verzeichnis (Basis oder Basis + rel_path).
**Ausnahmen:** `directory_not_found` - Wenn das Verzeichnis nicht existiert.

#### `create_temp_dir(const fs::path& rel_path = "")`
Erstellt das temporäre Verzeichnis (inkl. rel_path) falls nicht vorhanden und gibt es zurück.

**Parameter:**
- `rel_path`: Relativer Pfad zum Basis-Verzeichnis (optional).

**Plattform-spezifisches Verhalten:**
- **Windows:** Erstellt `%TEMP%\<appname>\<rel_path>`
- **Linux:** Erstellt `/tmp/<appname>/<rel_path>`
- **macOS Bundle:** Erstellt `~/Library/Caches/TemporaryItems/<appname>/<rel_path>`
- **macOS CLI:** Erstellt `/tmp/<appname>/<rel_path>`

**Rückgabewert:** `fs::path` - Das temporäre Verzeichnis (Basis + rel_path).

#### `static_data_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im statischen Datenverzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `static_data_dir()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des statischen Datenverzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `user_data_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im Benutzer-Datenverzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `user_data_dir()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des Benutzer-Datenverzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `cache_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im Cache-Verzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `cache_dir()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des Cache-Verzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `log_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im Log-Verzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `log_dir()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des Log-Verzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `temp_file(const fs::path& rel_path)`
Gibt den absoluten Pfad zu einer Datei im temporären Verzeichnis zurück.

Sucht nach der durch `rel_path` angegebenen Datei im durch `temp_dir()`
zurückgegebenen Verzeichnis.

**Parameter:**
- `rel_path`: Relativer Pfad zur Datei innerhalb des temporären Verzeichnisses.

**Rückgabewert:** `fs::path` - Absoluter Pfad zur Datei.

**Ausnahmen:**
- `file_not_found`: Wenn die Datei nicht gefunden wurde.

#### `user_dir()`
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
    pfadfinder::application_environment env("MeineApp");
    
    try
    {
        // Ermittle verschiedene Verzeichnisse
        std::println("Executable: {}", env.executable_path().string());
        std::println("Executable Dir: {}", env.executable_dir().string());
        std::println("User Dir: {}", env.user_dir().string());
        
        // Für Verzeichnisse, die existieren müssen: erst erstellen, dann verwenden
        auto user_data_dir = env.user_data_dir();
        std::println("User Data Dir: {}", user_data_dir.string());
        
        auto config_dir = env.create_config_dir();
        std::println("Config Dir: {}", config_dir.string());
        
        auto cache_dir = env.create_cache_dir();
        std::println("Cache Dir: {}", cache_dir.string());
        
        auto log_dir = env.create_log_dir();
        std::println("Log Dir: {}", log_dir.string());
        
        auto temp_dir = env.create_temp_dir();
        std::println("Temp Dir: {}", temp_dir.string());
        
        // static_data_dir() wirft, wenn das Verzeichnis nicht existiert
        std::println("Data Dir: {}", env.static_data_dir().string());
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
