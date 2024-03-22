#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/support/Environment.h"
#include "shared/support/Logger.h"
#include "shared/support/String.h"

/**
 * The main entry-point of the entire application.
 */
const int main(const int count, const char ** arguments) {
	Logger * logger = createLogger("EntryPoint");
	initializeFlexActionsModule();
	initializeBisonActionsModule();
	initializeSyntacticAnalyzerModule();

	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}

	// Begin compilation process.
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.succeed = false
	};
	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	if (syntacticAnalysisStatus == ACCEPT) {
		// ...
	}
	else {
		// ...
	}

	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	destroyLogger(logger);
	return syntacticAnalysisStatus;
}
