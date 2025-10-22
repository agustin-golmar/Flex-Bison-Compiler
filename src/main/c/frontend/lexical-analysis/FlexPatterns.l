%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus TokenLexemeAction(TokenLabel label);
extern CompilationStatus StringValueLexemeAction(TokenLabel label);
extern CompilationStatus StringLexemeAction();
extern CompilationStatus TimeLexemeAction();
extern CompilationStatus DateLexemeAction();

%}

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
time                {digit}{digit}:{digit}{digit}
date                [0-9]{4}-[0-9]{2}-[0-9]{2}
month_name          (January|February|March|April|May|June|July|August|September|October|November|December)
weekday             (Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|Sunday)
timezone_name       [TODO]
url_pattern         https?:\/\/[A-Za-z0-9\.\-_/]+
string              \"([^\"\\]|\\.)*\"
hex_value           #[0-9A-Fa-f]{6}

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }


"test"              { return TokenLexemeAction(TEST); }

"year"                              { return TokenLexemeAction(YEAR); }
"month"                             { return TokenLexemeAction(MONTH); }
"timezone"                          { return TokenLexemeAction(TIMEZONE); }

"event"                             { return TokenLexemeAction(EVENT); }
"override"                          { return TokenLexemeAction(OVERRIDE); }

"on"                                { return TokenLexemeAction(ON); }
"and"                               { return TokenLexemeAction(AND); }
"from"                              { return TokenLexemeAction(FROM); }
"to"                                { return TokenLexemeAction(TO); }
"at"                                { return TokenLexemeAction(AT); }
"since"                             { return TokenLexemeAction(SINCE); }
"till"                              { return TokenLexemeAction(TILL); }

"every"                             { return TokenLexemeAction(EVERY); }
"starting"                          { return TokenLexemeAction(STARTING); }
"only"                              { return TokenLexemeAction(ONLY); }
"if"                                { return TokenLexemeAction(IF); }

"day-of-month"                      { return TokenLexemeAction(DAY_OF_MONTH); }
"day-of-week"                       { return TokenLexemeAction(DAY_OF_WEEK); }
"color"                             { return TokenLexemeAction(COLOR); }
"description"                       { return TokenLexemeAction(DESCRIPTION); }
"url"                               { return TokenLexemeAction(URL);}

{month_name}                        { return StringValueLexemeAction(MONTH_NAME); }
{weekday}                           { return StringValueLexemeAction(DAY_OF_WEEK); }
{time}                              { return TimeLexemeAction(); }  //revisar, ver si paso la string o el struct ya armado
{date}                              { return DateLexemeAction(); }  //revisar, misma

{timezone_name}                     { return StringValueLexemeAction(TIMEZONE_NAME); }

{url_pattern}                       { return StringValueLexemeAction(URL_PATTERN); } //URL_PATTERN

{string}                            { return StringLexemeAction(); }

{hex_value}                         { return StringValueLexemeAction(HEXVALUE); }

{identifier}                        { return StringValueLexemeAction(IDENTIFIER); }

[[:digit:]]+						{ return IntegerLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
