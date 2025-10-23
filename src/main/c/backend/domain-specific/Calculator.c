#include "Calculator.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownCalculatorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Calculator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeCalculatorModule() {
	_logger = createLogger("Calculator");
	return _shutdownCalculatorModule;
}

/** PRIVATE FUNCTIONS */

// static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type);
// static ComputationResult _invalidBinaryOperator(const int x, const int y);
// static ComputationResult _invalidComputation();

/**
 * Converts and expression type to the proper binary operator. If that's not
 * possible, returns a binary operator that always returns an invalid
 * computation result.
 */
// static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type) {
// 	switch (type) {
// 		case ADDITION: return add;
// 		case DIVISION: return divide;
// 		case MULTIPLICATION: return multiply;
// 		case SUBTRACTION: return subtract;
// 		default:
// 			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
// 			return _invalidBinaryOperator;
// 	}
// }

/**
 * A binary operator that always returns an invalid computation result.
 */
// static ComputationResult _invalidBinaryOperator(const int x, const int y) {
// 	return _invalidComputation();
// }

/**
 * A computation that always returns an invalid result.
 */
// static ComputationResult _invalidComputation() {
// 	ComputationResult computationResult = {
// 		.succeeded = false,
// 		.value = 0
// 	};
// 	return computationResult;
// }

// /** PUBLIC FUNCTIONS */

// ComputationResult add(const int leftAddend, const int rightAddend) {
// 	ComputationResult computationResult = {
// 		.succeeded = true,
// 		.value = leftAddend + rightAddend
// 	};
// 	return computationResult;
// }

// ComputationResult divide(const int dividend, const int divisor) {
// 	const int sign = dividend < 0 ? -1 : +1;
// 	const bool divisionByZero = divisor == 0 ? true : false;
// 	if (divisionByZero) {
// 		logError(_logger, "The divisor cannot be zero (the computation was %d/%d).", dividend, divisor);
// 	}
// 	ComputationResult computationResult = {
// 		.succeeded = divisionByZero ? false : true,
// 		.value = divisionByZero ? (sign * INT_MAX) : (dividend / divisor)
// 	};
// 	return computationResult;
// }

// ComputationResult multiply(const int multiplicand, const int multiplier) {
// 	ComputationResult computationResult = {
// 		.succeeded = true,
// 		.value = multiplicand * multiplier
// 	};
// 	return computationResult;
// }

// ComputationResult subtract(const int minuend, const int subtract) {
// 	ComputationResult computationResult = {
// 		.succeeded = true,
// 		.value = minuend - subtract
// 	};
// 	return computationResult;
// }

// ComputationResult computeConstant(Constant * constant) {
// 	ComputationResult computationResult = {
// 		.succeeded = true,
// 		.value = constant->value
// 	};
// 	return computationResult;
// }

// ComputationResult computeExpression(Expression * expression) {
// 	switch (expression->type) {
// 		case ADDITION:
// 		case DIVISION:
// 		case MULTIPLICATION:
// 		case SUBTRACTION:
// 			ComputationResult leftResult = computeExpression(expression->leftExpression);
// 			ComputationResult rightResult = computeExpression(expression->rightExpression);
// 			if (leftResult.succeeded && rightResult.succeeded) {
// 				BinaryOperator binaryOperator = _expressionTypeToBinaryOperator(expression->type);
// 				return binaryOperator(leftResult.value, rightResult.value);
// 			}
// 			else {
// 				return _invalidComputation();
// 			}
// 		case FACTOR:
// 			return computeFactor(expression->factor);
// 		default:
// 			return _invalidComputation();
// 	}
// }

// ComputationResult computeFactor(Factor * factor) {
// 	switch (factor->type) {
// 		case CONSTANT:
// 			return computeConstant(factor->constant);
// 		case EXPRESSION:
// 			return computeExpression(factor->expression);
// 		default:
// 			return _invalidComputation();
// 	}
// }
//testeo
// ComputationResult executeCalculator(CompilerState * compilerState) {
// 	Program * program = compilerState->abstractSyntaxtTree;
// 	//return computeExpression(program->expression);
// 	ComputationResult computationResult = {
//  		.succeeded = true,
// 		.value = 10
// 	};
// 	// return computeStatement(program->firstStatement);
// 	return computationResult;
// }

// ComputationResult computeStatement(Statement * statement) {
// 	return computeEvent(statement->event);
// }

// ComputationResult computeEvent(Event * event) {
// 	ComputationResult computationResult = {
// 		.succeeded = true,
// 		.value = 10
// 	};
// 	return computationResult;
// }
