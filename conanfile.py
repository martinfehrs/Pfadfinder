from conan import ConanFile
from conan.tools.cmake import CMakeConfigDeps, cmake_layout


class PfadfinderConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    options = {"docs": [True, False]}
    default_options = {"docs": True}

    def requirements(self):
        self.requires("catch2/3.6.0")
        # cmake und ninja bewusst nicht als tool_requires: conan_provider.cmake
        # kann sie nicht in den PATH legen, bevor CMake sie braucht (cmake
        # läuft bereits, ninja wird beim Generieren im System-PATH gesucht).
        # Doxygen dagegen wird über find_package(Doxygen) via CMakeConfigDeps
        # aus dem conan-Cache aufgelöst.
        if self.options.docs:
            self.tool_requires("doxygen/1.17.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        # Kein CMakeToolchain: CMake erkennt den Compiler selbst (z.B. cl via
        # VS-Installation), conan_provider.cmake ruft conan install beim
        # ersten find_package() automatisch auf.
        CMakeConfigDeps(self).generate()
