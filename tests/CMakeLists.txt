find_package(Catch2 REQUIRED)
include(Catch)
enable_testing()

# Test-Executable erstellen
add_executable(test_pfadfinder)

# Modul-Quellen hinzufügen
# Nur Modul-Schnittstellen-Einheiten (export module) müssen im FILE_SET CXX_MODULES stehen
target_sources(test_pfadfinder
    PRIVATE
        FILE_SET CXX_MODULES
        FILES
            scoped_environment.cppm
)

target_sources(test_pfadfinder PRIVATE test_pfadfinder.cpp)

# CTest-Integration für Catch2
catch_discover_tests(test_pfadfinder)

# Benötigte Bibliotheken verlinken
target_link_libraries(test_pfadfinder PRIVATE
    pfadfinder
    Catch2::Catch2
    Catch2::Catch2WithMain
)

# C++20 für Tests aktivieren
target_compile_features(test_pfadfinder PRIVATE cxx_std_20)


