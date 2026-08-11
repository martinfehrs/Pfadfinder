cmake_minimum_required(VERSION 3.28)

project(Pfadfinder
    VERSION 0.9.0
    DESCRIPTION "Projekt zur Bestimmung des Binärverzeichnisses der laufenden Anwendung"
    LANGUAGES CXX
)

# ============================================================================
# KOMPILIERUNG
# ============================================================================

# C++20-Standard aktivieren
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Statische Bibliothek mit Modul-Dateisatz erstellen
add_library(pfadfinder STATIC)

# Moduldateien in FILE_SET CXX_MODULES
target_sources(pfadfinder PUBLIC
    FILE_SET CXX_MODULES
        FILES src/pfadfinder.cppm
              src/error.cppm
              src/system_environment.cppm
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(pfadfinder PRIVATE src/windows_system_environment.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(pfadfinder PRIVATE src/macos_system_environment.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(pfadfinder PRIVATE src/linux_system_environment.cpp)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Compiler-Features setzen
target_compile_features(pfadfinder PUBLIC cxx_std_20)

# ============================================================================
# TESTS
# ============================================================================

# Testing aktivieren
enable_testing()

# Test-Verzeichnis hinzufügen
add_subdirectory(tests)

# ============================================================================
# INSTALLATION
# ============================================================================
include(GNUInstallDirs)

# CMake Config + Module installieren (für find_package)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pfadfinderConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pfadfinderConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pfadfinder
)

# Bibliothek installieren
install(TARGETS pfadfinder
    EXPORT pfadfinderTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pfadfinder
)

# Export-Targets installieren
install(EXPORT pfadfinderTargets
    FILE pfadfinderConfig.cmake
    NAMESPACE pfadfinder::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pfadfinder
)

# pkg-config Datei installieren (nur für Linux/macOS)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pfadfinder.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/pfadfinder.pc
        @ONLY
    )
    
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pfadfinder.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# ============================================================================
# DOKUMENTATIONSGENERIERUNG
# ============================================================================

# Doxygen documentation target (separates Target, nicht Teil des Haupt-Builds)
find_package(Doxygen 1.17 REQUIRED)

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
target_compile_features(filter_comments PRIVATE cxx_std_20)

# Create filtered source directory for English documentation
set(FILTERED_SRC_EN_DIR ${CMAKE_CURRENT_BINARY_DIR}/src_filtered_en)

# Custom target to filter German comments from source files
add_custom_target(filter_sources
    COMMAND ${CMAKE_COMMAND} -E remove -f ${FILTERED_SRC_EN_DIR}/*
    COMMAND filter_comments ${CMAKE_CURRENT_SOURCE_DIR}/src ${FILTERED_SRC_EN_DIR}
    COMMENT "Creating filtered source files for English doxygen"
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
    DEPENDS filter_sources
    VERBATIM
)

# Add combined target for both languages
add_custom_target(docs
    DEPENDS docs-de docs-en
    COMMENT "Generating API documentation in both German and English"
)

# ============================================================================
# PACKETIERUNG
# ============================================================================
include(CPack)
