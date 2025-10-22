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
/* ENUM */
typedef enum StatementType StatementType;
typedef enum EventSpecType EventSpecType;

/* TERMINAL */
typedef struct Date Date;
typedef struct Time Time;
typedef struct Color Color;

/* NON-TERMINAL */
typedef struct Program Program;
typedef struct Header Header;
typedef struct TimezoneDecl TimezoneDecl;
typedef struct ColorList ColorList;
typedef struct ColorDef ColorDef;
typedef struct YearBlock YearBlock;
typedef struct MonthBlocks MonthBlocks;
typedef struct MonthBlock MonthBlock;
typedef struct Statements Statements;
typedef struct Statement Statement;
typedef struct EventDecl EventDecl;
typedef struct OverrideDecl OverrideDecl;
typedef struct EventSpec EventSpec;
typedef struct DayList DayList;
typedef struct EventBody EventBody;
typedef struct EventProp EventProp;

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

/* ENUM */
enum StatementType{
	STATEMENT_EVENT,
	STATEMENT_OVERRIDE
};


enum EventSpecType {
	NORMAL,
	PUNTUAL,
	RECURRENT
}; 

/* TERMINAL */
struct Date {
    int year;
    int month;
    int day;
};

struct Time {
    int hour;
    int minute;
};

struct Color {
	char * name;
	char * hexValue;
};

/* NON-TERMINAL */
struct Program {
	Header * header;
	YearBlock * yearBlock;
};

struct Header {
	union {
		TimezoneDecl * timezoneDecl;
		ColorList * colorList;
	};
};

struct TimezoneDecl {
	char * timezone; //revisar
};

struct ColorList {
	ColorDef * firstColorDef;
};

struct ColorDef {
	Color * color;
	ColorDef * next;
};

struct YearBlock {
	int year;
	MonthBlocks * monthBlocks;
};

struct MonthBlocks {
	MonthBlock * months[12];
};

struct MonthBlock {
	int month;
	Statements * statements;
};

struct Statements {
	Statement * firstStatement;
};

struct Statement {
	Statement * nextStatement;
	StatementType type;
	union {
		EventDecl * eventDecl;
		OverrideDecl * OverrideDecl;
	};
};

struct EventDecl {
	char * identifier;
	EventSpec * eventSpec;
	EventBody * eventBody;
};

struct OverrideDecl {
	char * identifier;
	EventBody * eventBody;
};

struct EventSpec {
	EventSpecType type;
	DayList * dayList;
	union {
		Time start;
		Time end;
		int recurrence;
		int startingDay;
	};
};

struct DayList {
	int days[7];
	int count;
};

struct EventBody {
	union {
		Color * color;
		char * description;
		char * url;
	};
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
