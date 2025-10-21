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
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus KeyWordLexemeAction(TokenLabel label);
extern CompilationStatus BeginOfNewParameterLexemeAction();
extern CompilationStatus ObjectAccessExpressionLexemeAction();
extern CompilationStatus BracketsLexemeAction(TokenLabel label);
extern CompilationStatus PunctuatorLexemeAction(TokenLabel label);
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus FieldAccessExpressionLexemeAction();

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"//".*                              { return IgnoredLexemeAction(); }
"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

"="								    { return ArithmeticOperatorLexemeAction(ASSIGN); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }

"{"                                 { return BracketsLexemeAction(OPEN_BRACE); }
"}"                                 { return BracketsLexemeAction(CLOSE_BRACE); }

";"                                 { return PunctuatorLexemeAction(SEMICOLON); }
","                                 { return PunctuatorLexemeAction(COMMA); }

[[:digit:]]+						{ return IntegerLexemeAction(); }

"class"							    { return KeyWordLexemeAction(CLASS); }
"new"								{ return KeyWordLexemeAction(NEW); }
"return"							{ return KeyWordLexemeAction(RETURN); }
"private"							{ return KeyWordLexemeAction(PRIVATE); }
"public"							{ return KeyWordLexemeAction(PUBLIC); }
"this"                              { return KeyWordLexemeAction(THIS); }
"static"                            { return KeyWordLexemeAction(STATIC); }
"constructor"                       { return KeyWordLexemeAction(CONSTRUCTOR); }
"destructor"                        { return KeyWordLexemeAction(DESTRUCTOR); }
"int"								{ return KeyWordLexemeAction(INT); }
"void"								{ return KeyWordLexemeAction(VOID); }
"char"                              { return KeyWordLexemeAction(CHAR); }

"->"                                { return ObjectAccessExpressionLexemeAction(); }
"."                                 { return FieldAccessExpressionLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }

[[:alpha:]][[:alnum:]]*             { return IdentifierLexemeAction(); }

.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
