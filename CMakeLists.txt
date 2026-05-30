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
set(IS_MACOSX 0)
set(IS_LINUX 0)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(IS_WINDOWS 1)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(IS_MACOSX 1)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(IS_LINUX 1)
endif()

# Plattformspezifisches Backend-Modul auswählen
set(SYSTEM_BACKEND_SOURCE "")
if(IS_WINDOWS)
    set(SYSTEM_BACKEND_SOURCE src/system_backend_windows.cppm)
    set(SELECTED_BACKEND "system_backend_windows")
elseif(IS_MACOSX)
    set(SYSTEM_BACKEND_SOURCE src/system_backend_macos.cppm)
    set(SELECTED_BACKEND "system_backend_macos")
elseif(IS_LINUX)
    set(SYSTEM_BACKEND_SOURCE src/system_backend_linux.cppm)
    set(SELECTED_BACKEND "system_backend_linux")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Statische Bibliothek mit Modul-Dateisatz erstellen
add_library(pfadfinder STATIC)
# Alle Moduldateien in einem FILE_SET
target_sources(pfadfinder PUBLIC
    FILE_SET CXX_MODULES
        FILES src/pfadfinder.cppm
                ${SYSTEM_BACKEND_SOURCE}
)

# Compiler-Features setzen
target_compile_features(pfadfinder PUBLIC cxx_std_23)

# Testing aktivieren
enable_testing()

# Test-Verzeichnis hinzufügen
add_subdirectory(tests)
