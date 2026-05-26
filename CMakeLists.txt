cmake_minimum_required(VERSION 4.3)

project(pfadfinder
    VERSION 1.0.0
    DESCRIPTION "Projekt zur Bestimmung des Binary-Verzeichnisses der laufenden Anwendung"
    LANGUAGES CXX
)

# C++23-Standard aktivieren (erforderlich für std::expected)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plattform-spezifische Makros für config.hpp
# Korrekt für Cross-Compiling: basierend auf Zielplattform, nicht Host
# Verwende CMAKE_SYSTEM_NAME für die Zielplattform

# Variablen initialisieren
set(IS_WINDOWS 0)
set(IS_MACOSX 0)
set(IS_LINUX 0)

# Plattform erkennen (separate if-Blöcke, da elif mit STREQUAL in dieser CMake-Version Probleme hat)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(IS_WINDOWS 1)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(IS_MACOSX 1)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(IS_LINUX 1)
endif()

# config.hpp aus Vorlage generieren
configure_file(config.hpp.in config.hpp)

# Statische Bibliothek mit Modul-Dateisatz erstellen
add_library(pfadfinder STATIC)
target_sources(pfadfinder PUBLIC
    FILE_SET CXX_MODULES
        FILES src/pfadfinder.cppm
)

# Include-Verzeichnis für config.hpp hinzufügen
target_include_directories(pfadfinder PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Compiler-Features setzen
target_compile_features(pfadfinder PUBLIC cxx_std_23)

# Testing aktivieren
enable_testing()

# Test-Verzeichnis hinzufügen
add_subdirectory(tests)
