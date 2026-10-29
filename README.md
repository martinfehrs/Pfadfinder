[🇩🇪 Deutsch (LIESMICH.md)](LIESMICH.md)

# Pfadfinder

Cross-platform project for determining common application directories.

## Description

Using an instance of the `pfadfinder::application_environment` class, you get access to the following operating system-dependent paths and their subpaths.

### Windows

| Method | Path |
|--------|------|
| `executable_path()` | Full path to the executable |
| `find_executable_path()` | Like `executable_path()`, returns `std::optional` |
| `executable_dir()` | Directory containing the executable |
| `find_executable_dir()` | Like `executable_dir()`, returns `std::optional` |
| `static_data_dir()` | Binary directory |
| `find_static_data_dir()` | Like `static_data_dir()`, returns `std::optional` |
| `shared_config_dir()` | `%ALLUSERSAPPDATA%\<appname>` |
| `find_shared_config_dir()` | Like `shared_config_dir()`, returns `std::optional` |
| `create_shared_config_dir()` | Creates `%ALLUSERSAPPDATA%\<appname>` |
| `data_dir()` | `%APPDATA%\<appname>` |
| `find_data_dir()` | Like `data_dir()`, returns `std::optional` |
| `create_data_dir()` | Creates `%APPDATA%\<appname>` |
| `user_config_dir()` | `%APPDATA%\<appname>` |
| `find_user_config_dir()` | Like `user_config_dir()`, returns `std::optional` |
| `create_user_config_dir()` | Creates `%APPDATA%\<appname>` |
| `user_config_file()` | `%APPDATA%\<appname>\<file>` (with file existence check) |
| `find_user_config_file()` | Like `user_config_file()`, returns `std::optional` |
| `shared_config_file()` | `%ALLUSERSAPPDATA%\<appname>\<file>` (with file existence check) |
| `find_shared_config_file()` | Like `shared_config_file()`, returns `std::optional` |
| `find_config_file()` | Searches user then shared config file (returns `std::optional`) |
| `config_files()` | Returns all available config files (0, 1 or 2) |
| `cache_dir()` | `%LOCALAPPDATA%\<appname>\Cache` |
| `find_cache_dir()` | Like `cache_dir()`, returns `std::optional` |
| `create_cache_dir()` | Creates `%LOCALAPPDATA%\<appname>\Cache` |
| `log_dir()` | `%LOCALAPPDATA%\<appname>\Logs` |
| `find_log_dir()` | Like `log_dir()`, returns `std::optional` |
| `create_log_dir()` | Creates `%LOCALAPPDATA%\<appname>\Logs` |
| `temp_dir()` | `%TEMP%\<appname>` |
| `find_temp_dir()` | Like `temp_dir()`, returns `std::optional` |
| `create_temp_dir()` | Creates `%TEMP%\<appname>` |
| `home_dir()` | `%USERPROFILE%` |
| `find_home_dir()` | Like `home_dir()`, returns `std::optional` |

### macOS

| Method | Bundle | CLI |
|--------|--------|-----|
| `executable_path()` | Full path to the executable | Full path to the executable |
| `find_executable_path()` | Like `executable_path()`, returns `std::optional` | Like `executable_path()`, returns `std::optional` |
| `executable_dir()` | Directory containing the executable | Directory containing the executable |
| `find_executable_dir()` | Like `executable_dir()`, returns `std::optional` | Like `executable_dir()`, returns `std::optional` |
| `static_data_dir()` | `Resources` directory | Derived from binary path |
| `find_static_data_dir()` | Like `static_data_dir()`, returns `std::optional` | Like `static_data_dir()`, returns `std::optional` |
| `shared_config_dir()` | `/Library/Preferences/<appname>` | `/Library/Preferences/<appname>` |
| `find_shared_config_dir()` | Like `shared_config_dir()`, returns `std::optional` | Like `shared_config_dir()`, returns `std::optional` |
| `create_shared_config_dir()` | Creates `/Library/Preferences/<appname>` | Creates `/Library/Preferences/<appname>` |
| `data_dir()` | `~/Library/Application Support/<appname>` | `~/.local/share/<appname>` |
| `find_data_dir()` | Like `data_dir()`, returns `std::optional` | Like `data_dir()`, returns `std::optional` |
| `create_data_dir()` | Creates `~/Library/Application Support/<appname>` | Creates `~/.local/share/<appname>` |
| `user_config_dir()` | `~/Library/Preferences/<appname>` | `~/.config/<appname>` |
| `find_user_config_dir()` | Like `user_config_dir()`, returns `std::optional` | Like `user_config_dir()`, returns `std::optional` |
| `create_user_config_dir()` | Creates `~/Library/Preferences/<appname>` | Creates `~/.config/<appname>` |
| `user_config_file()` | `~/Library/Preferences/<appname>/<file>` (with file existence check) | `~/.config/<appname>/<file>` (with file existence check) |
| `find_user_config_file()` | Like `user_config_file()`, returns `std::optional` | Like `user_config_file()`, returns `std::optional` |
| `shared_config_file()` | `/Library/Preferences/<appname>/<file>` (with file existence check) | `/Library/Preferences/<appname>/<file>` (with file existence check) |
| `find_shared_config_file()` | Like `shared_config_file()`, returns `std::optional` | Like `shared_config_file()`, returns `std::optional` |
| `find_config_file()` | Searches user then shared config file (returns `std::optional`) | Searches user then shared config file (returns `std::optional`) |
| `config_files()` | Returns all available config files (0, 1 or 2) | Returns all available config files (0, 1 or 2) |
| `cache_dir()` | `~/Library/Caches/<appname>` | `~/.cache/<appname>` |
| `find_cache_dir()` | Like `cache_dir()`, returns `std::optional` | Like `cache_dir()`, returns `std::optional` |
| `create_cache_dir()` | Creates `~/Library/Caches/<appname>` | Creates `~/.cache/<appname>` |
| `log_dir()` | `~/Library/Logs/<appname>` | `~/.local/state/<appname>/log` |
| `find_log_dir()` | Like `log_dir()`, returns `std::optional` | Like `log_dir()`, returns `std::optional` |
| `create_log_dir()` | Creates `~/Library/Logs/<appname>` | Creates `~/.local/state/<appname>/log` |
| `temp_dir()` | `~/Library/Caches/TemporaryItems/<appname>` | `/tmp/<appname>` |
| `find_temp_dir()` | Like `temp_dir()`, returns `std::optional` | Like `temp_dir()`, returns `std::optional` |
| `create_temp_dir()` | Creates `~/Library/Caches/TemporaryItems/<appname>` | Creates `/tmp/<appname>` |
| `home_dir()` | `$HOME` | `$HOME` |
| `find_home_dir()` | Like `home_dir()`, returns `std::optional` | Like `home_dir()`, returns `std::optional` |

### Linux

| Method | Path |
|--------|------|
| `executable_path()` | Full path to the executable |
| `find_executable_path()` | Like `executable_path()`, returns `std::optional` |
| `executable_dir()` | Directory containing the executable |
| `find_executable_dir()` | Like `executable_dir()`, returns `std::optional` |
| `static_data_dir()` | Derived from binary path (e.g., `/usr/bin/myapp` → `/usr/share/myapp`) |
| `find_static_data_dir()` | Like `static_data_dir()`, returns `std::optional` |
| `shared_config_dir()` | `/etc/<appname>` |
| `find_shared_config_dir()` | Like `shared_config_dir()`, returns `std::optional` |
| `create_shared_config_dir()` | Creates `/etc/<appname>` |
| `data_dir()` | `~/.local/share/<appname>` |
| `find_data_dir()` | Like `data_dir()`, returns `std::optional` |
| `create_data_dir()` | Creates `~/.local/share/<appname>` |
| `user_config_dir()` | `~/.config/<appname>` |
| `find_user_config_dir()` | Like `user_config_dir()`, returns `std::optional` |
| `create_user_config_dir()` | Creates `~/.config/<appname>` |
| `user_config_file()` | `~/.config/<appname>/<file>` (with file existence check) |
| `find_user_config_file()` | Like `user_config_file()`, returns `std::optional` |
| `shared_config_file()` | `/etc/<appname>/<file>` (with file existence check) |
| `find_shared_config_file()` | Like `shared_config_file()`, returns `std::optional` |
| `find_config_file()` | Searches user then shared config file (returns `std::optional`) |
| `config_files()` | Returns all available config files (0, 1 or 2) |
| `cache_dir()` | `~/.cache/<appname>` |
| `find_cache_dir()` | Like `cache_dir()`, returns `std::optional` |
| `create_cache_dir()` | Creates `~/.cache/<appname>` |
| `log_dir()` | `~/.local/state/<appname>/log` |
| `find_log_dir()` | Like `log_dir()`, returns `std::optional` |
| `create_log_dir()` | Creates `~/.local/state/<appname>/log` |
| `temp_dir()` | `/tmp/<appname>` or system temp directory |
| `find_temp_dir()` | Like `temp_dir()`, returns `std::optional` |
| `create_temp_dir()` | Creates `/tmp/<appname>` or system temp directory |
| `home_dir()` | `$HOME` |
| `find_home_dir()` | Like `home_dir()`, returns `std::optional` |

## Documentation in different languages

- [🇬🇧 English Documentation](https://martinfehrs.github.io/Pfadfinder/en/)
- [🇩🇪 Deutsche Dokumentation](https://martinfehrs.github.io/Pfadfinder/de/)

## Requirements

- C++20
- CMake 3.28 or higher
- Compiler with C++ module support (GCC 15, Clang 19, MSVC 19.40+)

## Build

First, create Conan profile:

```bash
conan profile detect --force
```

Then install dependencies and build the project:

### On macOS with Clang:

**Note:** Apple Clang currently does not have sufficient C++ module support. Therefore, LLVM Clang is used.

First, install LLVM:

```bash
brew install llvm@22
```

Then set environment variables:

```bash
LLVM_PREFIX=$(brew --prefix llvm@22)
export CC=$LLVM_PREFIX/bin/clang
export CXX=$LLVM_PREFIX/bin/clang++
export PATH="$LLVM_PREFIX/bin:$PATH"
```

Then run Conan and CMake:

```bash
conan install . --build=missing -s:a compiler=clang -s:a compiler.version=22 -s:a compiler.libcxx=libc++ -s:a compiler.cppstd=20 -s:a build_type=Release -c tools.cmake.cmaketoolchain:generator="Ninja"
cmake --preset=conan-release
cmake --build --preset=conan-release
```

### On Linux:

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -s:a build_type=Release -c tools.cmake.cmaketoolchain:generator="Ninja"
cmake --preset=conan-release
cmake --build --preset=conan-release
```

### On Windows with Visual Studio 2022:

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -c tools.cmake.cmaketoolchain:generator="Visual Studio 17 2022"
cmake --preset=conan-default
cmake --build --preset=conan-release
```

## Usage examples

### Without customizations

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
    std::println(stderr, "Error: {}", e.what());
    return 1;    
}
```

### With custom application name

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
    std::println(stderr, "Error: {}", e.what());
    return 1;    
}
```

### With custom system environment

```cpp
import std;
import pfadfinder;

namespace fs = std::filesystem;

struct my_custom_environment : pfadfinder::default_system_environment
{
    [[nodiscard]] std::optional<fs::path> user_config_dir(const fs::path&, const std::string& app_name) const override
    {
        auto home = default_system_environment::home_dir();
        if (!home.has_value())
            return std::nullopt;
        return *home / std::format(".{}", app_name);
    }

    [[nodiscard]] std::optional<fs::path> data_dir(const fs::path&, const std::string& app_name) const override
    {
        auto home = default_system_environment::home_dir();
        if (!home.has_value())
            return std::nullopt;
        return *home / std::format(".{}", app_name);
    }

    [[nodiscard]] std::optional<fs::path> cache_dir(const fs::path&, const std::string& app_name) const override
    {
        auto home = default_system_environment::home_dir();
        if (!home.has_value())
            return std::nullopt;
        return *home / std::format(".{}", app_name);
    }
};

int main() try
{
    pfadfinder::application_environment env{ my_custom_environment{} };

    std::println("Executable:    {}", env.executable_path().string());
    std::println("Home Dir:      {}", env.home_dir().string());
    std::println("Data Dir:      {}", env.data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Error: {}", e.what());
    return 1;    
}
```

## Run tests

```bash
cmake --build --preset=conan-release --target test
```

## Author

Martin Fehrs
