#ifndef CALCULATOR_HEADER
#define CALCULATOR_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <limits.h>

/** Initialize module's internal state. */
void initializeCalculatorModule();

/** Shutdown module's internal state. */
void shutdownCalculatorModule();

/**
 * The result of a computation. It's considered valid only if "succeed" is
 * true.
 */
typedef struct {
	boolean succeed;
	int value;
} ComputationResult;

typedef ComputationResult (*BinaryOperator)(const int, const int);

/** Arithmetic operations. */

ComputationResult add(const int leftAddend, const int rightAddend);
ComputationResult divide(const int dividend, const int divisor);
ComputationResult multiply(const int multiplicand, const int multiplier);
ComputationResult subtract(const int minuend, const int subtract);

/**
 * Computes the final value of a mathematical constant.
 */
ComputationResult computeConstant(Constant * constant);

/**
 * Computes the final value of a mathematical expression.
 */
ComputationResult computeExpression(Expression * expression);

/**
 * Computes the final value of a mathematical factor.
 */
ComputationResult computeFactor(Factor * factor);

#endif
