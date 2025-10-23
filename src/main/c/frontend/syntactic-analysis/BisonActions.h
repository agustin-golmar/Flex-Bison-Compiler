#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions.
 */

// Constant * IntegerConstantSemanticAction(const int value);
// Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
// Expression * FactorExpressionSemanticAction(Factor * factor);
// Factor * ConstantFactorSemanticAction(Constant * constant);
// Factor * ExpressionFactorSemanticAction(Expression * expression);
// Program * ExpressionProgramSemanticAction(Expression * expression);

// Constant * TestConstantSemanticAction();
// Program * EventSemanticAction(int value);


EventProp * ColorSemanticAction(char * colorId); 
EventProp * DescriptionSemanticAction(char * description);
EventProp * UrlSemanticAction(char * url);
EventBody * EventBodySingleSemanticAction(EventProp * prop);
EventBody * EventBodyAppendSemanticAction(EventBody * body, EventProp * prop);
DayList * SingleDayStringSemanticAction(const char * dayName);
DayList * AppendDayStringSemanticAction(DayList * dayList, const char * dayName);
Time * TimeSemanticAction(char * timeString);
EventSpec * DayListSpecSemanticAction(DayList * dayList, Time * start, Time * end);
EventSpec * DayOfMonthSpecSemanticAction(int dayOfMonth, Time * start, Time * end);
OverrideDecl * CreateOverrideSemanticAction(char * identifier, EventBody * body);
EventDecl * CreateEventSemanticAction(char * indentifier, EventSpec * spec, EventBody * body);
Statement * OverrideStatementSemanticAction(OverrideDecl * override);
Statement * EventStatementSemanticAction(EventDecl * event);
Statements * SingleStatementListSemanticAction(Statement * statement);
Statements * AppendStatementSemanticAction(Statements * statements, Statement * statement);
MonthBlock * StatementsMonthIntegerSemanticAction(int month, Statements * statements);
MonthBlock * StatementsMonthNameSemanticAction(char * monthName, Statements * statements);
MonthBlocks * SingleMonthBlockSemanticAction(MonthBlock * monthBlock);
MonthBlocks * AppendMonthBlockSemanticAction(MonthBlocks * monthBlocks, MonthBlock * monthBlock);
YearBlock * YearBlockSemanticAction(int year, MonthBlocks * monthBlocks);
ColorDef * DefineColorSemanticAction(char * identifier, char * hexColor);
ColorList * SingleColorDefSemanticAction(ColorDef * colorDef);
ColorList * AppendColorDefSemanticAction(ColorList * colorList, ColorDef * colorDef);
TimezoneDecl * CreateTimezoneSemanticAction(char * timezone);
Header * createHeaderSemanticAction(TimezoneDecl * timezoneDecl, ColorList * colorList);
Program * CreateProgramSemanticAction(Header * header, YearBlock * yearBlock);

#endif
