#include "frontend/syntactic-analysis/bison-parser.h"
#include <stdio.h>

/**
 * Punto de entrada principal del compilador.
 */

const int main(const int argumentCount, const char ** arguments) {
	// Mostrar parámetros recibidos por consola:
	for (int i = 0; i < argumentCount; ++i) {
		printf("Argumento %d: '%s'\n", i, arguments[i]);
	}

	// Compilar el programa de entrada:
	printf("%s\n", "Compilando...");
	const int result = yyparse();
	switch (result) {
		case 0:
			printf("%s\n", "La compilacion fue exitosa.");
			break;
		case 1:
			printf("%s\n", "Bison finalizo debido a un error de sintaxis.");
			break;
		case 2:
			printf("%s\n", "Bison finalizo abruptamente debido a que ya no hay memoria disponible.");
			break;
		default:
			printf("%s (codigo %d).\n", "Error desconocido mientras se ejecutaba el analizador Bison.", result);
	}
	printf("%s\n", "Fin.");
	return result;
}
