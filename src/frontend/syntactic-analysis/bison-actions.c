#include "../../backend/domain-specific/calculator.h"
#include "bison-actions.h"
#include <stdio.h>
#include <string.h>

/**
 * Implementación de "bison-grammar.h".
 */

void yyerror(const char * string) {
	//fprintf(stderr, "FPRINTF ERROR: '%s'.\n", string);
	printf("\nError: [%s] en [%s], linea %d.\n", string, yytext, yylineno);
	printf("Los codigos ASCII son:\n\t");
	const int length = strlen(yytext);
	for (int i = 0; i < length; ++i) {
		printf("<%d>", yytext[i]);
	}
	printf("\n\n");
}

int ProgramGrammarAction(const int value) {
	printf("ProgramGrammarAction(%d)\n", value);
	return value;
}

int AdditionExpressionGrammarAction(const int leftValue, const int rightValue) {
	printf("AdditionExpressionGrammarAction(%d, %d)\n", leftValue, rightValue);
	return Add(leftValue, rightValue);
}

int SubtractionExpressionGrammarAction(const int leftValue, const int rightValue) {
	printf("SubtractionExpressionGrammarAction(%d, %d)\n", leftValue, rightValue);
	return Subtract(leftValue, rightValue);
}

int MultiplicationExpressionGrammarAction(const int leftValue, const int rightValue) {
	printf("MultiplicationExpressionGrammarAction(%d, %d)\n", leftValue, rightValue);
	return Multiply(leftValue, rightValue);
}

int DivisionExpressionGrammarAction(const int leftValue, const int rightValue) {
	printf("DivisionExpressionGrammarAction(%d, %d)\n", leftValue, rightValue);
	return Divide(leftValue, rightValue);
}

int FactorExpressionGrammarAction(const int value) {
	printf("FactorExpressionGrammarAction(%d)\n", value);
	return value;
}

int ExpressionFactorGrammarAction(const int value) {
	printf("ExpressionFactorGrammarAction(%d)\n", value);
	return value;
}

int ConstantFactorGrammarAction(const int value) {
	printf("ConstantFactorGrammarAction(%d)\n", value);
	return value;
}

int IntegerConstantGrammarAction(const int value) {
	printf("IntegerConstantGrammarAction(%d)\n", value);
	return value;
}
