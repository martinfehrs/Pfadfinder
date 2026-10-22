# CMake-Skript für den Installationstest
# Dieses Skript wird von CTest ausgeführt und prüft, ob die Bibliothek
# installiert und in einem externen CMake-Projekt verwendet werden kann.
#
# Erwartete Variablen:
#   PFADFINDER_SOURCE_DIR   - Quellverzeichnis des Pfadfinder-Projekts
#   PFADFINDER_BUILD_DIR    - Build-Verzeichnis des Pfadfinder-Projekts
#   PFADFINDER_GENERATOR    - CMake-Generator (z.B. "Ninja")
#   CMAKE_COMMAND           - Pfad zum cmake-Kommando (automatisch gesetzt)

set(INSTALL_PREFIX "${PFADFINDER_BUILD_DIR}/install-test")
set(CONSUMER_SOURCE_DIR "${PFADFINDER_SOURCE_DIR}/tests/consumer")
set(CONSUMER_BUILD_DIR "${PFADFINDER_BUILD_DIR}/consumer-test-build")

message(STATUS "=== Installationstest ===")
message(STATUS "Install prefix: ${INSTALL_PREFIX}")
message(STATUS "Consumer source: ${CONSUMER_SOURCE_DIR}")

# Multi-Config-Generatoren (z.B. "Ninja Multi-Config", "Visual Studio ...")
# benötigen --config beim Installieren
if(PFADFINDER_GENERATOR MATCHES "Multi-Config|Visual Studio|Xcode")
    set(PFADFINDER_CONFIG "Release")
else()
    set(PFADFINDER_CONFIG "")
endif()

# Consumer-Projekt immer mit Ninja Multi-Config bauen:
# Der Visual-Studio-Generator unterstützt keine BMI-Only-Kompilierung
# von C++-Modulen aus importierten Targets.
set(CONSUMER_GENERATOR "Ninja Multi-Config")
set(CONSUMER_CONFIG "Release")
set(CONSUMER_CONFIG_SUBDIR "Release")

# Auf Windows: MSVC-Umgebung über vswhere.exe und Wrapper-Skript einrichten.
# Ohne eingerichtete Umgebung kann der Ninja-Generator nicht linken
# (fehlende Windows-SDK-Bibliotheken wie kernel32.lib).
set(VCVARS_WRAPPER "")

if(CMAKE_HOST_WIN32 AND NOT DEFINED ENV{VCINSTALLDIR})
    find_program(VSWHERE_EXE
        NAMES vswhere vswhere.exe
        PATHS
            "C:/Program Files (x86)/Microsoft Visual Studio/Installer"
            "C:/Program Files/Microsoft Visual Studio/Installer"
    )

    if(VSWHERE_EXE)
        execute_process(
            COMMAND "${VSWHERE_EXE}" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
            OUTPUT_VARIABLE VS_INSTALL_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(VS_INSTALL_DIR)
            message(STATUS "Visual Studio gefunden: ${VS_INSTALL_DIR}")
            # Forward-Slashes in Backslashes umwandeln (cmd.exe verträgt keine gemischten Pfade)
            string(REPLACE "/" "\\" VS_INSTALL_DIR "${VS_INSTALL_DIR}")
            set(VCVARS_BAT "${VS_INSTALL_DIR}\\VC\\Auxiliary\\Build\\vcvars64.bat")
            if(EXISTS "${VCVARS_BAT}")
                set(VCVARS_WRAPPER "${PFADFINDER_BUILD_DIR}/vcvars_wrapper.bat")
                file(WRITE "${VCVARS_WRAPPER}" "@echo off\r\ncall \"${VCVARS_BAT}\"\r\n%*\r\n")
                message(STATUS "MSVC-Wrapper erstellt: ${VCVARS_WRAPPER}")
            endif()
        endif()
    endif()
endif()

# Auf Windows muss die MSVC-Umgebung eingerichtet sein, sonst schlägt der Build fehl
if(CMAKE_HOST_WIN32 AND NOT DEFINED ENV{VCINSTALLDIR} AND NOT VCVARS_WRAPPER)
    message(FATAL_ERROR "MSVC-Umgebung konnte nicht eingerichtet werden (vswhere.exe oder vcvars64.bat nicht gefunden).")
endif()

# Schritt 1: Bibliothek installieren
message(STATUS "Schritt 1: Bibliothek installieren...")
set(INSTALL_CMD ${CMAKE_COMMAND} --install "${PFADFINDER_BUILD_DIR}" --prefix "${INSTALL_PREFIX}")

if(PFADFINDER_CONFIG)
    list(APPEND INSTALL_CMD --config "${PFADFINDER_CONFIG}")
endif()

execute_process(
    COMMAND ${INSTALL_CMD}
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_output
    ERROR_VARIABLE install_error
)

if(NOT install_result EQUAL 0)
    message(FATAL_ERROR "Installation fehlgeschlagen:\n${install_output}\n${install_error}")
endif()

message(STATUS "Installation erfolgreich.")

# Schritt 2: Prüfen, ob die installierten Dateien existieren
message(STATUS "Schritt 2: Installierte Dateien prüfen...")
# Bibliotheksname ist plattformabhängig (Windows: pfadfinder.lib, Unix: libpfadfinder.a)

if(EXISTS "${INSTALL_PREFIX}/lib/pfadfinder.lib")
    set(PFADFINDER_LIB_FILE "${INSTALL_PREFIX}/lib/pfadfinder.lib")
elseif(EXISTS "${INSTALL_PREFIX}/lib/libpfadfinder.a")
    set(PFADFINDER_LIB_FILE "${INSTALL_PREFIX}/lib/libpfadfinder.a")
else()
    message(FATAL_ERROR "Bibliothek nicht gefunden in ${INSTALL_PREFIX}/lib/ (weder pfadfinder.lib noch libpfadfinder.a)")
endif()

set(EXPECTED_FILES
    "${PFADFINDER_LIB_FILE}"
    "${INSTALL_PREFIX}/lib/cmake/pfadfinder/pfadfinderConfig.cmake"
    "${INSTALL_PREFIX}/lib/cmake/pfadfinder/src/pfadfinder.cppm"
    "${INSTALL_PREFIX}/lib/cmake/pfadfinder/src/errors.cppm"
    "${INSTALL_PREFIX}/lib/cmake/pfadfinder/src/system_environment.cppm"
)

foreach(expected_file IN LISTS EXPECTED_FILES)
    if(NOT EXISTS "${expected_file}")
        message(FATAL_ERROR "Erwartete Datei nicht gefunden: ${expected_file}")
    endif()
endforeach()

message(STATUS "Alle erwarteten Dateien vorhanden.")

# Schritt 3: Consumer-Projekt konfigurieren (prüft find_package())
message(STATUS "Schritt 3: Consumer-Projekt konfigurieren...")

set(CONFIGURE_CMD
    "${CMAKE_COMMAND}" -G "${CONSUMER_GENERATOR}"
    -S "${CONSUMER_SOURCE_DIR}"
    -B "${CONSUMER_BUILD_DIR}"
    -DCMAKE_PREFIX_PATH=${INSTALL_PREFIX}
    -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
)

# Gleichen Compiler wie das Hauptprojekt verwenden (z.B. MSVC statt MinGW auf Windows)
if(PFADFINDER_CXX_COMPILER)
    list(APPEND CONFIGURE_CMD "-DCMAKE_CXX_COMPILER=${PFADFINDER_CXX_COMPILER}")
endif()

if(VCVARS_WRAPPER)
    list(PREPEND CONFIGURE_CMD "${VCVARS_WRAPPER}")
endif()

execute_process(
    COMMAND ${CONFIGURE_CMD}
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_error
)

if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "Consumer-Konfiguration fehlgeschlagen:\n${configure_output}\n${configure_error}")
endif()

message(STATUS "Consumer-Konfiguration erfolgreich (find_package() funktioniert).")

# Schritt 4: Consumer-Projekt bauen
message(STATUS "Schritt 4: Consumer-Projekt bauen...")

set(BUILD_CMD
    "${CMAKE_COMMAND}" --build "${CONSUMER_BUILD_DIR}" --config "${CONSUMER_CONFIG}"
)

if(VCVARS_WRAPPER)
    list(PREPEND BUILD_CMD "${VCVARS_WRAPPER}")
endif()

execute_process(
    COMMAND ${BUILD_CMD}
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_output
    ERROR_VARIABLE build_error
)

if(NOT build_result EQUAL 0)
    message(FATAL_ERROR "Consumer-Build fehlgeschlagen:\n${build_output}\n${build_error}")
endif()

message(STATUS "Consumer-Build erfolgreich.")

# Schritt 5: Consumer-Programm ausführen
message(STATUS "Schritt 5: Consumer-Programm ausführen...")
set(CONSUMER_EXE_DIR "${CONSUMER_BUILD_DIR}/${CONSUMER_CONFIG_SUBDIR}")

# Executable-Name ist plattformabhängig (Windows: consumer.exe, Unix: consumer)
if(EXISTS "${CONSUMER_EXE_DIR}/consumer.exe")
    set(CONSUMER_EXE "${CONSUMER_EXE_DIR}/consumer.exe")
else()
    set(CONSUMER_EXE "${CONSUMER_EXE_DIR}/consumer")
endif()

execute_process(
    COMMAND "${CONSUMER_EXE}"
    RESULT_VARIABLE run_result
    OUTPUT_VARIABLE run_output
    ERROR_VARIABLE run_error
)

message(STATUS "Consumer-Ausgabe:\n${run_output}")

if(NOT run_result EQUAL 0)
    message(FATAL_ERROR "Consumer-Ausführung fehlgeschlagen (Exit-Code: ${run_result}):\n${run_error}")
endif()

# Prüfen, ob die Ausgabe "OK" enthält
string(FIND "${run_output}" "OK" ok_pos)

if(ok_pos EQUAL -1)
    message(FATAL_ERROR "Consumer-Ausgabe enthält kein 'OK':\n${run_output}")
endif()

message(STATUS "=== Installationstest erfolgreich ===")
