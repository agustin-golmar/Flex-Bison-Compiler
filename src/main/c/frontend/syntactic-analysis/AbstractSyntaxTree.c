#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

// void destroyProgram(Program * program) {
// 	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
// 	if (program != NULL) {
// 		destroyStatement(program->firstStatement);
// 		free(program);
// 	}
// }


/* Nuevos destructores*/
void destroyEventBody(EventBody * body) {
    if (body == NULL) return;

    if (body->colorId != NULL)
        free(body->colorId);

    if (body->description != NULL)
        free(body->description);

    if (body->url != NULL)
        free(body->url);

    free(body);
}

void destroyDayList(DayList * dayList) {
    if (dayList == NULL)
        return;
    free(dayList);
}


void destroyOverrideDecl(OverrideDecl * override) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (override == NULL)
        return;
	free(override->identifier);
	destroyEventBody(override->eventBody);
	free(override);
}

void destroyEventDecl(EventDecl * event) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (event == NULL)
        return;
	free(event->identifier);
	destroyEventSpec(event->eventSpec);
	destroyEventBody(event->eventBody);
	free(event);
}

void destroyStatement(Statement * statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statement == NULL)
        return;

    switch (statement->type) {
        case STATEMENT_EVENT:
            destroyEventDecl(statement->eventDecl);
            break;
        case STATEMENT_OVERRIDE:
            destroyOverrideDecl(statement->OverrideDecl);
            break;
        default:
            break;
    }

    destroyStatement(statement->nextStatement);

    free(statement);
}

void destroyStatements(Statements * statements) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statements == NULL)
        return;

    destroyStatement(statements->firstStatement);
    free(statements);
}

void destroyMonthBlock(MonthBlock * monthBlock) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (monthBlock == NULL)
        return;

    destroyStatements(monthBlock->statements);
    free(monthBlock);
}

void destroyMonthBlocks(MonthBlocks * monthBlocks) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (monthBlocks == NULL)
        return;

    for (int i = 0; i < 12; i++) {
        if (monthBlocks->months[i] != NULL) {
            destroyMonthBlock(monthBlocks->months[i]);
        }
    }
    free(monthBlocks);
}

void destroyYearBlock(YearBlock * yearBlock) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (yearBlock == NULL){
		return;
	}
	destroyMonthBlocks(yearBlock->monthBlocks);
	free(yearBlock);
}

void destroyColor(Color * color){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(color == NULL){
		return;
	}
	free(color->name);
	free(color->hexValue);
	free(color);
}

void destroyColorDef(ColorDef * colorDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (colorDef == NULL)
        return;

    destroyColor(colorDef->color);
    destroyColorDef(colorDef->next);
    free(colorDef);
}

void destroyColorList(ColorList * colorList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (colorList == NULL)
        return;

    destroyColorDef(colorList->firstColorDef);
    free(colorList);
}

void destroyTimezoneDecl(TimezoneDecl * timezoneDecl){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(timezoneDecl == NULL){
		return;
	}
	free(timezoneDecl->timezone);
	free(timezoneDecl);
}

void destroyHeader(Header * header) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (header == NULL)
        return;

    if (header->timezoneDecl != NULL)
        destroyTimezoneDecl(header->timezoneDecl);

    if (header->colorList != NULL)
        destroyColorList(header->colorList);

    free(header);
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(program == NULL){
		return;
	}
	if (program->header != NULL)
        destroyHeader(program->header);

	destroyYearBlock(program->yearBlock);
    free(program);
}