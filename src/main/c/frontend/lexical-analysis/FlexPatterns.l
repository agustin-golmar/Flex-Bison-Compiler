%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


}


/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x IMPORT_EXPRESSION
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
digit               [0-9]
letter              [A-Za-z]
identifier          {letter}({letter}|{digit}|[_\-])*
hour                {digit}{digit}:{digit}{digit}
month_name          (January|February|March|April|May|June|July|August|September|October|November|December)
weekday             (Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|Sunday)
timezone_name       (UTC|GMT|[A-Z]{3,4})
url_pattern         https?:\/\/[A-Za-z0-9\.\-_/]+
string              \"([^\"\\]|\\.)*\"

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}


"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"{"                                 { return LBRACE; }
"}"                                 { return RBRACE; }

"year"                              { return YEAR; }
"month"                             { return MONTH; }
"timezone"                          { return TIMEZONE; }

"event"                             { return EVENT; }
"override"                          { return OVERRIDE; }

"on"                                { return ON; }
"and"                               { return AND; }
"from"                              { return FROM; }
"to"                                { return TO; }
"at"                                { return AT; }
"since"                             { return SINCE; }
"till"                              { return TILL; }

"every"                             { return EVERY; }
"starting"                          { return STARTING; }
"only"                              { return ONLY; }
"if"                                { return IF; }

"day-of-month"                      { return DAY_OF_MONTH; }
"day-of-week"                       { return DAY_OF_WEEK; }

"color"                             { return COLOR; }
"description"                       { return DESCRIPTION; }

{month_name}                        { yylval->strVal = strdup(yytext); return MONTH_NAME; }
{weekday}                           { yylval->strVal = strdup(yytext); return WEEKDAY; }
{hour}                              { yylval->strVal = strdup(yytext); return HOUR; }

{timezone_name}                     { yylval->strVal = strdup(yytext); return TIMEZONE_NAME; }

{url_pattern}                       { yylval->strVal = strdup(yytext); return URL; }

{string}                            {
                                        char *s = strdup(yytext);
                                        s[strlen(s)-1] = '\0';
                                        yylval->strVal = strdup(s+1);
                                        free(s);
                                        return STRING;
                                    }

{identifier}                        { yylval->strVal = strdup(yytext); return IDENTIFIER; }

[[:digit:]]+                        { yylval->intVal = atoi(yytext); return INTEGER; }

.                                   { 
                                      fprintf(stderr, "Caracter inesperado: '%s'\n", yytext);
                                      return UNKNOWN;
                                    }

<<EOF>>                             { return 0; }

{whitespace}						{ return IgnoredLexemeAction(); }


%%


/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
