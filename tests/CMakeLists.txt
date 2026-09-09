find_package(Catch2 REQUIRED)
include(Catch)
enable_testing()

# Plattform-spezifische Variable für config.cppm
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CURRENT_SYSTEM windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CURRENT_SYSTEM linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CURRENT_SYSTEM macos)
endif()

# Konfigurationsdatei generieren
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cppm.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.cppm
    @ONLY
)

# Test-Executable erstellen
add_executable(test_pfadfinder)

# Modul-Quellen hinzufügen
target_sources(test_pfadfinder
    PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS 
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/config.cppm
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
