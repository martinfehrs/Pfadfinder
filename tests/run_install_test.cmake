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
# benötigen --config und legen Ausgaben in einem Konfigurations-Unterverzeichnis ab
if(PFADFINDER_GENERATOR MATCHES "Multi-Config|Visual Studio|Xcode")
    set(PFADFINDER_CONFIG "Release")
    set(PFADFINDER_CONFIG_SUBDIR "Release")
else()
    set(PFADFINDER_CONFIG "")
    set(PFADFINDER_CONFIG_SUBDIR "")
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
    "${INSTALL_PREFIX}/lib/cmake/pfadfinder/src/error.cppm"
    "${INSTALL_PREFIX}/lib/cmake/pfadfinder/src/system_environment.cppm"
)
foreach(expected_file IN LISTS EXPECTED_FILES)
    if(NOT EXISTS "${expected_file}")
        message(FATAL_ERROR "Erwartete Datei nicht gefunden: ${expected_file}")
    endif()
endforeach()
message(STATUS "Alle erwarteten Dateien vorhanden.")

# Schritt 3: Consumer-Projekt konfigurieren
message(STATUS "Schritt 3: Consumer-Projekt konfigurieren...")
execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${PFADFINDER_GENERATOR}"
        -S "${CONSUMER_SOURCE_DIR}"
        -B "${CONSUMER_BUILD_DIR}"
        -DCMAKE_PREFIX_PATH=${INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=Release
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "Consumer-Konfiguration fehlgeschlagen:\n${configure_output}\n${configure_error}")
endif()
message(STATUS "Consumer-Konfiguration erfolgreich.")

# Schritt 4: Consumer-Projekt bauen
message(STATUS "Schritt 4: Consumer-Projekt bauen...")
set(BUILD_CMD ${CMAKE_COMMAND} --build "${CONSUMER_BUILD_DIR}")
if(PFADFINDER_CONFIG)
    list(APPEND BUILD_CMD --config "${PFADFINDER_CONFIG}")
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
if(PFADFINDER_CONFIG_SUBDIR)
    set(CONSUMER_EXE_DIR "${CONSUMER_BUILD_DIR}/${PFADFINDER_CONFIG_SUBDIR}")
else()
    set(CONSUMER_EXE_DIR "${CONSUMER_BUILD_DIR}")
endif()
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
