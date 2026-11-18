/**
 * @file main.cpp
 * @brief Consumer-Testprogramm für den Installationstest
 *
 * Dieses Programm wird nach der Installation der Bibliothek gebaut und ausgeführt,
 * um zu prüfen, ob find_package(pfadfinder) funktioniert und die Bibliothek
 * korrekt gelinkt und verwendet werden kann.
 */

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

import pfadfinder;

namespace fs = std::filesystem;

int main() try
{
    pfadfinder::application_environment env{ "consumer_test" };

    auto exe_path = env.executable_path();
    auto home = env.home_dir();

    std::cout << "executable_path: " << exe_path.string() << std::endl;
    std::cout << "home_dir: " << home.string() << std::endl;

    // Prüfe, ob find_home_dir konsistent ist
    auto found_home = env.find_home_dir();
    if (!found_home.has_value())
    {
        std::cerr << "FAIL: find_home_dir returned nullopt" << std::endl;
        return 1;
    }
    if (*found_home != home)
    {
        std::cerr << "FAIL: find_home_dir != home_dir" << std::endl;
        return 1;
    }

    // Prüfe create_data_dir + data_dir
    auto data = env.create_data_dir();
    if (!fs::exists(data) || !fs::is_directory(data))
    {
        std::cerr << "FAIL: create_data_dir did not create directory" << std::endl;
        return 1;
    }
    std::cout << "data_dir: " << data.string() << std::endl;

    std::cout << "OK" << std::endl;
    return 0;
}
catch(const pfadfinder::error& e)
{
    std::cerr << "FAIL: " << e.what() << std::endl;
    return 1;
}
