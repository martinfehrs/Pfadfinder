import os

from conan import ConanFile
from conan.tools.cmake import CMakeConfigDeps, cmake_layout
from conan.tools.files import save


class PfadfinderConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    options = {"docs": [True, False]}
    default_options = {"docs": True}

    def requirements(self):
        self.requires("catch2/3.6.0")
        # cmake und ninja bewusst nicht als tool_requires: conan_provider.cmake
        # kann sie nicht in den PATH legen, bevor CMake sie braucht (cmake
        # läuft bereits, ninja wird beim Generieren im System-PATH gesucht).
        if self.options.docs:
            self.tool_requires("doxygen/1.17.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        # Kein CMakeToolchain: CMake erkennt den Compiler selbst (z.B. cl via
        # VS-Installation), conan_provider.cmake ruft conan install beim
        # ersten find_package() automatisch auf.
        CMakeConfigDeps(self).generate()
        if self.options.docs:
            # Das doxygen-Rezept setzt cmake_find_mode=none, conan generiert also
            # keine CMake-Config dafür. Stattdessen den Pfad zum conan-doxygen
            # schreiben, den CMakeLists.txt vor find_package(Doxygen) einliest.
            doxygen = self.dependencies.build["doxygen"]
            exe = "doxygen.exe" if self.settings.os == "Windows" else "doxygen"
            path = os.path.join(doxygen.package_folder, doxygen.cpp_info.bindirs[0], exe)
            save(self, "doxygen_executable.cmake",
                 f'set(DOXYGEN_EXECUTABLE "{path.replace(chr(92), "/")}" '
                 'CACHE FILEPATH "Doxygen executable (conan)")\n')
