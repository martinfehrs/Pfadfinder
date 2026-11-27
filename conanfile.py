import os

from conan import ConanFile
from conan.tools.cmake import CMakeConfigDeps, cmake_layout
from conan.tools.files import save


class PfadfinderConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    options = {"docs": [True, False]}
    # docs ist Opt-in (Preset conan-docs): build- und clang-tidy-Job sollen
    # doxygen nicht in den Abhängigkeitsgraphen ziehen
    default_options = {"docs": False}

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
            # keine CMake-Config dafür. Damit find_package(Doxygen) das conan-
            # doxygen findet, eigene Config-Dateien in den Generators-Ordner
            # schreiben (conan_provider.cmake durchsucht diesen Ordner).
            doxygen = self.dependencies.build["doxygen"]
            exe = "doxygen.exe" if self.settings.os == "Windows" else "doxygen"
            path = os.path.join(doxygen.package_folder, doxygen.cpp_info.bindirs[0], exe)
            save(self, "DoxygenConfig.cmake",
                 f'set(DOXYGEN_EXECUTABLE "{path.replace(chr(92), "/")}" '
                 'CACHE FILEPATH "Doxygen executable (conan)")\n'
                 # CMakeLists.txt prüft DOXYGEN_FOUND (sonst setzt das nur das
                 # FindDoxygen-Modul, im Config-Modus heißt es Doxygen_FOUND)
                 'set(DOXYGEN_FOUND TRUE)\n')
            save(self, "DoxygenConfigVersion.cmake",
                 f'set(PACKAGE_VERSION "{doxygen.ref.version}")\n'
                 'if(PACKAGE_FIND_VERSION VERSION_EQUAL PACKAGE_VERSION '
                 'OR PACKAGE_FIND_VERSION VERSION_LESS PACKAGE_VERSION)\n'
                 '    set(PACKAGE_VERSION_COMPATIBLE TRUE)\n'
                 '    if(PACKAGE_FIND_VERSION VERSION_EQUAL PACKAGE_VERSION)\n'
                 '        set(PACKAGE_VERSION_EXACT TRUE)\n'
                 '    endif()\n'
                 'endif()\n')
