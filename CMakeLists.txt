cmake_minimum_required(VERSION 3.28)

project(pfadfinder
    VERSION 1.0.0
    DESCRIPTION "Projekt zur Bestimmung des Binärverzeichnisses der laufenden Anwendung"
    LANGUAGES CXX
)

# C++23-Standard aktivieren (erforderlich für std::expected)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plattform erkennen
set(IS_WINDOWS 0)
set(IS_MACOS 0)
set(IS_LINUX 0)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(IS_WINDOWS 1)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(IS_MACOS 1)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(IS_LINUX 1)
endif()

configure_file(config.hpp.in config.hpp)

# Plattformspezifisches Backend-Modul auswählen
set(SYSTEM_BACKEND_SOURCE "")
if(IS_WINDOWS)
    set(SYSTEM_BACKEND_SOURCE src/windows_backend.cppm)
elseif(IS_MACOS)
    set(SYSTEM_BACKEND_SOURCE src/macos_backend.cppm)
elseif(IS_LINUX)
    set(SYSTEM_BACKEND_SOURCE src/linux_backend.cppm)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Statische Bibliothek mit Modul-Dateisatz erstellen
add_library(pfadfinder STATIC)
# Alle Moduldateien in einem FILE_SET
target_sources(pfadfinder PUBLIC
    FILE_SET CXX_MODULES
        FILES src/pfadfinder.cppm
                src/error.cppm
                src/system_environment.cppm
                ${SYSTEM_BACKEND_SOURCE}
)

target_include_directories(pfadfinder PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Compiler-Features setzen
target_compile_features(pfadfinder PUBLIC cxx_std_23)

# Testing aktivieren
enable_testing()

# Test-Verzeichnis hinzufügen
add_subdirectory(tests)

# Doxygen documentation target (separates Target, nicht Teil des Haupt-Builds)
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    # Fetch doxygen-awesome-css theme
    include(FetchContent)
    FetchContent_Declare(
        doxygen-awesome-css
        URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)
    
    # Set output directory in BINARY_DIR
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
    
    # Configure Doxyfile template with awesome CSS path
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    )
       
    # Add custom target for documentation
    add_custom_target(docs
        # Create output directory first
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(WARNING "Doxygen not found. Documentation target will show warning.")
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found. Please install Doxygen to generate documentation."
        COMMENT "Doxygen not available"
    )
endif()
