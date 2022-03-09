#include "flex-actions.h"

/**
 * Implementación de "flex-rules.h".
 */

TokenID IntegerPatternAction(const char * lexeme) {
	printf("IntegerPatternAction: '%s'.\n", lexeme);
	yylval = atoi(lexeme);
	return INTEGER;
}

void IgnoredPatternAction(const char * lexeme) {
	printf("IgnoredPatternAction: '%s'.\n", lexeme);
	return;
}

TokenID UnknownPatternAction(const char * lexeme) {
	printf("UnknownPatternAction: '%s'.\n", lexeme);
	return YYUNDEF;
}
