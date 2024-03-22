#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/* PRIVATE FUNCTIONS */



/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int integer) {
	return NULL;
}

Expression * AdditionExpressionSemanticAction(Expression * left, Expression * right) {
	return NULL;
}

Expression * DivisionExpressionSemanticAction(Expression * left, Expression * right) {
	return NULL;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	return NULL;
}

Expression * MultiplicationExpressionSemanticAction(Expression * left, Expression * right) {
	return NULL;
}

Expression * SubtractionExpressionSemanticAction(Expression * left, Expression * right) {
	return NULL;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	return NULL;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	return NULL;
}

Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression) {
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	compilerState->abstractSyntaxtTree = program;
	compilerState->succeed = true;
	return program;
}
