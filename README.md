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
- `file_not_found` - File not found
- `directory_not_found` - Directory not found

All exceptions are derived from `pfadfinder::error`, which in turn is
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
- **Windows:** Returns `<executable_directory>\\<appname>` (e.g. `C:\\App\\my_app`)
- **Linux:** Derives the share directory from the binary directory
  (e.g. `/usr/share/my_app`)
- **macOS Bundle:** Returns the Resources directory
  (e.g. `MyApp.app/Contents/Resources/`)
- **macOS CLI:** Similar to Linux (e.g. `/usr/local/share/my_app`)

**Return value:** `fs::path` - The data directory.
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `user_data_directory()`
Returns the user-specific data directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\\<appname>`
  (e.g. `C:\\Users\\User\\AppData\\Roaming\\my_app`)
- **Linux:** Returns `~/.local/share/<appname>`
  (e.g. `/home/user/.local/share/my_app`)
- **macOS Bundle:** Returns `~/Library/Application Support/<appname>`
- **macOS CLI:** Returns `~/.local/share/<appname>`

**Return value:** `fs::path` - The user data directory.
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `create_user_data_directory()`
Creates the user-specific data directory if it does not exist and returns it.

**Platform-specific behavior:**
- **Windows:** Creates `%APPDATA%\\<appname>`
- **Linux:** Creates `~/.local/share/<appname>`
- **macOS Bundle:** Creates `~/Library/Application Support/<appname>`
- **macOS CLI:** Creates `~/.local/share/<appname>`

**Return value:** `fs::path` - The user data directory.

#### `config_directory()`
Returns the configuration directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\\<appname>`
- **Linux:** Returns `~/.config/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Preferences/<appname>`
- **macOS CLI:** Returns `~/.config/<appname>`

**Return value:** `fs::path` - The configuration directory.
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `create_config_directory()`
Creates the configuration directory if it does not exist and returns it.

**Platform-specific behavior:**
- **Windows:** Creates `%APPDATA%\\<appname>`
- **Linux:** Creates `~/.config/<appname>`
- **macOS Bundle:** Creates `~/Library/Preferences/<appname>`
- **macOS CLI:** Creates `~/.config/<appname>`

**Return value:** `fs::path` - The configuration directory.

#### `cache_directory()`
Returns the cache directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\\<appname>\\Cache`
- **Linux:** Returns `~/.cache/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Caches/<appname>`
- **macOS CLI:** Returns `~/.cache/<appname>`

**Return value:** `fs::path` - The cache directory.
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `create_cache_directory()`
Creates the cache directory if it does not exist and returns it.

**Platform-specific behavior:**
- **Windows:** Creates `%LOCALAPPDATA%\\<appname>\\Cache`
- **Linux:** Creates `~/.cache/<appname>`
- **macOS Bundle:** Creates `~/Library/Caches/<appname>`
- **macOS CLI:** Creates `~/.cache/<appname>`

**Return value:** `fs::path` - The cache directory.

#### `log_directory()`
Returns the log directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\\<appname>\\Logs`
- **Linux:** Returns `~/.local/state/<appname>/log` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Logs/<appname>`
- **macOS CLI:** Returns `~/.local/state/<appname>/log`

**Return value:** `fs::path` - The log directory.
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `create_log_directory()`
Creates the log directory if it does not exist and returns it.

**Platform-specific behavior:**
- **Windows:** Creates `%LOCALAPPDATA%\\<appname>\\Logs`
- **Linux:** Creates `~/.local/state/<appname>/log`
- **macOS Bundle:** Creates `~/Library/Logs/<appname>`
- **macOS CLI:** Creates `~/.local/state/<appname>/log`

**Return value:** `fs::path` - The log directory.

#### `temp_directory()`
Returns the temporary directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%TEMP%\\<appname>`
- **Linux:** Returns `/tmp/<appname>`
- **macOS Bundle:** Returns `~/Library/Caches/TemporaryItems/<appname>`
- **macOS CLI:** Returns `/tmp/<appname>`

**Return value:** `fs::path` - The temporary directory.
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `create_temp_directory()`
Creates the temporary directory if it does not exist and returns it.

**Platform-specific behavior:**
- **Windows:** Creates `%TEMP%\\<appname>`
- **Linux:** Creates `/tmp/<appname>`
- **macOS Bundle:** Creates `~/Library/Caches/TemporaryItems/<appname>`
- **macOS CLI:** Creates `/tmp/<appname>`

**Return value:** `fs::path` - The temporary directory.

#### `data_file(const fs::path& rel_path)`
Returns the absolute path to a file in the data directory.

Searches for the file specified by `rel_path` in the directory returned by `data_directory()`.

**Parameters:**
- `rel_path`: Relative path to the file within the data directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `user_data_file(const fs::path& rel_path)`
Returns the absolute path to a file in the user data directory.

Searches for the file specified by `rel_path` in the directory returned by `user_data_directory()`.

**Parameters:**
- `rel_path`: Relative path to the file within the user data directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `cache_file(const fs::path& rel_path)`
Returns the absolute path to a file in the cache directory.

Searches for the file specified by `rel_path` in the directory returned by `cache_directory()`.

**Parameters:**
- `rel_path`: Relative path to the file within the cache directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `log_file(const fs::path& rel_path)`
Returns the absolute path to a file in the log directory.

Searches for the file specified by `rel_path` in the directory returned by `log_directory()`.

**Parameters:**
- `rel_path`: Relative path to the file within the log directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `temp_file(const fs::path& rel_path)`
Returns the absolute path to a file in the temporary directory.

Searches for the file specified by `rel_path` in the directory returned by `temp_directory()`.

**Parameters:**
- `rel_path`: Relative path to the file within the temporary directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

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
        std::println("User Dir: {}", env.user_directory().string());
        
        // For directories that may not exist, use create_*_directory()
        auto user_data_dir = env.create_user_data_directory();
        std::println("User Data Dir: {}", user_data_dir.string());
        
        auto config_dir = env.create_config_directory();
        std::println("Config Dir: {}", config_dir.string());
        
        auto cache_dir = env.create_cache_directory();
        std::println("Cache Dir: {}", cache_dir.string());
        
        auto log_dir = env.create_log_directory();
        std::println("Log Dir: {}", log_dir.string());
        
        auto temp_dir = env.create_temp_directory();
        std::println("Temp Dir: {}", temp_dir.string());
        
        // data_directory() throws if the directory does not exist
        try
        {
            std::println("Data Dir: {}", env.data_directory().string());
        }
        catch (const pfadfinder::directory_not_found&)
        {
            std::println("Data directory does not exist (read-only location)");
        }
    }
    catch (const pfadfinder::error& e)
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
