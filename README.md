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

- `home_not_set`                - HOME environment variable not set
- `readlink_failed`             - readlink /proc/self/exe failed (Linux)
- `appdata_not_set`             - APPDATA environment variable not set (Windows)
- `localappdata_not_set`        - LOCALAPPDATA environment variable not set (Windows)
- `allusersappdata_not_set`     - ALLUSERSAPPDATA environment variable not set (Windows)
- `get_module_file_name_failed` - GetModuleFileNameW failed (Windows)
- `get_executable_path_failed`  - _NSGetExecutablePath failed (macOS)
- `realpath_failed`             - realpath failed (macOS)
- `file_not_found`              - File not found
- `directory_not_found`         - Directory not found

All exceptions are derived from `pfadfinder::error`, which in turn is
derived from `std::runtime_error`.

### Methods

#### `executable_path()`
Returns the full path to the executable file.

**Return value:** `fs::path` - The full path to the executable file.
**Exceptions:** Platform-specific exceptions (see Error Handling).

#### `executable_dir()`
Returns the directory containing the executable file.

**Return value:** `fs::path` - The directory of the executable file.

#### `static_data_dir(const fs::path& rel_path = "")`
Returns the system-wide static data directory of the application.

**Parameters:**
- `rel_path`: Relative path to the base directory (optional).

**Platform-specific behavior:**
- **Windows:** Returns `<executable_dir>\\<appname>` (e.g. `C:\\App\\my_app`)
- **Linux:** Derives the share directory from the binary directory
  (e.g. `/usr/share/my_app`)
- **macOS Bundle:** Returns the Resources directory
  (e.g. `MyApp.app/Contents/Resources/`)
- **macOS CLI:** Similar to Linux (e.g. `/usr/local/share/my_app`)

**Return value:** `fs::path` - The static data directory (base or base + rel_path).
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `shared_data_dir(const fs::path& rel_path = "")`
Returns the system-wide shared data directory of the application.

**Parameters:**
- `rel_path`: Relative path to the base directory (optional).

**Platform-specific behavior:**
- **Windows:** Returns `%ALLUSERSAPPDATA%\\<appname>`
- **Linux:** Returns `/var/lib/<appname>`
- **macOS:** Returns `/Library/Application Support/<appname>`

**Return value:** `fs::path` - The shared data directory (base or base + rel_path).
**Exceptions:** `directory_not_found` - If the directory does not exist.


#### `user_data_dir` (overloaded)
Returns the user-specific data directory of the application. Optionally creates the directory if it does not exist.

**Overloads:**

1. **`user_data_dir(bool create_dir = true)`**
   
   Returns the base user data directory.

   **Parameters:**
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%APPDATA%\<appname>`
     (e.g. `C:\\Users\\User\\AppData\\Roaming\\my_app`)
   - **Linux:** Returns `~/.local/share/<appname>`
     (e.g. `/home/user/.local/share/my_app`)
   - **macOS Bundle:** Returns `~/Library/Application Support/<appname>`
   - **macOS CLI:** Returns `~/.local/share/<appname>`

   **Return value:** `fs::path` - The user data directory.
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

2. **`user_data_dir(const fs::path& rel_path, bool create_dir = true)`**
   
   Returns the user data directory including subpath. Optionally creates the directory if it does not exist.

   **Parameters:**
   - `rel_path`: Relative path to the base directory.
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%APPDATA%\<appname>\<rel_path>`
   - **Linux:** Returns `~/.local/share/<appname>/<rel_path>`
   - **macOS Bundle:** Returns `~/Library/Application Support/<appname>/<rel_path>`
   - **macOS CLI:** Returns `~/.local/share/<appname>/<rel_path>`

   **Return value:** `fs::path` - The user data directory (base + rel_path).

#### `config_dir` (overloaded)
Returns the configuration directory of the application. Optionally creates the directory if it does not exist.

**Overloads:**

1. **`config_dir(bool create_dir = true)`**
   
   Returns the base configuration directory.

   **Parameters:**
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%APPDATA%\<appname>`
   - **Linux:** Returns `~/.config/<appname>` (XDG Base Directory Specification)
   - **macOS Bundle:** Returns `~/Library/Preferences/<appname>`
   - **macOS CLI:** Returns `~/.config/<appname>`

   **Return value:** `fs::path` - The configuration directory.
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

2. **`config_dir(const fs::path& rel_path, bool create_dir = true)`**
   
   Returns the configuration directory including subpath. Optionally creates the directory if it does not exist.

   **Parameters:**
   - `rel_path`: Relative path to the base directory.
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%APPDATA%\<appname>\<rel_path>`
   - **Linux:** Returns `~/.config/<appname>/<rel_path>`
   - **macOS Bundle:** Returns `~/Library/Preferences/<appname>/<rel_path>`
   - **macOS CLI:** Returns `~/.config/<appname>/<rel_path>`

   **Return value:** `fs::path` - The configuration directory (base + rel_path).
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

**Return value:** `fs::path` - The configuration directory (base + rel_path).


#### `cache_dir` (overloaded)
Returns the cache directory of the application. Optionally creates the directory if it does not exist.

**Overloads:**

1. **`cache_dir(bool create_dir = true)`**
   
   Returns the base cache directory.

   **Parameters:**
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%LOCALAPPDATA%\<appname>\Cache`
   - **Linux:** Returns `~/.cache/<appname>` (XDG Base Directory Specification)
   - **macOS Bundle:** Returns `~/Library/Caches/<appname>`
   - **macOS CLI:** Returns `~/.cache/<appname>`

   **Return value:** `fs::path` - The cache directory.
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

2. **`cache_dir(const fs::path& rel_path, bool create_dir = true)`**
   
   Returns the cache directory including subpath. Optionally creates the directory if it does not exist.

   **Parameters:**
   - `rel_path`: Relative path to the base directory.
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%LOCALAPPDATA%\<appname>\Cache\<rel_path>`
   - **Linux:** Returns `~/.cache/<appname>/<rel_path>`
   - **macOS Bundle:** Returns `~/Library/Caches/<appname>/<rel_path>`
   - **macOS CLI:** Returns `~/.cache/<appname>/<rel_path>`

   **Return value:** `fs::path` - The cache directory (base + rel_path).
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

**Return value:** `fs::path` - The cache directory (base + rel_path).


#### `log_dir` (overloaded)
Returns the log directory of the application. Optionally creates the directory if it does not exist.

**Overloads:**

1. **`log_dir(bool create_dir = true)`**
   
   Returns the base log directory.

   **Parameters:**
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%LOCALAPPDATA%\<appname>\Logs`
   - **Linux:** Returns `~/.local/state/<appname>/log` (XDG Base Directory Specification)
   - **macOS Bundle:** Returns `~/Library/Logs/<appname>`
   - **macOS CLI:** Returns `~/.local/state/<appname>/log`

   **Return value:** `fs::path` - The log directory.
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

2. **`log_dir(const fs::path& rel_path, bool create_dir = true)`**
   
   Returns the log directory including subpath. Optionally creates the directory if it does not exist.

   **Parameters:**
   - `rel_path`: Relative path to the base directory.
   - `create_dir`: Whether to create the directory if it does not exist (Default: `true`).

   **Platform-specific behavior:**
   - **Windows:** Returns `%LOCALAPPDATA%\<appname>\Logs\<rel_path>`
   - **Linux:** Returns `~/.local/state/<appname>/log/<rel_path>`
   - **macOS Bundle:** Returns `~/Library/Logs/<appname>/<rel_path>`
   - **macOS CLI:** Returns `~/.local/state/<appname>/log/<rel_path>`

   **Return value:** `fs::path` - The log directory (base + rel_path).
   **Exceptions:** `directory_not_found` - If the directory does not exist and `create_dir=false`.

- **macOS CLI:** Creates `~/.local/state/<appname>/log/<rel_path>`

**Return value:** `fs::path` - The log directory (base + rel_path).

#### `temp_dir(const fs::path& rel_path = "")`
Returns the temporary directory of the application.

**Parameters:**
- `rel_path`: Relative path to the base directory (optional).

**Platform-specific behavior:**
- **Windows:** Returns `%TEMP%\\<appname>`
- **Linux:** Returns `/tmp/<appname>`
- **macOS Bundle:** Returns `~/Library/Caches/TemporaryItems/<appname>`
- **macOS CLI:** Returns `/tmp/<appname>`

**Return value:** `fs::path` - The temporary directory (base or base + rel_path).
**Exceptions:** `directory_not_found` - If the directory does not exist.

#### `create_temp_dir(const fs::path& rel_path = "")`
Creates the temporary directory (incl. rel_path) if it does not exist and returns it.

**Parameters:**
- `rel_path`: Relative path to the base directory (optional).

**Platform-specific behavior:**
- **Windows:** Creates `%TEMP%\\<appname>\\<rel_path>`
- **Linux:** Creates `/tmp/<appname>/<rel_path>`
- **macOS Bundle:** Creates `~/Library/Caches/TemporaryItems/<appname>/<rel_path>`
- **macOS CLI:** Creates `/tmp/<appname>/<rel_path>`

**Return value:** `fs::path` - The temporary directory (base + rel_path).

#### `static_data_file(const fs::path& rel_path)`
Returns the absolute path to a file in the static data directory.

Searches for the file specified by `rel_path` in the directory returned by `static_data_dir()`.

**Parameters:**
- `rel_path`: Relative path to the file within the static data directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `user_data_file(const fs::path& rel_path)`
Returns the absolute path to a file in the user data directory.

Searches for the file specified by `rel_path` in the directory returned by `user_data_dir()`.

**Parameters:**
- `rel_path`: Relative path to the file within the user data directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `cache_file(const fs::path& rel_path)`
Returns the absolute path to a file in the cache directory.

Searches for the file specified by `rel_path` in the directory returned by `cache_dir()`.

**Parameters:**
- `rel_path`: Relative path to the file within the cache directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `log_file(const fs::path& rel_path)`
Returns the absolute path to a file in the log directory.

Searches for the file specified by `rel_path` in the directory returned by `log_dir()`.

**Parameters:**
- `rel_path`: Relative path to the file within the log directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `temp_file(const fs::path& rel_path)`
Returns the absolute path to a file in the temporary directory.

Searches for the file specified by `rel_path` in the directory returned by `temp_dir()`.

**Parameters:**
- `rel_path`: Relative path to the file within the temporary directory.

**Return value:** `fs::path` - Absolute path to the file.

**Exceptions:**
- `file_not_found`: If the file was not found.

#### `user_dir()`
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
        std::println("Executable Dir: {}", env.executable_dir().string());
        std::println("User Dir: {}", env.user_dir().string());
        
        // For directories that may not exist, use create_dir=true (default)
        auto user_data_dir = env.user_data_dir();
        std::println("User Data Dir: {}", user_data_dir.string());
        
        auto config_dir = env.config_dir();
        std::println("Config Dir: {}", config_dir.string());
        
        auto cache_dir = env.cache_dir();
        std::println("Cache Dir: {}", cache_dir.string());
        
        auto log_dir = env.log_dir();
        std::println("Log Dir: {}", log_dir.string());
        
        auto temp_dir = env.create_temp_dir();
        std::println("Temp Dir: {}", temp_dir.string());
        
        // static_data_dir() throws if the directory does not exist
        std::println("Data Dir: {}", env.static_data_dir().string());
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
