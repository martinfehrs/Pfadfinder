cmake_minimum_required(VERSION 4.3)

project(pfadfinder
    VERSION 1.0.0
    DESCRIPTION "Projekt zur Bestimmung des Binary-Verzeichnisses der laufenden Anwendung"
    LANGUAGES CXX
)

# C++23-Standard aktivieren (erforderlich für std::expected)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Statische Bibliothek mit Modul-Dateisatz erstellen
add_library(pfadfinder STATIC)
target_sources(pfadfinder PUBLIC
    FILE_SET CXX_MODULES
        FILES src/pfadfinder.cppm
)

# Compiler-Features setzen
target_compile_features(pfadfinder PUBLIC cxx_std_23)

# Testing aktivieren
enable_testing()

# Test-Verzeichnis hinzufügen
add_subdirectory(tests)
