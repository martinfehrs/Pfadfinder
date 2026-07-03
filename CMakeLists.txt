cmake_minimum_required(VERSION 3.28)

project(pfadfinder
    VERSION 0.9.0
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
    
    # Set output directories for German and English documentation
    set(DOXYGEN_OUTPUT_DIR_DE ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/de)
    set(DOXYGEN_OUTPUT_DIR_EN ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/en)
    
    # Configure German Doxyfile template with awesome CSS path
    set(DOXYGEN_OUTPUT_DIR ${DOXYGEN_OUTPUT_DIR_DE})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_de.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_de
    )
    
    # Filter README.md for English documentation (remove lines before # Pfadfinder)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/README.md README_CONTENT)
    string(FIND "${README_CONTENT}" "# Pfadfinder" TITLE_POS)
    if(NOT TITLE_POS EQUAL -1)
        string(SUBSTRING "${README_CONTENT}" "${TITLE_POS}" -1 README_DOXYGEN)
    else()
        set(README_DOXYGEN "${README_CONTENT}")
    endif()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/README_doxygen.md "${README_DOXYGEN}")
    
    # Configure English Doxyfile template with awesome CSS path
    set(DOXYGEN_OUTPUT_DIR ${DOXYGEN_OUTPUT_DIR_EN})
    
    # Create a small C++ program to filter comments from source files
    add_executable(filter_comments EXCLUDE_FROM_ALL docs/filter_comments.cpp)
    target_compile_features(filter_comments PRIVATE cxx_std_23)
    
    # Create filtered source directory for English documentation
    set(FILTERED_SRC_EN_DIR ${CMAKE_CURRENT_BINARY_DIR}/src_filtered_en)
    file(MAKE_DIRECTORY ${FILTERED_SRC_EN_DIR})
    
    # Filter German comments from source files using filter_comments program
    add_custom_command(
        OUTPUT ${FILTERED_SRC_EN_DIR}/stamp
        COMMAND ${CMAKE_COMMAND} -E remove -f ${FILTERED_SRC_EN_DIR}/*
        COMMAND ${CMAKE_COMMAND} -E make_directory ${FILTERED_SRC_EN_DIR}
        COMMAND filter_comments < ${CMAKE_CURRENT_SOURCE_DIR}/src/error.cppm > ${FILTERED_SRC_EN_DIR}/error.cppm
        COMMAND filter_comments < ${CMAKE_CURRENT_SOURCE_DIR}/src/system_environment.cppm > ${FILTERED_SRC_EN_DIR}/system_environment.cppm
        COMMAND filter_comments < ${CMAKE_CURRENT_SOURCE_DIR}/src/linux_backend.cppm > ${FILTERED_SRC_EN_DIR}/linux_backend.cppm
        COMMAND filter_comments < ${CMAKE_CURRENT_SOURCE_DIR}/src/windows_backend.cppm > ${FILTERED_SRC_EN_DIR}/windows_backend.cppm
        COMMAND filter_comments < ${CMAKE_CURRENT_SOURCE_DIR}/src/macos_backend.cppm > ${FILTERED_SRC_EN_DIR}/macos_backend.cppm
        COMMAND filter_comments < ${CMAKE_CURRENT_SOURCE_DIR}/src/pfadfinder.cppm > ${FILTERED_SRC_EN_DIR}/pfadfinder.cppm
        COMMAND ${CMAKE_COMMAND} -E touch ${FILTERED_SRC_EN_DIR}/stamp
        COMMENT "Creating filtered source files and English documentation for English doxygen"
        DEPENDS filter_comments
        VERBATIM
    )
    
    # Set output directory for English doxygen
    set(DOXYGEN_OUTPUT_DIR ${DOXYGEN_OUTPUT_DIR_EN})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_en.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_en
    )
        
    # Add custom target for German documentation
    add_custom_target(docs-de
        # Create output directory first
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR_DE}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_de
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating German API documentation with Doxygen"
        VERBATIM
    )
    
    # Add custom target for English documentation
    add_custom_target(docs-en
        # Create output directory first
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR_EN}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_en
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating English API documentation with Doxygen"
        DEPENDS ${FILTERED_SRC_EN_DIR}/stamp
        VERBATIM
    )
    
    # Add combined target for both languages
    add_custom_target(docs
        DEPENDS docs-de docs-en
        COMMENT "Generating API documentation in both German and English"
    )
else()
    message(WARNING "Doxygen not found. Documentation targets will show warning.")
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found. Please install Doxygen to generate documentation."
        COMMENT "Doxygen not available"
    )
    add_custom_target(docs-de
        COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found. Please install Doxygen to generate German documentation."
        COMMENT "Doxygen not available (German)"
    )
    add_custom_target(docs-en
        COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found. Please install Doxygen to generate English documentation."
        COMMENT "Doxygen not available (English)"
    )
endif()
