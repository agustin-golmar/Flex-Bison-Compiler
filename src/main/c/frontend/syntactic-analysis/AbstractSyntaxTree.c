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

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyStatement(program->firstStatement);
		free(program);
	}
}

void destroyEvent(Event * event) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (event != NULL) {
		free(event->name);
		free(event);
	}
}

void destroyStatement(Statement * statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (statement != NULL){
		switch (statement->type){
			case STATEMENT_COLOR_DECLARATION: break;
			case STATEMENT_EVENT:
				destroyEvent(statement->event);
				break;
			case STATEMENT_REPLACE: break;
		}
		free(statement);
	}
}
