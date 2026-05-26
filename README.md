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
import std;
import pfadfinder;

pfadfinder::application_environment env("my_app");
```

**Parameter:**
- `app_name` (required): The name of the application, used for all directory paths.

### Error Handling

All path functions return `std::expected<fs::path, error>`, where `error` is an enum
class that describes platform-specific error cases:

- `platform_not_supported` - Platform is not supported
- `windows_get_module_file_name_failed` - GetModuleFileNameW failed
- `appdata_not_set` - APPDATA environment variable not set
- `localappdata_not_set` - LOCALAPPDATA environment variable not set
- `macos_get_executable_path_failed` - _NSGetExecutablePath failed
- `macos_realpath_failed` - realpath failed
- `linux_readlink_failed` - readlink /proc/self/exe failed
- `home_not_set` - HOME environment variable not set

Use `pfadfinder::error_message(error err)` to get a human-readable error description.

### Methods

#### `executable_path()`
Returns the full path to the executable file.

**Return value:** `std::expected<fs::path, error>` - The full path to the
executable file or an error code.

**Example:**
```cpp
import std;
import pfadfinder;

pfadfinder::application_environment env("my_app");
auto result = env.executable_path();
if (result)
{
    // Success
    cout << "Executable: " << result->string() << endl;
    // Linux: /usr/bin/test_pfadfinder
    // Windows: C:\\Projects\\Pfadfinder\\build\\tests\\test_pfadfinder.exe
    // macOS: /Users/martin/Projects/Pfadfinder/build/tests/test_pfadfinder
}
else
{
    // Error
    cerr << "Error: " << pfadfinder::error_message(result.error()) << endl;
}
```

#### `executable_directory()`
Returns the directory containing the executable file.

**Return value:** `std::expected<fs::path, error>` - The directory of the
executable file or an error code.

**Example:**
```cpp
import std;
import pfadfinder;

pfadfinder::application_environment env("my_app");
auto result = env.executable_directory();
if (result)
{
    cout << "Executable Dir: " << result->string() << endl;
}
else
{
    cerr << "Error: " << pfadfinder::error_message(result.error()) << endl;
}
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

**Return value:** `std::expected<fs::path, error>` - The data directory
or an error code.

#### `user_data_directory()`
Returns the user-specific data directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\<appname>`
  (e.g. `C:\\Users\\User\\AppData\\Roaming\\my_app`)
- **Linux:** Returns `~/.local/share/<appname>`
  (e.g. `/home/user/.local/share/my_app`)
- **macOS Bundle:** Returns `~/Library/Application Support/<appname>`
- **macOS CLI:** Returns `~/.local/share/<appname>`

**Return value:** `std::expected<fs::path, error>` - The user data directory
or an error code.

#### `config_directory()`
Returns the configuration directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%APPDATA%\<appname>`
- **Linux:** Returns `~/.config/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Preferences/<appname>`
- **macOS CLI:** Returns `~/.config/<appname>`

**Return value:** `std::expected<fs::path, error>` - The configuration directory
or an error code.

#### `cache_directory()`
Returns the cache directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\<appname>\Cache`
- **Linux:** Returns `~/.cache/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Caches/<appname>`
- **macOS CLI:** Returns `~/.cache/<appname>`

**Return value:** `std::expected<fs::path, error>` - The cache directory
or an error code.

#### `log_directory()`
Returns the log directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%LOCALAPPDATA%\<appname>\Logs`
- **Linux:** Returns `~/.local/state/<appname>/log` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Logs/<appname>`
- **macOS CLI:** Returns `~/.local/state/<appname>/log`

**Return value:** `std::expected<fs::path, error>` - The log directory
or an error code.

#### `temp_directory()`
Returns the temporary directory of the application.

**Platform-specific behavior:**
- **Windows:** Returns `%TEMP%\<appname>`
- **Linux:** Returns `/tmp/<appname>` (XDG Base Directory Specification)
- **macOS Bundle:** Returns `~/Library/Caches/TemporaryItems/<appname>`
- **macOS CLI:** Returns `/tmp/<appname>`

**Return value:** `std::expected<fs::path, error>` - The temporary directory
or an error code.

#### `user_directory()`
Returns the user's home directory.

**Platform-specific behavior:**
- **Windows:** Returns `%USERPROFILE%`
- **Linux and macOS:** Returns `$HOME`

**Return value:** `std::expected<fs::path, error>` - The home directory
or an error code.

## Usage Example

```cpp
import std;
import pfadfinder;

int main()
{
    // Create an environment for the application "MyApp"
    pfadfinder::application_environment env("MyApp");
    
    // Determine various directories
    auto exec_result = env.executable_path();
    if (exec_result)
        cout << "Executable: " << exec_result->string() << endl;
    else
        cerr << "Error: " << pfadfinder::error_message(exec_result.error()) << endl;
    
    auto exec_dir_result = env.executable_directory();
    if (exec_dir_result)
        cout << "Executable Dir: " << exec_dir_result->string() << endl;
    else
        cerr << "Error: " << pfadfinder::error_message(exec_dir_result.error()) << endl;
    
    auto data_dir_result = env.data_directory();
    if (data_dir_result)
        cout << "Data Dir: " << data_dir_result->string() << endl;
    else
        cerr << "Error: " << pfadfinder::error_message(data_dir_result.error()) << endl;
    
    auto user_data_dir_result = env.user_data_directory();
    if (user_data_dir_result)
        cout << "User Data Dir: " << user_data_dir_result->string() << endl;
    else
        cerr << "Error: " << pfadfinder::error_message(user_data_dir_result.error()) << endl;
    
    auto config_dir_result = env.config_directory();
    if (config_dir_result)
        cout << "Config Dir: " << config_dir_result->string() << endl;
    else
        cerr << "Error: " << pfadfinder::error_message(config_dir_result.error()) << endl;
    
    auto cache_dir_result = env.cache_directory();
    if (cache_dir_result)
        cout << "Cache Dir: " << cache_dir_result->string() << endl;
    else
        cerr << "Error: " << pfadfinder::error_message(cache_dir_result.error()) << endl;
    
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
