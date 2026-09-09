find_package(Catch2 REQUIRED)
include(Catch)
enable_testing()

# Plattform-spezifische Variablen für config.cppm.in
set(IS_WINDOWS false)
set(IS_LINUX false)
set(IS_MACOS false)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(IS_WINDOWS true)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(IS_LINUX true)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(IS_MACOS true)
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
# BASE_DIRS muss alle Verzeichnisse enthalten, in denen die Dateien liegen
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
