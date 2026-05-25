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

```cpp
import pfadfinder;

pfadfinder::application_environment env("meine_app");
```

**Parameter:**
- `app_name` (erforderlich): Der Name der Anwendung, der für alle
  Verzeichnispfade verwendet wird.

### Methoden

#### `executable_path()`
Gibt den vollständigen Pfad zur ausführbaren Datei zurück.

**Rückgabewert:** `std::filesystem::path` - Der vollständige Pfad zur ausführbaren Datei.

**Ausnahmen:** `std::runtime_error` - Falls der Pfad nicht bestimmt werden kann.

**Beispiel:**
```cpp
pfadfinder::application_environment env("meine_app");
auto path = env.executable_path();
// Linux: /usr/bin/test_pfadfinder
// Windows: C:\\Projekte\\Pfadfinder\\build\\tests\\test_pfadfinder.exe
// macOS: /Users/martin/Projekte/Pfadfinder/build/tests/test_pfadfinder
```

#### `executable_directory()`
Gibt das Verzeichnis zurück, das die ausführbare Datei enthält.

**Rückgabewert:** `std::filesystem::path` - Das Verzeichnis der ausführbaren Datei.

**Beispiel:**
```cpp
pfadfinder::application_environment env("meine_app");
auto dir = env.executable_directory();
```

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

**Rückgabewert:** `std::filesystem::path` - Das Datenverzeichnis.

**Ausnahmen:** `std::runtime_error` - Falls der Pfad nicht bestimmt werden kann.

#### `user_data_directory()`
Gibt das benutzer-spezifische Datenverzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%/<appname>` zurück
  (z. B. `C:\\Users\\Benutzer\\AppData\\Roaming\\meine_app`)
- **Linux:** Gibt `~/.local/share/<appname>` zurück
  (z. B. `/home/benutzer/.local/share/meine_app`)
- **macOS Bundle:** Gibt `~/Library/Application Support/<appname>` zurück
- **macOS CLI:** Gibt `~/.local/share/<appname>` zurück

**Rückgabewert:** `std::filesystem::path` - Das Benutzer-Datenverzeichnis.

**Ausnahmen:** `std::runtime_error` - Falls die Umgebungsvariable (APPDATA/HOME)
  nicht gesetzt ist oder die Plattform nicht unterstützt wird.

#### `config_directory()`
Gibt das Konfigurationsverzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%APPDATA%/<appname>` zurück
- **Linux:** Gibt `~/.config/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Preferences/<appname>` zurück
- **macOS CLI:** Gibt `~/.config/<appname>` zurück

**Rückgabewert:** `std::filesystem::path` - Das Konfigurationsverzeichnis.

**Ausnahmen:** `std::runtime_error` - Falls die Umgebungsvariable (APPDATA/HOME)
  nicht gesetzt ist oder die Plattform nicht unterstützt wird.

#### `cache_directory()`
Gibt das Cache-Verzeichnis der Anwendung zurück.

**Plattform-spezifisches Verhalten:**
- **Windows:** Gibt `%LOCALAPPDATA%/<appname>/Cache` zurück
- **Linux:** Gibt `~/.cache/<appname>` zurück (XDG Base Directory Specification)
- **macOS Bundle:** Gibt `~/Library/Caches/<appname>` zurück
- **macOS CLI:** Gibt `~/.cache/<appname>` zurück

**Rückgabewert:** `std::filesystem::path` - Das Cache-Verzeichnis.

**Ausnahmen:** `std::runtime_error` - Falls die Umgebungsvariable
  (LOCALAPPDATA/HOME) nicht gesetzt ist oder die Plattform nicht unterstützt
  wird.

## Verwendungsbeispiel

```cpp
#include <iostream>
import pfadfinder;

int main()
{
    // Erstelle eine Umgebung für die Anwendung "MeineApp"
    pfadfinder::application_environment env("MeineApp");
    
    // Ermittle verschiedene Verzeichnisse
    std::cout << "Executable: " << env.executable_path().string() << std::endl;
    std::cout << "Executable Dir: " << env.executable_directory().string() << std::endl;
    std::cout << "Data Dir: " << env.data_directory().string() << std::endl;
    std::cout << "User Data Dir: " << env.user_data_directory().string() << std::endl;
    std::cout << "Config Dir: " << env.config_directory().string() << std::endl;
    std::cout << "Cache Dir: " << env.cache_directory().string() << std::endl;
    
    return 0;
}
```

## Voraussetzungen

- C++20
- CMake 4.3 oder höher
- Compiler mit C++20 Modul-Unterstützung (GCC 15, Clang 17, MSVC 19.40+)

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
├── LIESMICH.md          # Diese Datei
├── CMakeLists.txt       # Haupt-CMake-Konfiguration
├── src/
│   └── pfadfinder.cppm  # C++23 Schnittstellenmodul
└── tests/
    ├── CMakeLists.txt   # Test-Konfiguration
    └── test_pfadfinder.cpp  # Testfälle mit CATCH2
```

## Autor

Martin Fehrs
