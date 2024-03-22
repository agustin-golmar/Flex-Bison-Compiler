#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/support/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

Constant * IntegerConstantSemanticAction(const int integer);
Expression * AdditionExpressionSemanticAction(Expression * left, Expression * right);
Expression * DivisionExpressionSemanticAction(Expression * left, Expression * right);
Expression * FactorExpressionSemanticAction(Factor * factor);
Expression * MultiplicationExpressionSemanticAction(Expression * left, Expression * right);
Expression * SubtractionExpressionSemanticAction(Expression * left, Expression * right);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression);

#endif
