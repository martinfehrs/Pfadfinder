find_package(Catch2 REQUIRED)
include(Catch)
enable_testing()

# Test-Executable erstellen
add_executable(test_pfadfinder)

target_sources(test_pfadfinder
    PRIVATE
        FILE_SET CXX_MODULES
        FILES
            env_guard.cppm
)

target_sources(test_pfadfinder
    PRIVATE
        test_pfadfinder.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(test_pfadfinder
        PRIVATE
            env_guard_windows.cpp
            windows_environment_tests.cpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(test_pfadfinder
        PRIVATE
            env_guard_posix.cpp
            linux_environment_tests.cpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(test_pfadfinder
        PRIVATE
            env_guard_posix.cpp
            macos_environment_tests.cpp
    )
endif()

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

# Installationstest: prüft find_package() in externem Projekt
add_test(NAME install_test
    COMMAND ${CMAKE_COMMAND}
        "-DPFADFINDER_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
        "-DPFADFINDER_BUILD_DIR=${CMAKE_BINARY_DIR}"
        "-DPFADFINDER_GENERATOR=${CMAKE_GENERATOR}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/run_install_test.cmake"
)
