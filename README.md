# Pfadfinder

Project for determining various directories of a running application.

## Description

This project provides the `pfadfinder::application_environment` class, which offers
methods to determine various path directories for an application across different
platforms (Windows, Linux, macOS).

The implementation supports:
- Windows (using `GetModuleFileNameW`, `GetModuleFileName`)
- macOS (using `_NSGetExecutablePath` and `realpath`)
- Linux (using `/proc/self/exe`)

## Class: `application_environment`

The main class of the module that provides all path functions as methods.

### Constructor

```cpp
#include <filesystem>
import pfadfinder;

pfadfinder::application_environment env("my_app");
```

**Parameter:**
- `app_name` (required): The name of the application, used for all directory paths.

### Methods

#### `executable_path()`
Returns the full path to the executable file.

**Return value:** `std::filesystem::path` - The full path to the executable file.

**Exceptions:** `std::runtime_error` - If the path cannot be determined.

**Example:**
```cpp
pfadfinder::application_environment env("my_app");
auto path = env.executable_path();
// Linux: /usr/bin/test_pfadfinder
// Windows: C:\\Projects\\Pfadfinder\\build\\tests\\test_pfadfinder.exe
// macOS: /Users/martin/Projects/Pfadfinder/build/tests/test_pfadfinder
```

#### `executable_directory()`
Returns the directory containing the executable file.

**Return value:** `std::filesystem::path` - The directory of the executable file.

**Example:**
```cpp
pfadfinder::application_environment env("my_app");
auto dir = env.executable_directory();
```

#### `data_directory()`
Returns the system-wide data directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns the binary directory with app_name subdirectory
  (e.g. `C:\\App\\my_app`)
- **Linux:** Derives the share directory from the binary directory
  (e.g. `/usr/share/my_app`)
- **macOS Bundle:** Returns the Resources directory
  (e.g. `MyApp.app/Contents/Resources/`)
- **macOS CLI:** Similar to Linux (e.g. `/usr/local/share/my_app`)

**Return value:** `std::filesystem::path` - The data directory.

**Exceptions:** `std::runtime_error` - If the path cannot be determined.

#### `user_data_directory()`
Returns the user-specific data directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\<appname>`
  (e.g. `C:\\Users\\User\\AppData\\Roaming\\my_app`)
- **Linux:** Returns `~/.local/share/<appname>`
  (e.g. `/home/user/.local/share/my_app`)
- **macOS Bundle:** Returns `~/Library/Application Support/<appname>`
- **macOS CLI:** Returns `~/.local/share/<appname>`

**Return value:** `std::filesystem::path` - The user data directory.

**Exceptions:** `std::runtime_error` - If the environment variable (APPDATA/HOME)
  is not set or the platform is not supported.

#### `config_directory()`
Returns the configuration directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\<appname>`
- **Linux:** Returns `~/.config/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Preferences/<appname>`
- **macOS CLI:** Returns `~/.config/<appname>`

**Return value:** `std::filesystem::path` - The configuration directory.

**Exceptions:** `std::runtime_error` - If the environment variable (APPDATA/HOME)
  is not set or the platform is not supported.

#### `cache_directory()`
Returns the cache directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\<appname>\Cache`
- **Linux:** Returns `~/.cache/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Caches/<appname>`
- **macOS CLI:** Returns `~/.cache/<appname>`

**Return value:** `std::filesystem::path` - The cache directory.

**Exceptions:** `std::runtime_error` - If the environment variable
  (LOCALAPPDATA/HOME) is not set or the platform is not supported.

## Usage Example

```cpp
#include <iostream>
import pfadfinder;

int main()
{
    // Create an environment for the application "MyApp"
    pfadfinder::application_environment env("MyApp");
    
    // Determine various directories
    std::cout << "Executable: " << env.executable_path().string() << std::endl;
    std::cout << "Executable Dir: " << env.executable_directory().string() << std::endl;
    std::cout << "Data Dir: " << env.data_directory().string() << std::endl;
    std::cout << "User Data Dir: " << env.user_data_directory().string() << std::endl;
    std::cout << "Config Dir: " << env.config_directory().string() << std::endl;
    std::cout << "Cache Dir: " << env.cache_directory().string() << std::endl;
    
    return 0;
}
```

## Requirements

- C++23
- CMake 4.3 or higher
- Compiler with C++23 module support (GCC 15, Clang 17, MSVC 19.40+)

## Build

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G Ninja ..
cmake --build .
```

## Run Tests

```bash
cd build
./tests/test_pfadfinder
```

## Project Structure

```
Pfadfinder/
├── .github/
│   └── workflows/
│       └── cmake.yml    # GitHub Actions CI configuration
├── CMakeLists.txt       # Main CMake configuration
├── CMakePresets.json    # CMake Presets (Ninja as default)
├── LIESMICH.md          # Projektbeschreibung (Deutsch)
├── README.md           # Project description (English)
├── config.hpp.in        # Template for platform macros
├── src/
│   └── pfadfinder.cppm  # C++23 interface module
└── tests/
    ├── CMakeLists.txt   # Test configuration
    └── test_pfadfinder.cpp  # Test cases with CATCH2
```

## Author

Martin Fehrs
