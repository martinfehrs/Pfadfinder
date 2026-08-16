[🇩🇪 Deutsch (LIESMICH.md)](LIESMICH.md)

# Pfadfinder

Cross-platform project for determining common application directories.

## Description

Using an instance of the `pfadfinder::application_environment` class, you get access to the following operating system-dependent paths and their subpaths.

### Windows

| Method | Path |
|--------|------|
| `executable_path()` | Full path to the executable |
| `executable_dir()` | Directory containing the executable |
| `static_data_dir()` | Binary directory |
| `shared_data_dir()` | `%ALLUSERSAPPDATA%\<appname>` |
| `shared_config_dir()` | `%ALLUSERSAPPDATA%\<appname>` |
| `shared_cache_dir()` | `%ALLUSERSAPPDATA%\<appname>\Cache` |
| `shared_log_dir()` | `%ALLUSERSAPPDATA%\<appname>\Logs` |
| `user_data_dir()` | `%APPDATA%\<appname>` |
| `user_config_dir()` | `%APPDATA%\<appname>` |
| `user_cache_dir()` | `%LOCALAPPDATA%\<appname>\Cache` |
| `user_log_dir()` | `%LOCALAPPDATA%\<appname>\Logs` |
| `temp_dir()` | `%TEMP%\<appname>` |
| `user_dir()` | `%USERPROFILE%` |

### macOS

| Method | Bundle | CLI |
|--------|--------|-----|
| `executable_path()` | Full path to the executable | Full path to the executable |
| `executable_dir()` | Directory containing the executable | Directory containing the executable |
| `static_data_dir()` | `Resources` directory | Derived from binary path |
| `shared_data_dir()` | `/Library/Application Support/<appname>` | `/Library/Application Support/<appname>` |
| `shared_config_dir()` | `/Library/Preferences/<appname>` | `/Library/Preferences/<appname>` |
| `shared_cache_dir()` | `/Library/Caches/<appname>` | `/Library/Caches/<appname>` |
| `shared_log_dir()` | `/Library/Logs/<appname>` | `/Library/Logs/<appname>` |
| `user_data_dir()` | `~/Library/Application Support/<appname>` | `~/.local/share/<appname>` |
| `user_config_dir()` | `~/Library/Preferences/<appname>` | `~/.config/<appname>` |
| `user_cache_dir()` | `~/Library/Caches/<appname>` | `~/.cache/<appname>` |
| `user_log_dir()` | `~/Library/Logs/<appname>` | `~/.local/state/<appname>/log` |
| `temp_dir()` | `~/Library/Caches/TemporaryItems/<appname>` | `/tmp/<appname>` |
| `user_dir()` | `$HOME` | `$HOME` |

### Linux

| Method | Path |
|--------|------|
| `executable_path()` | Full path to the executable |
| `executable_dir()` | Directory containing the executable |
| `static_data_dir()` | Derived from binary path (e.g., `/usr/bin/myapp` → `/usr/share/myapp`) |
| `shared_data_dir()` | `/var/lib/<appname>` |
| `shared_config_dir()` | `/etc/<appname>` |
| `shared_cache_dir()` | `/var/cache/<appname>` |
| `shared_log_dir()` | `/var/log/<appname>` |
| `user_data_dir()` | `~/.local/share/<appname>` (XDG standard) |
| `user_config_dir()` | `~/.config/<appname>` (XDG standard) |
| `user_cache_dir()` | `~/.cache/<appname>` (XDG standard) |
| `user_log_dir()` | `~/.local/state/<appname>/log` (XDG Base Directory Specification) |
| `temp_dir()` | `/tmp/<appname>` or system temp directory |
| `user_dir()` | `$HOME` |

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
conan install . --build=missing -s:a compiler=clang -s:a compiler.version=22 -s:a compiler.libcxx=libc++ -s:a compiler.cppstd=20 -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
cd Pfadfinder
mkdir -p build
cd build
cmake --preset=conan-default
cmake --build --preset=conan-release
```

### On Linux:

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
cd Pfadfinder
mkdir -p build
cd build
cmake --preset=conan-default
cmake --build --preset=conan-release
```

### On Windows with Visual Studio 2022:

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -c tools.cmake.cmaketoolchain:generator="Visual Studio 17 2022"
cd Pfadfinder
mkdir -p build
cd build
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
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.user_cache_dir().string());
 
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
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.user_cache_dir().string());
 
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
    [[nodiscard]] fs::path user_config_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }

    [[nodiscard]] fs::path user_data_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }

    [[nodiscard]] fs::path user_cache_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }
};

int main() try
{
    pfadfinder::application_environment env{ my_custom_environment{} };

    std::println("Executable:    {}", env.executable_path().string());
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.user_config_dir().string());
    std::println("Cache Dir:     {}", env.user_cache_dir().string());
 
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
cd build
./tests/test_pfadfinder
```

## Author

Martin Fehrs
