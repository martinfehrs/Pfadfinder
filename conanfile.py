from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


class PfadfinderConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    options = {"docs": [True, False]}
    default_options = {"docs": True}

    def requirements(self):
        self.requires("catch2/3.6.0")
        self.tool_requires("cmake/4.4.2")
        self.tool_requires("ninja/1.13.2")
        if self.options.docs:
            self.tool_requires("doxygen/1.17.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        CMakeDeps(self).generate()
        CMakeToolchain(self).generate()
