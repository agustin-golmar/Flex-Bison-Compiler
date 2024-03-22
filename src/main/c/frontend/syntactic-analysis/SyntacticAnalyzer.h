#ifndef SYNTACTIC_ANALYZER_HEADER
#define SYNTACTIC_ANALYZER_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"

/** Bison imported functions. */

int yylex(void);
void yyerror(const char * string);

/** Initialize module's internal state. */
void initializeSyntacticAnalyzerModule();

/** Shutdown module's internal state. */
void shutdownSyntacticAnalyzerModule();

typedef enum {
	ACCEPT,
	OUT_OF_MEMORY,
	REJECT,
	UNKNOWN_ERROR
} SyntacticAnalysisStatus;

/**
 * Retrieves the current compiler state. The state must be accessed during the
 * parsing phase (i.e., during a call to the "parse" function). Outside of the
 * parse, the state is set to NULL.
 */
CompilerState * currentCompilerState();

/**
 * Executes the parsing phase of the compiler.
 */
SyntacticAnalysisStatus parse(CompilerState * compilerState);

#endif
