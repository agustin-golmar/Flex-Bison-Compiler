[![✗](https://img.shields.io/badge/Release-v0.2.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)

# Compilador Flex/Bison

Un compilador vacío de ejemplo construido con Flex y Bison.

## Requerimientos

Para construir el compilador, se requieren las siguientes dependencias:

* [Bison v3.8.2](https://www.gnu.org/software/bison/)
* [CMake v3.24.1](https://cmake.org/)
* [Flex v2.6.4](https://github.com/westes/flex)
* [GCC v11.1.0](https://gcc.gnu.org/)
* [Make v4.3](https://www.gnu.org/software/make/)

Si en lugar de trabajar con un entorno _Linux_, se está construyendo el proyecto sobre un entorno _Microsoft Windows_, se debe instalar _Microsoft Visual Studio 2022_ con las extensiones para desarrollar aplicaciones en _C/C++_, así como también las herramientas requeridas, con excepción del compilador _GCC_ y la herramienta _Make_.

## Construcción

Para construir el proyecto por completo, ejecute en la raíz del repositorio los siguientes comandos (en _Linux_):

```bash
user@machine:path/ $ chmod u+x --recursive script
user@machine:path/ $ script/build.sh
```

En un entorno _Microsoft Windows_, en cambio, se debe ejecutar:

```bash
user@machine:path/ $ script\build.bat
```

Luego se deberá abrir la solución generada `bin\Compiler.sln` con el IDE _Microsoft Visual Studio 2022_. Los ejecutables que este sistema construye se depositan dentro del directorio `bin\Debug` y `bin\Release`, según corresponda.

## Ejecución

Para compilar un programa, primero cree un archivo vacío denominado `program` (o el nombre que desee), con el siguiente contenido:

```
123123 + 123 - 2 * (454 + 890 / 89)
```

Luego, ejecute el compilador desde el directorio raíz del proyecto, o desde cualquier otro lugar indicando el path hacia el script `start.sh` y pasando por parámetro el path hacia el programa a compilar:

```bash
user@machine:path/ $ script/start.sh program
```

En Windows:

```bash
user@machine:path/ $ script\start.bat program
```

Debería obtener el resultado correcto de evaluar el programa anterior: `122318`.

## Testing

En Linux:

```bash
user@machine:path/ $ script/test.sh
```

En Windows:

```bash
user@machine:path/ $ script\test.bat
```

Si desea agregar nuevos casos de uso, deberá crear un archivo por cada uno, que contenga el programa a testear dentro de las carpetas `test/accept` o `test/reject` según corresponda (es decir, si el mismo debe ser aceptado o rechazado por el compilador).
