# Versión mínima requerida de CMake. Se puede reducir o incrementar a riesgo del desarrollador.
# @see https://cmake.org/download/
cmake_minimum_required(VERSION 3.22)

# Nombre del proyecto, y lenguaje utilizado (C, en este caso).
project(Compiler C)

# Compilar el parser con Bison, generando contra-ejemplos en caso de que se presenten conflictos.
add_custom_command(
	OUTPUT ../src/frontend/syntactic-analysis/bison-parser.c ../src/frontend/syntactic-analysis/bison-parser.h
	COMMAND bison -Wcounterexamples -d ../src/frontend/syntactic-analysis/bison-grammar.y -o ../src/frontend/syntactic-analysis/bison-parser.c)

# Seleccionar estrategia según el compilador de C disponible en el sistema.
# @see https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	message(NOTICE "El compilador de C es GCC.")

	# Opciones para GCC.
	add_compile_options(-static-libgcc)
	add_compile_options(-std=gnu99)
	add_compile_options(-O3)

	# Compilar el scanner con Flex.
	add_custom_command(
		OUTPUT ../src/frontend/lexical-analysis/flex-scanner.c
		COMMAND flex -o ../src/frontend/lexical-analysis/flex-scanner.c ../src/frontend/lexical-analysis/flex-patterns.l
		DEPENDS ../src/frontend/syntactic-analysis/bison-parser.c ../src/frontend/syntactic-analysis/bison-parser.h)

elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	message(NOTICE "El compilador de C es Microsoft Visual Studio.")

	# Opciones para MSVC.
	# add_compile_options(...)

	# Compilar el scanner con Flex, usando compatibilidad con Microsoft Windows.
	add_custom_command(
		OUTPUT ../src/frontend/lexical-analysis/flex-scanner.c
		COMMAND flex --wincompat -o ../src/frontend/lexical-analysis/flex-scanner.c ../src/frontend/lexical-analysis/flex-patterns.l
		DEPENDS ../src/frontend/syntactic-analysis/bison-parser.c ../src/frontend/syntactic-analysis/bison-parser.h)

elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
	message(NOTICE "El compilador de C es LLVM Clang.")

	# Opciones para Clang.
	# add_compile_options(...)

	# Compilar el scanner con Flex.
	add_custom_command(
		OUTPUT ../src/frontend/lexical-analysis/flex-scanner.c
		COMMAND flex -o ../src/frontend/lexical-analysis/flex-scanner.c ../src/frontend/lexical-analysis/flex-patterns.l
		DEPENDS ../src/frontend/syntactic-analysis/bison-parser.c ../src/frontend/syntactic-analysis/bison-parser.h)
else ()
	message(NOTICE "El compilador de C es desconocido.")
	message(NOTICE "No se generará el scanner con Flex.")
	message(NOTICE "Incluya un 'elseif' adicional para manejar su compilador.")

endif ()

# Especificar punto de entrada del proyecto y códigos fuente (extensión *.c).
# Los *.h se incluyen automáticamente desde estos fuentes.
add_executable(Compiler
	src/main.c
	src/backend/code-generation/generator.c
	src/backend/domain-specific/calculator.c
	src/backend/support/logger.c
	src/frontend/lexical-analysis/flex-actions.c
	src/frontend/lexical-analysis/flex-scanner.c
	src/frontend/syntactic-analysis/bison-actions.c
	src/frontend/syntactic-analysis/bison-parser.c
	# Agregar otros archivos *.c (si no se incluyen, no se compilan).
	# ...
)

# Linkear el proyecto y sus librerías.
target_link_libraries(Compiler)
