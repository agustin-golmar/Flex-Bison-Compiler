#ifndef SHARED_HEADER
#define SHARED_HEADER

#include <stdio.h>
#include "../semantic-analysis/abstract-syntax-tree.h"

// Descriptor del archivo de entrada que utiliza Bison.
extern FILE * yyin;

// Descriptor del archivo de salida que utiliza Bison.
extern FILE * yyout;

// La cantidad de caracteres en el lexema "yytext". Se debe leer en las
// acciones de Flex, no en Bison.
extern int yyleng;

// Variable global que contiene el número de la línea analizada.
extern int yylineno;

// Token actual en el tope de la pila del analizador Bison.
extern char * yytext;

// Función global de manejo de errores en Bison.
extern void yyerror(const char * string);

// Función global del analizador léxico Flex.
extern int yylex(void);

// Función global del analizador sintáctico Bison.
extern int yyparse(void);

// Emular tipo "boolean".
typedef enum {

	false = 0,
	true = 1
} boolean;

// El tipo de los tokens emitidos por Flex.
typedef int token;

// Estado global de toda la aplicación.
typedef struct {

	// Indica si la compilación tuvo problemas hasta el momento.
	boolean succeed;

	// Indica el resultado de la compilación (para la calculadora).
	int result;

	// El nodo raíz del AST (se usará cuando se implemente el backend).
	Program * program;

	// Agregar lo que sea necesario para el compilador.
	// Agregar una pila para manipular scopes.
	// Agregar una tabla de símbolos.
	// ...

} CompilerState;

// El estado se define e inicializa en el archivo "main.c".
extern CompilerState state;

#endif
