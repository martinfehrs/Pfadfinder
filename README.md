[🇩🇪 Deutsch (LIESMICH.md)](LIESMICH.md)

# Pfadfinder

Project for determining various directories of a running application.

## Description

This project provides the `pfadfinder::application_environment` class, which offers
methods to determine various path directories for an application across different
platforms (Windows, Linux, macOS).

The implementation supports:
- Windows (using `GetModuleFileNameW`)
- macOS (using `_NSGetExecutablePath` and `realpath`)
- Linux (using `/proc/self/exe`)

## Class: `application_environment`

The main class of the module that provides all path functions as methods.

### Constructor

**Parameter:**
- `app_name` (optional): The name of the application, used for all directory paths.
  If not provided, the filename of the executable (without extension) is used.

### Error Handling

All path functions return `fs::path` and may throw exceptions if an error occurs.
The following exceptions may be thrown:

- `home_not_set` - HOME environment variable not set
- `readlink_failed` - readlink /proc/self/exe failed (Linux)
- `appdata_not_set` - APPDATA environment variable not set (Windows)
- `localappdata_not_set` - LOCALAPPDATA environment variable not set (Windows)
- `get_module_file_name_failed` - GetModuleFileNameW failed (Windows)
- `get_executable_path_failed` - _NSGetExecutablePath failed (macOS)
- `realpath_failed` - realpath failed (macOS)

All exceptions are derived from `pfadfinder::pathfinder_error`, which in turn is
derived from `std::runtime_error`.

### Methods

#### `executable_path()`
Returns the full path to the executable file.

**Return value:** `fs::path` - The full path to the executable file.
**Exceptions:** Platform-specific exceptions (see Error Handling).

#### `executable_directory()`
Returns the directory containing the executable file.

**Return value:** `fs::path` - The directory of the executable file.

#### `data_directory()`
Returns the system-wide data directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `<executable_directory>` (e.g. `C:\\App`)
- **Linux:** Derives the share directory from the binary directory
  (e.g. `/usr/share/my_app`)
- **macOS Bundle:** Returns the Resources directory
  (e.g. `MyApp.app/Contents/Resources/`)
- **macOS CLI:** Similar to Linux (e.g. `/usr/local/share/my_app`)

**Return value:** `fs::path` - The data directory.

#### `user_data_directory()`
Returns the user-specific data directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\<appname>`
  (e.g. `C:\\Users\\User\\AppData\\Roaming\\my_app`)
- **Linux:** Returns `~/.local/share/<appname>`
  (e.g. `/home/user/.local/share/my_app`)
- **macOS Bundle:** Returns `~/Library/Application Support/<appname>`
- **macOS CLI:** Returns `~/.local/share/<appname>`

**Return value:** `fs::path` - The user data directory.

#### `config_directory()`
Returns the configuration directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\<appname>`
- **Linux:** Returns `~/.config/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Preferences/<appname>`
- **macOS CLI:** Returns `~/.config/<appname>`

**Return value:** `fs::path` - The configuration directory.

#### `cache_directory()`
Returns the cache directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\<appname>\\Cache`
- **Linux:** Returns `~/.cache/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Caches/<appname>`
- **macOS CLI:** Returns `~/.cache/<appname>`

**Return value:** `fs::path` - The cache directory.

#### `log_directory()`
Returns the log directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\<appname>\\Logs`
- **Linux:** Returns `~/.local/state/<appname>/log` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Logs/<appname>`
- **macOS CLI:** Returns `~/.local/state/<appname>/log`

**Return value:** `fs::path` - The log directory.

#### `temp_directory()`
Returns the temporary directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%TEMP%\<appname>`
- **Linux:** Returns `/tmp/<appname>`
- **macOS Bundle:** Returns `~/Library/Caches/TemporaryItems/<appname>`
- **macOS CLI:** Returns `/tmp/<appname>`

**Return value:** `fs::path` - The temporary directory.

#### `user_directory()`
Returns the user's home directory.

**Platform-specific behavior:**
- **Windows:** Returns `%USERPROFILE%`
- **Linux and macOS:** Returns `$HOME`

**Return value:** `fs::path` - The home directory.

## Usage Example

```cpp
import std;
import pfadfinder;

int main()
{
    // Create an environment for the application "MyApp"
    pfadfinder::application_environment env("MyApp");
    
    try
    {
        // Determine various directories
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
    catch (const pfadfinder::pathfinder_error& e)
    {
        std::println(stderr, "Error: {}", e.what());
        return 1;
    }
    
    return 0;
}
```

## Requirements

- C++23
- CMake 3.28 or higher
- Compiler with C++-module support (GCC 15, Clang 19, MSVC 19.40+)

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
├── CMakeLists.txt                  # Main CMake configuration
├── LIESMICH.md                     # Projektbeschreibung (Deutsch)
├── README.md                       # Project description (English)
├── src/
│   ├── error.cppm                  # Error handling (exception classes)
│   ├── pfadfinder.cppm             # main module
│   ├── system_backend_linux.cppm   # Linux-specific implementation
│   ├── system_backend_windows.cppm # Windows-specific implementation
│   └── system_backend_macos.cppm   # macOS-specific implementation
└── tests/
    ├── CMakeLists.txt              # Test configuration
    └── test_pfadfinder.cpp         # Test cases with CATCH2
```

## Author

Martin Fehrs
