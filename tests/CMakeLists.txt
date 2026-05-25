# CATCH2 laden (header-only, single-include version)
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.6.0
)

FetchContent_MakeAvailable(Catch2)

# Test-Executable erstellen
add_executable(test_pfadfinder test_pfadfinder.cpp)

# Benötigte Bibliotheken verlinken
target_link_libraries(test_pfadfinder PRIVATE
    pfadfinder
    Catch2::Catch2
    Catch2::Catch2WithMain
)

# C++23 für Tests aktivieren
target_compile_features(test_pfadfinder PRIVATE cxx_std_23)

# Include-Verzeichnisse für CATCH2
target_include_directories(test_pfadfinder PRIVATE ${Catch2_SOURCE_DIR})
