# Projektspezifische Anweisungen für Pfadfinder

## Projektbeschreibung

@LIESMICH.md

## Kompilierung und Testausführung

Führe folgende Befehle aus, um das Projekt zu kompilieren und die Tests auszuführen.

### Release-Build

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -s:a build_type=Release -c tools.cmake.cmaketoolchain:generator="Ninja"
unset LD_LIBRARY_PATH && cmake --preset=conan-release
unset LD_LIBRARY_PATH && cmake --build --preset=conan-release
```

### Ausführung der Release-Tests

```bash
unset LD_LIBRARY_PATH && cmake --build --preset=conan-release --target test
```

### Debug-Build

```bash
conan install . --build=missing -s:a compiler.cppstd=20 -s:a build_type=Debug -c tools.cmake.cmaketoolchain:generator="Ninja"
unset LD_LIBRARY_PATH && cmake --preset=conan-debug
unset LD_LIBRARY_PATH && cmake --build --preset=conan-debug
```

### Ausführung der Debug-Tests

```bash
unset LD_LIBRARY_PATH && cmake --build --preset=conan-debug --target test
```
