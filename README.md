[![✗](https://img.shields.io/badge/Release-v0.1.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)

# Compilador Flex/Bison

Un compilador vacío construido con Flex y Bison.

## Requerimientos

Para construir el compilador, se requieren las siguientes dependencias:

* [Bison v3.8.2](https://www.gnu.org/software/bison/)
* [CMake v3.22.2](https://cmake.org/)
* [Flex v2.6.4](https://github.com/westes/flex)
* [GCC v11.1.0](https://gcc.gnu.org/)

## Construcción

Para construir el proyecto por completo, ejecute en la raíz del repositorio el siguiente comando:

```bash
user@machine:path/ $ cmake
```

## Ejecución

Para compilar un programa, primero cree un archivo vacío denominado `program` con el siguiente contenido:

```
1 + 7 - 8
```

Luego, ejecute el compilador indicando la ruta hacia este archivo como parámetro:

```bash
user@machine:path/ $ ./compiler program
```
