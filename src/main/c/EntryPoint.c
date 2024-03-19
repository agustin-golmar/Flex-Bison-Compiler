//#include "backend/code-generation/Generator.h"
//#include "backend/support/Shared.h"
//#include "frontend/syntactic-analysis/BisonParser.h"
//#include <stdio.h>
#include "shared/support/Environment.h"
#include "shared/support/Logger.h"
#include "shared/support/String.h"

// Estado de la aplicación.
//CompilerState state;

/**
 * The main entry-point of the entire application.
 */
const int main(const int count, const char ** arguments) {
	// Inicializar estado de la aplicación.
	// state.program = NULL;
	// state.result = 0;
	// state.succeed = false;

	Logger * logger = createLogger("EntryPoint");

	// Print the arguments of the application.
	for (int k = 0; k < count; ++k) {
		logDebugging(logger, "Argument %d: '%s'", k, arguments[k]);
	}

	destroyLogger(logger);
	return 0;

	// // Compilar el programa de entrada.
	// LogInfo("Compilando...\n");
	// const int result = yyparse();
	// switch (result) {
	// 	case 0:
	// 		// La variable "succeed" es la que setea Bison al identificar el símbolo
	// 		// inicial de la gramática satisfactoriamente.
	// 		if (state.succeed) {
	// 			LogInfo("La compilacion fue exitosa.");
	// 			Generator(state.result);
	// 		}
	// 		else {
	// 			LogError("Se produjo un error en la aplicacion.");
	// 			return -1;
	// 		}
	// 		break;
	// 	case 1:
	// 		LogError("Bison finalizo debido a un error de sintaxis.");
	// 		break;
	// 	case 2:
	// 		LogError("Bison finalizo abruptamente debido a que ya no hay memoria disponible.");
	// 		break;
	// 	default:
	// 		LogError("Error desconocido mientras se ejecutaba el analizador Bison (codigo %d).", result);
	// }
	// LogInfo("Fin.");
	// return result;
}
