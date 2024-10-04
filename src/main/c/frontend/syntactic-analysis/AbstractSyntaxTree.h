#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */


typedef enum RegexType RegexType;


typedef struct Action Action;
typedef struct Regex Regex;
typedef struct Program Program;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum RegexType {
	ID,
	DEF
};

struct Regex {
	char * string;
	union {
		char * action_id;
		Action * action_def;
	}
	RegexType type;
}

struct Program {
	Regex * Regex;
};

/**
 * Node recursive destructors.
 */
/*
void releaseConstant(Constant * constant);
void releaseExpression(Expression * expression);
void releaseFactor(Factor * factor);
*/

void releaseRegex(Regex * regex);
void releaseProgram(Program * program);

#endif


