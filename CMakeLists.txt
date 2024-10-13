# Minimum required version fo CMake. Can be modified at your own risk.
# @see https://cmake.org/download/
cmake_minimum_required(VERSION 3.22)

# Name of the project and language to use (C, in this case).
project(Compiler C)

# Compiles the parser with Bison, generating counter-examples in case S/R or R/R conflicts exists.
add_custom_command(
	OUTPUT ../src/main/c/frontend/syntactic-analysis/BisonParser.c ../src/main/c/frontend/syntactic-analysis/BisonParser.h
	COMMAND bison -Wcounterexamples -d ../src/main/c/frontend/syntactic-analysis/BisonGrammar.y --output=../src/main/c/frontend/syntactic-analysis/BisonParser.c)

# Selects the best strategy according to the available compiler in the system.
# @see https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	message(NOTICE "The C compiler is GCC.")

	# Options for GCC.
	add_compile_options(-fsanitize=address)
	add_compile_options(-O3)
	add_compile_options(-static-libgcc)
	add_compile_options(-std=gnu99)
	add_link_options(-fsanitize=address)

	# Compiles the scanner with Flex.
	add_custom_command(
		OUTPUT ../src/main/c/frontend/lexical-analysis/FlexScanner.c
		COMMAND flex --noyywrap --outfile=../src/main/c/frontend/lexical-analysis/FlexScanner.c --yylineno ../src/main/c/frontend/lexical-analysis/FlexPatterns.l
		DEPENDS ../src/main/c/frontend/syntactic-analysis/BisonParser.c ../src/main/c/frontend/syntactic-analysis/BisonParser.h)

elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	message(NOTICE "The C compiler is Microsoft Visual Studio.")

	# Options for MSVC.
	# add_compile_options(...)

	# Compiles the scanner with Flex (Microsoft Windows compatible).
	add_custom_command(
		OUTPUT ../src/main/c/frontend/lexical-analysis/FlexScanner.c
		COMMAND flex --noyywrap --outfile=../src/main/c/frontend/lexical-analysis/FlexScanner.c --wincompat --yylineno ../src/main/c/frontend/lexical-analysis/FlexPatterns.l
		DEPENDS ../src/main/c/frontend/syntactic-analysis/BisonParser.c ../src/main/c/frontend/syntactic-analysis/BisonParser.h)

else ()
	message(NOTICE "The C compiler is unknown.")
	message(NOTICE "Add an 'elseif' statement to handle this compiler in a more specific way.")

	# Compiles the scanner with Flex.
	add_custom_command(
		OUTPUT ../src/main/c/frontend/lexical-analysis/FlexScanner.c
		COMMAND flex --noyywrap --outfile=../src/main/c/frontend/lexical-analysis/FlexScanner.c --yylineno ../src/main/c/frontend/lexical-analysis/FlexPatterns.l
		DEPENDS ../src/main/c/frontend/syntactic-analysis/BisonParser.c ../src/main/c/frontend/syntactic-analysis/BisonParser.h)

endif ()

# Defines the entry-point of the application, and the source-codes (*.c extension).
# The header files (*.h extension), are automatically included from the source-codes.
add_executable(Compiler
	src/main/c/backend/code-generation/Generator.c
	src/main/c/backend/domain-specific/Calculator.c
	src/main/c/EntryPoint.c
	src/main/c/frontend/lexical-analysis/FlexActions.c
	src/main/c/frontend/lexical-analysis/FlexScanner.c
	src/main/c/frontend/lexical-analysis/LexicalAnalyzerContext.c
	src/main/c/frontend/syntactic-analysis/AbstractSyntaxTree.c
	src/main/c/frontend/syntactic-analysis/BisonActions.c
	src/main/c/frontend/syntactic-analysis/BisonParser.c
	src/main/c/frontend/syntactic-analysis/SyntacticAnalyzer.c
	src/main/c/shared/Environment.c
	src/main/c/shared/Logger.c
	src/main/c/shared/String.c
	# Add more *.c files if needed (otherwise, they won't be compiled).
	# ...
)

# Link final project and libraries.
target_link_libraries(Compiler)
