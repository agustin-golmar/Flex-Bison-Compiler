#include "BisonActions.h"

static int DayOfWeekStringToNumber(const char * dayName);
static DayList * SingleDaySemanticAction(int day);
static DayList * AppendDaySemanticAction(DayList * dayList, int day);

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

// Constant * IntegerConstantSemanticAction(const int value) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Constant * constant = calloc(1, sizeof(Constant));
// 	constant->value = value;
// 	return constant;
// }

// Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Expression * expression = calloc(1, sizeof(Expression));
// 	expression->leftExpression = leftExpression;
// 	expression->rightExpression = rightExpression;
// 	expression->type = type;
// 	return expression;
// }

// Expression * FactorExpressionSemanticAction(Factor * factor) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Expression * expression = calloc(1, sizeof(Expression));
// 	expression->factor = factor;
// 	expression->type = FACTOR;
// 	return expression;
// }

// Factor * ConstantFactorSemanticAction(Constant * constant) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Factor * factor = calloc(1, sizeof(Factor));
// 	factor->constant = constant;
// 	factor->type = CONSTANT;
// 	return factor;
// }

// Factor * ExpressionFactorSemanticAction(Expression * expression) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Factor * factor = calloc(1, sizeof(Factor));
// 	factor->expression = expression;
// 	factor->type = EXPRESSION;
// 	return factor;
// }

// Program * ExpressionProgramSemanticAction(Expression * expression) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Program * program = calloc(1, sizeof(Program));
// 	//program->expression = expression;
// 	_compilerState->abstractSyntaxtTree = program;
// 	return program;
// }

// _________________________
// -----------NUEVO---------
// ________________________


// Constant * TestConstantSemanticAction() {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Constant * constant = calloc(1, sizeof(Constant));
// 	constant->value = 1;
// 	return constant;
// }

// //testeando
// Program * EventSemanticAction(int value) {
// 	_logSyntacticAnalyzerAction(__FUNCTION__);
// 	Factor * factor = ConstantFactorSemanticAction(IntegerConstantSemanticAction(1));
// 	Expression * expression = FactorExpressionSemanticAction(factor);
// 	return ExpressionProgramSemanticAction(expression);
// }


//NUEVONUEVO

EventProp * ColorSemanticAction(char * colorId) { 
    _logSyntacticAnalyzerAction(__FUNCTION__);
    EventProp * prop = calloc(1, sizeof(EventProp));
    prop->type = EVENT_PROP_COLOR;
    prop->colorId = colorId;
    return prop;
}

EventProp * DescriptionSemanticAction(char * description) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    EventProp * prop = calloc(1, sizeof(EventProp));
    prop->type = EVENT_PROP_DESCRIPTION;
    prop->description = description;
    return prop;
}

EventProp * UrlSemanticAction(char * url) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    EventProp * prop = calloc(1, sizeof(EventProp));
    prop->type = EVENT_PROP_URL;
    prop->url = url;
    return prop;
}


EventBody * EventBodySingleSemanticAction(EventProp * prop) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    EventBody * body = calloc(1, sizeof(EventBody));

    switch (prop->type) {
        case EVENT_PROP_COLOR: body->colorId = prop->colorId; break;
        case EVENT_PROP_DESCRIPTION: body->description = prop->description; break;
        case EVENT_PROP_URL: body->url = prop->url; break;
    }

    free(prop); 
    return body;
}

EventBody * EventBodyAppendSemanticAction(EventBody * body, EventProp * prop) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    switch (prop->type) {
        case EVENT_PROP_COLOR:
            if (body->colorId != NULL) logError(_logger, "Color already defined in event body");
            body->colorId = prop->colorId;
            break;
        case EVENT_PROP_DESCRIPTION:
            if (body->description != NULL) logError(_logger, "Description already defined in event body");
            body->description = prop->description;
            break;
        case EVENT_PROP_URL:
            if (body->url != NULL) logError(_logger, "URL already defined in event body");
            body->url = prop->url;
            break;
    }
    free(prop);
    return body;
}


static int DayOfWeekStringToNumber(const char * dayName) {
    if (dayName == NULL) {
        return 0; // 0 = inválido
    }

    if (strcmp(dayName, "Monday") == 0) return 1;
    if (strcmp(dayName, "Tuesday") == 0) return 2;
    if (strcmp(dayName, "Wednesday") == 0) return 3;
    if (strcmp(dayName, "Thursday") == 0) return 4;
    if (strcmp(dayName, "Friday") == 0) return 5;
    if (strcmp(dayName, "Saturday") == 0) return 6;
    if (strcmp(dayName, "Sunday") == 0) return 7;

    return 0; // inválido
}

DayList * SingleDayStringSemanticAction(const char * dayName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    int day = DayOfWeekStringToNumber(dayName);
    if (day == 0) return NULL; // error
	free(dayName);
    return SingleDaySemanticAction(day);
}

DayList * AppendDayStringSemanticAction(DayList * dayList, const char * dayName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    int day = DayOfWeekStringToNumber(dayName);
    if (day == 0) return dayList; // error
	free(dayName);
    return AppendDaySemanticAction(dayList, day);
}

static DayList * SingleDaySemanticAction(int day) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DayList * dayList = calloc(1, sizeof(DayList));
    dayList->days[day - 1] = day;
    dayList->count = 1;

    return dayList;
}

static DayList * AppendDaySemanticAction(DayList * dayList, int day) {
    _logSyntacticAnalyzerAction(__FUNCTION__);

    int index = day - 1;
    if (dayList->days[index] != 0) {
        return dayList;
    }
    dayList->days[index] = day;
    dayList->count++;
    return dayList;
}

Time * TimeSemanticAction(char * timeString) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    int hour, minute;
    sscanf(timeString, "%d:%d", &hour, &minute); 
    Time * time = calloc(1, sizeof(Time));
    time->hour = hour;
    time->minute = minute;
	free(timeString);
    return time;
}


EventSpec * DayListSpecSemanticAction(DayList * dayList, Time * start, Time * end){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EventSpec * spec = calloc(1, sizeof(EventSpec));
	spec->type = SPEC_DAYLIST;
	spec->dayList = dayList;
	spec->start = start;
	spec->end = end;
	return spec;
}

EventSpec * DayOfMonthSpecSemanticAction(int dayOfMonth, Time * start, Time * end){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EventSpec * spec = calloc(1, sizeof(EventSpec));
	spec->type = SPEC_DAYOFMONTH;
	spec->dayOfMonth = dayOfMonth;
	spec->start = start;
	spec->end = end;
	return spec;
}


OverrideDecl * CreateOverrideSemanticAction(char * identifier, EventBody * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	OverrideDecl * override = calloc(1, sizeof(OverrideDecl));
	override->identifier = identifier;
	override->eventBody = body;
	return override;
}

EventDecl * CreateEventSemanticAction(char * identifier, EventSpec * spec, EventBody * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EventDecl * event = calloc(1, sizeof(EventDecl));
	event->identifier = identifier;
	event->eventSpec = spec;
	event->eventBody = body;
	return event;
}

Statement * OverrideStatementSemanticAction(OverrideDecl * override) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->nextStatement = NULL;
	statement->type = STATEMENT_OVERRIDE;
	statement->OverrideDecl = override;
	return statement;
}

Statement * EventStatementSemanticAction(EventDecl * event) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->nextStatement = NULL;
	statement->type = STATEMENT_EVENT;
	statement->eventDecl = event;
	return statement;
}

Statements * SingleStatementListSemanticAction(Statement * statement) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statements * statements = calloc(1, sizeof(Statements));
    statements->firstStatement = statement;
    return statements;
}

Statements * AppendStatementSemanticAction(Statements * statements, Statement * statement) {
    _logSyntacticAnalyzerAction(__FUNCTION__);

    // if (statements == NULL) {
    //     // fallback defensivo
    //     statements = calloc(1, sizeof(Statements));
    //     statements->firstStatement = statement;
    //     return statements;
    // }

    Statement * current = statements->firstStatement;
    while (current->nextStatement != NULL) {
        current = current->nextStatement;
    }
    current->nextStatement = statement;
    return statements;
}

MonthBlock * StatementsMonthIntegerSemanticAction(int month, Statements * statements) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MonthBlock * monthBlock = calloc(1, sizeof(MonthBlock));
	monthBlock->month = month;
	monthBlock->statements = statements;
	return monthBlock;
}

MonthBlock * StatementsMonthNameSemanticAction(char * monthName, Statements * statements) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	int month = 0;
	if (strcmp(monthName, "January") == 0) {
        month = 1;
    } else if (strcmp(monthName, "February") == 0) {
        month = 2;
    } else if (strcmp(monthName, "March") == 0) {
        month = 3;
    } else if (strcmp(monthName, "April") == 0) {
        month = 4;
    } else if (strcmp(monthName, "May") == 0) {
        month = 5;
    } else if (strcmp(monthName, "June") == 0) {
        month = 6;
    } else if (strcmp(monthName, "July") == 0) {
        month = 7;
    } else if (strcmp(monthName, "August") == 0) {
        month = 8;
    } else if (strcmp(monthName, "September") == 0) {
        month = 9;
    } else if (strcmp(monthName, "October") == 0) {
        month = 10;
    } else if (strcmp(monthName, "November") == 0) {
        month = 11;
    } else if (strcmp(monthName, "December") == 0) {
        month = 12;
    } else {
        logError(_logger, "Unknown month name in StatementsMonthNameSemanticAction");
        return NULL;
    }

	free(monthName);
	return StatementsMonthIntegerSemanticAction(month, statements);
}

MonthBlocks * SingleMonthBlockSemanticAction(MonthBlock * monthBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    MonthBlocks * monthBlocks = calloc(1, sizeof(MonthBlocks));
    int index = monthBlock->month - 1;
    monthBlocks->months[index] = monthBlock;
    return monthBlocks;
}

MonthBlocks * AppendMonthBlockSemanticAction(MonthBlocks * monthBlocks, MonthBlock * monthBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    // if (monthBlocks == NULL) {
    //     return SingleMonthBlockSemanticAction(monthBlock);
    // }
    int index = monthBlock->month - 1;
    if (monthBlocks->months[index] != NULL) {
        logError(_logger, "Duplicate month declaration in AppendMonthBlockSemanticAction");
		destroyMonthBlock(monthBlock);
        return monthBlocks;
    }
    monthBlocks->months[index] = monthBlock;
    return monthBlocks;
}

YearBlock * YearBlockSemanticAction(int year, MonthBlocks * monthBlocks) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	YearBlock * yearBlock = calloc(1, sizeof(YearBlock));
	yearBlock->year = year;
	yearBlock->monthBlocks = monthBlocks;
	return yearBlock;
}

ColorDef * DefineColorSemanticAction(char * identifier, char * hexColor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ColorDef * colorDef = calloc(1, sizeof(ColorDef));
	Color * color = calloc(1, sizeof(Color));
	color->name = identifier;
	color->hexValue = hexColor;
	colorDef->color = color;
	colorDef->next = NULL;
	return colorDef;
}

ColorList * SingleColorDefSemanticAction(ColorDef * colorDef) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ColorList * colorList = calloc(1, sizeof(ColorList));
    colorList->firstColorDef = colorDef;
    return colorList;
}

ColorList * AppendColorDefSemanticAction(ColorList * colorList, ColorDef * colorDef) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ColorDef * current = colorList->firstColorDef;
    while (current->next != NULL)
        current = current->next;

    current->next = colorDef;
    return colorList;
}

TimezoneDecl * CreateTimezoneSemanticAction(char * timezone) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TimezoneDecl * timezoneDecl = calloc(1, sizeof(TimezoneDecl));
	timezoneDecl->timezone = timezone;
	return timezoneDecl;
}

Header * createHeaderSemanticAction(TimezoneDecl * timezoneDecl, ColorList * colorList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Header * header = calloc(1, sizeof(Header));
    header->timezoneDecl = timezoneDecl;
    header->colorList = colorList;
    return header;
}

Program * CreateProgramSemanticAction(Header * header, YearBlock * yearBlock) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->header = header;
	program->yearBlock = yearBlock;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

