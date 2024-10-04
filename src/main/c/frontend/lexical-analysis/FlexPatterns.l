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
%x ACTION
%x ACTIONDOOER

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

\"                                            { BEGIN(ACTION); BeginActionLexeme(createLexicalAnalyzerContext()); }
<ACTION> [a-zA-Z]+                            { return NameLexeme(createLexicalAnalyzerContext(), STRING); }
<ACTION> \"                                   { EndActionLexeme(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
"->"                                          { BEGIN(ACTIONDOOER); BeginActionDooerLexeme(createLexicalAnalyzerContext()); }
<ACTIONDOOER> [a-zA-Z]+                       { return ActionNameLexeme(createLexicalAnalyzerContext(), ACTION_ID); }
<ACTIONDOOER> ";"                             { EndActionDooerLexeme(createLexicalAnalyzerContext()); BEGIN(INITIAL); }

<INITIAL, ACTION, ACTIONDOOER> [[:space:]]+   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<INITIAL, ACTION, ACTIONDOOER> .              { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
