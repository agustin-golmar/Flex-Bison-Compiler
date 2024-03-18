[![✗](https://img.shields.io/badge/Release-v1.0.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)

[![✗](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml/badge.svg?branch=production)](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml)

# Flex-Bison-Compiler

A base compiler example, developed with Flex and Bison.

## Requirements

* [Bison v3.8.2](https://www.gnu.org/software/bison/)
* [CMake v3.24.1](https://cmake.org/)
* [Docker v24.0.6](https://docs.docker.com/get-docker/)
* [Flex v2.6.4](https://github.com/westes/flex)
* [GCC v11.1.0](https://gcc.gnu.org/)
* [Make v4.3](https://www.gnu.org/software/make/)
* [Microsoft Visual Studio 2022](https://visualstudio.microsoft.com/vs/community/)

## Install

```bash
user@machine:path/ $ chmod u+x --recursive script/linux
user@machine:path/ $ script/linux/install.sh
```

To build an image (only the first time you clone the repository), and deploy a container:

```bash
user@machine:path/ $ script/linux/docker-build.sh
user@machine:path/ $ script/linux/docker-run.sh
```

## Build

```bash
user@machine:path/ $ script/linux/build.sh
```

## Test

```bash
user@machine:path/ $ script/linux/test.sh
```

## Start

```bash
user@machine:path/ $ chmod u+x --recursive script
user@machine:path/ $ script/linux/start.sh <program>
```

Replace `<program>` with a path to the program file.
