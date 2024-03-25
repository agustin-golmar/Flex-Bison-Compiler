# Ubuntu

This scripts can be used inside a _Docker_ container with _Ubuntu_, inside _Windows Subsystem for Linux_ (WSL), or in a real installation of the distribution.

## Requirements

* [Bison v3.8.2](https://www.gnu.org/software/bison/)
* [CMake v3.24.1](https://cmake.org/)
* [Flex v2.6.4](https://github.com/westes/flex)
* [GCC v11.1.0](https://gcc.gnu.org/)
* [Make v4.3](https://www.gnu.org/software/make/)

## Install

```bash
chmod u+x --recursive script/ubuntu
script/ubuntu/install.sh
```

## Build

```bash
script/ubuntu/build.sh
```

## Test

```bash
script/ubuntu/test.sh
```

## Start

```bash
script/ubuntu/start.sh <program>
```

Replace `<program>` with a path to the program file.
