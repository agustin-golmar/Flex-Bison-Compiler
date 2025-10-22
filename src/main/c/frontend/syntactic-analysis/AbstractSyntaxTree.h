#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;

//NUEVO
typedef struct Statement Statement;
typedef struct Program Program;
typedef struct Event Event;
typedef struct Replace Replace;
typedef struct ColorDeclaration ColorDeclaration;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

// ________________________________
// ________________________________
//          NUEVO
// ________________________________
// ________________________________

typedef enum {
	STATEMENT_COLOR_DECLARATION,
	STATEMENT_EVENT,
	STATEMENT_REPLACE
} StatementType;

// struct Statement {
// 	StatementType type;
// 	union {
// 		ColorDeclaration *colorDeclaration;
// 		Event *event;
// 		Replace *replace;
// 	};
// 	struct Statement *next;
// };

struct Statement {
	StatementType type;
	union {
		ColorDeclaration *colorDeclaration;
		Event *event;
		Replace *replace;
	};
};

struct Program {
	Statement * firstStatement;
};

struct ColorDeclaration {
	char * name;
	char * hexValue;
};

// struct Event {
// 	char *title;
// 	char *date;
// 	char *time;
// 	char *colorName;
// 	char *repeat;
// 	char *which;
// };

struct Event {
	int value;
};

struct Replace {
	char *targetTitle;
	char *date;
	char *newTitle;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);

void destroyEvent(Event * event);
void destroyStatement(Statement * statement);

#endif
