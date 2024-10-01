%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"\""								{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"->"								{ return LambdaDefinitionLexemeAction(createLexicalAnalyzerContext(), MUL); }
";"									{ return EndLineLexemeAction(createLexicalAnalyzerContext(), DIV); }


[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%% 

#include "FlexExport.h"
