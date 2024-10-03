%{

#include "FlexActions.h"
#include "Felx2Actions.h"

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
%s ACTION
%s ACTIONDOOER

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"\""								                            { BEGIN(ACTION); BeginActionLexeme(createLexicalAnalyzerContext()); }
<ACTION> [a-zA-Z]+                                              { return ActionLexeme(createLexicalAnalyzerContext()); } // return the name of the action, name not empty
<ACTION>"\""                                                    { EndActionLexeme(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
"->"								                            { BEGIN(ACTIONDOOER); BeginActionDooerLexeme(createLexicalAnalyzerContext()); }
<ACTIONDOOER> [a-zA-Z]*                                         { return ActionReturner(createLexicalAnalyzerContext()) } // acction defined in felx2actions
<ACTIONDOOER>";"					                            { EndActionDooerLexeme(createLexicalAnalyzerContext()); BEGIN(INITIAL); }


<INITIAL, ACTION, ACTIONDOOER>[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
[ \t\n\r]                                                       { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
/**
* Our default is included in "." so we dont added.
**/
<INITIAL, ACTION, ACTIONDOOER>.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); } 

%% 

#include "FlexExport.h"
