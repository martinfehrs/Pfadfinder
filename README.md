[🇩🇪 Deutsch (LIESMICH.md)](LIESMICH.md)

# Pfadfinder

Cross-platform project for determining common application directories.

## Description

Using an instance of the `pfadfinder::application_environment` class, you get access to the following operating system-dependent paths and their subpaths:

- User-specific configuration
- User-specific cache
- User-specific data
- System-wide configuration
- System-wide cache
- System-wide data
- Static application data
- User directory
- Temporary data

## Documentation

- [🇬🇧 English Documentation](https://martinfehrs.github.io/Pfadfinder/en/)
- [🇩🇪 Deutsche Dokumentation](https://martinfehrs.github.io/Pfadfinder/de/)

## Supported Platforms

- Windows
- macOS
- Linux

## Requirements

- C++23
- CMake 3.28 or higher
- Compiler with C++ module support (GCC 15, Clang 19, MSVC 19.40+)

## Build

### On macOS and Linux:

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G "Ninja Multi-Config" ..
cmake --build . --config Release
```

### On Windows with Visual Studio 2022:

```bash
cd Pfadfinder
mkdir -p build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## Usage Examples

### Without Customizations

```cpp
import std;
import pfadfinder;

int main() try
{
    pfadfinder::application_environment env{};

    std::println("Executable:    {}", env.executable_path().string());
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Error: {}", e.what());
    return 1;    
}
```

### With Custom Application Name

```cpp
import std;
import pfadfinder;

int main() try
{
    pfadfinder::application_environment env{ "my_app" };

    std::println("Executable:    {}", env.executable_path().string());
    std::println("User Dir:      {}", env.user_dir().string());
    std::println("User Data Dir: {}", env.user_data_dir().string());
    std::println("Config Dir:    {}", env.config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Error: {}", e.what());
    return 1;    
}
```

### With Custom System Environment

```cpp
import std;
import pfadfinder;

namespace fs = std::filesystem;

struct my_custom_environment : pfadfinder::default_system_environment
{
    [[nodiscard]] fs::path config_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }

    [[nodiscard]] fs::path user_data_dir(const fs::path&, const std::string& app_name) const override
    {
        return default_system_environment::user_dir()/std::format(".{}", app_name);
    }

    [[nodiscard]] fs::path cache_dir(const fs::path&, const std::string& app_name) const override
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
    std::println("Config Dir:    {}", env.config_dir().string());
    std::println("Cache Dir:     {}", env.cache_dir().string());
 
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::println(stderr, "Error: {}", e.what());
    return 1;    
}
```

## Run Tests

```bash
cd build
./tests/test_pfadfinder
```

## Author

Martin Fehrs
