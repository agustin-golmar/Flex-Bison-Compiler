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


//NUEVO
/* ENUM */
typedef enum StatementType StatementType;
typedef enum EventSpecType EventSpecType;
typedef enum EventPropType EventPropType;

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
	SPEC_DAYLIST,
	SPEC_DAYOFMONTH
}; 

enum EventPropType {
    EVENT_PROP_COLOR,
    EVENT_PROP_DESCRIPTION,
    EVENT_PROP_URL
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
	TimezoneDecl * timezoneDecl;
	ColorList * colorList;
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
	union {
		DayList * dayList;
		int dayOfMonth;
	};
	Time * start;
	Time * end;
};

struct DayList {
	int days[7];
	int count;
};

struct EventBody {
	char * colorId;
	char * description;
	char * url;
};


struct EventProp {
    EventPropType type;
    union {
        char * colorId;
        char * description;
        char * url;
    };
};





/**
 * Node recursive super-duper-trambolik-destructors.
 */


void destroyEventBody(EventBody * body);
void destroyDayList(DayList * dayList);
void destroyTime(Time * time);
void destroyEventSpec(EventSpec * spec);
void destroyOverrideDecl(OverrideDecl * override);
void destroyEventDecl(EventDecl * event);
void destroyStatement(Statement * statement);
void destroyStatements(Statements * statements);
void destroyMonthBlock(MonthBlock * monthBlock);
void destroyMonthBlocks(MonthBlocks * monthBlocks);
void destroyYearBlock(YearBlock * yearBlock);
void destroyColor(Color * color);
void destroyColorDef(ColorDef * colorDef);
void destroyColorList(ColorList * colorList);
void destroyTimezoneDecl(TimezoneDecl * timezoneDecl);
void destroyHeader(Header * header);
void destroyProgram(Program * program);
void destroyString(char * s);

#endif
