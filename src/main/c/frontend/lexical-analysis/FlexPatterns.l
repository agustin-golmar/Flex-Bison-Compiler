%{
#include "FlexActions.h"
#include "LexicalAnalyzerContext.h"
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
%x MULTILINE_COMMENT
%x REGEX
%x REGEX_NAME
%x REGEX_PRECONTENT
%x REGEX_CONTENT
%x LEXEME
%x REGEX_LEXEME
%x OUR_REGEX_LEXEME
%x PRE_ACTION
%x ACTION
%x SIMPLE_ACTION
%x FUNCTION_PARAM
%x AFTER_PARAM
%x FUNCTION_BODY
/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%
"/*"								            { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				            { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

/* Para mi, todos los Begin...Lexeme() podrían ser una sola función BeginLexeme(), idem con end */
/* Un dif entre este commit y el anterior muestra que saque los "nombre" del token de aca a las 
funciones que siempre usaban el mismo token, y los que no, los deje aca. (solo sobrevivió en el content */


/* SECCION DE DEFINICION DE REGEX Regex|regex nombre [xxx]x */

"[Rr]egex"                                      { BEGIN(REGEX); BeginRegexLexeme(createLexicalAnalyzerContext()); }
<REGEX> [a-zA-Z]                                { BEGIN(REGEX_NAME); BeginRegexNameLexeme(createLexicalAnalyzerContext()); }

<REGEX_NAME> [[:space:]]+                       { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<REGEX_NAME> [a-zA-Z0-9_$]*                     { return RegexNameLexeme(createLexicalAnalyzerContext(), OUR_REGEX_ID); BEGIN(REGEX_PRECONTENT); }

<REGEX_PRECONTENT> "["                          { BEGIN(REGEX_CONTENT); BeginRegexContent(createLexicalAnalyzerContext()); }
<REGEX_PRECONTENT> [^[:space:]]                 { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

<REGEX_CONTENT> [[:space:]]+                    { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<REGEX_CONTENT> [a-z]                           { return RegexContentLexeme(createLexicalAnalyzerContext(), MIN); }
<REGEX_CONTENT> [A-Z]                           { return RegexContentLexeme(createLexicalAnalyzerContext(), MAYUSC); }
<REGEX_CONTENT> [0-9]                           { return RegexContentLexeme(createLexicalAnalyzerContext(), DIGIT); }
<REGEX_CONTENT> "-"                             { return RegexContentLexeme(createLexicalAnalyzerContext(), RANGER); }
<REGEX_CONTENT> [^\-\[\]\\]                     { return RegexContentLexeme(createLexicalAnalyzerContext(), SIMBOL); }
<REGEX_CONTENT> "\\"[\^\-\[\]\\]                { return RegexContentLexeme(createLexicalAnalyzerContext(), ESCAPED_SIMBOL); }
<REGEX_CONTENT> "]"                             { IgnoredLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }

/* SECCION DE DECLARACION DE "xxxx" (previo al ->) */

"\""                                            { BEGIN(LEXEME); BeginLexeme(createLexicalAnalyzerContext()); }
<LEXEME> [^"]+                                  { return StringLexeme(createLexicalAnalyzerContext(), STR); }   
<LEXEME> "\""                                   { EndStringLexeme(createLexicalAnalyzerContext()); BEGIN(PRE_ACTION); }

/* SECCION DE DECLARACION  REGEX [XXX] (previo al ->) */

"["                                             { BEGIN(REGEX_LEXEME); BeginRegexContent(createLexicalAnalyzerContext()); }
<REGEX_LEXEME> [[:space:]]+                     { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<REGEX_LEXEME> [a-z]                            { return RegexContentLexeme(createLexicalAnalyzerContext(), MIN); }
<REGEX_LEXEME> [A-Z]                            { return RegexContentLexeme(createLexicalAnalyzerContext(), MAYUSC); }
<REGEX_LEXEME> [0-9]                            { return RegexContentLexeme(createLexicalAnalyzerContext(), DIGIT); }
<REGEX_LEXEME> "-"                              { return RegexContentLexeme(createLexicalAnalyzerContext(), RANGER); }
<REGEX_LEXEME> [^\-\[\]\\]                      { return RegexContentLexeme(createLexicalAnalyzerContext(), SIMBOL); }
<REGEX_LEXEME> "\\"[\^\-\[\]\\]                 { return RegexContentLexeme(createLexicalAnalyzerContext(), ESCAPED_SIMBOL); }
<REGEX_LEXEME> "]"                              { BEGIN(PRE_ACTION); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

/* SECCION DE DECLARACION MACROREGEX {nombreRegex} (previo al ->)*/

"{"                                             { BEGIN(OUR_REGEX_LEXEME); BeginRegexContent(createLexicalAnalyzerContext()); }
<OUR_REGEX_LEXEME> [[:space:]]+                 { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<OUR_REGEX_LEXEME> [a-z]*                       { return RegexNameLexeme(createLexicalAnalyzerContext(), OUR_REGEX_ID); }
<OUR_REGEX_LEXEME> "}"                          { BEGIN(PRE_ACTION); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"default"                                       { return DefaultLexeme(createLexicalAnalyzerContext()); }

<PRE_ACTION> "->"                               { IgnoredLexemeAction(createLexicalAnalyzerContext()); BEGIN(ACTION); }
<ACTION> [a-zA-Z_$]                             { BEGIN(SIMPLE_ACTION); BeginSimpleActionLexeme(createLexicalAnalyzerContext()); }
<ACTION> "{"                                    { BEGIN(FUNCTION_BODY); BeginFunctionBodyLexeme(createLexicalAnalyzerContext()); }
<ACTION> "("                                    { BEGIN(FUNCTION_PARAMS); BeginFunctionParamsLexeme(createLexicalAnalyzerContext()); }

/* Seccion para funciones "xxx" -> (....){......}*/

<FUNCTION_PARAM> [[:space:]]+                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<FUNCTION_PARAM> "String"                       { return FunctionParamLexeme(createLexicalAnalyzerContext(), STRING); BEGIN(AFTER_PARAM); }
<FUNCTION_PARAM> "Integer"                      { return FunctionParamLexeme(createLexicalAnalyzerContext(), INTEGER); BEGIN(AFTER_PARAM); }
<FUNCTION_PARAM> "Double"                       { return FunctionParamLexeme(createLexicalAnalyzerContext(), DOUBLE); BEGIN(AFTER_PARAM); }
<FUNCTION_PARAM> ")"[[:space]]*"{"              { BEGIN(FUNCTION_BODY);  BeginFunctionBodyLexeme(createLexicalAnalyzerContext()); }

<AFTER_PARAM> [[:space:]]+                      { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<AFTER_PARAM> ")"[[:space]]*"{"                 { BEGIN(FUNCTION_BODY);  BeginFunctionBodyLexeme(createLexicalAnalyzerContext()); }

<FUNCTION_BODY> [^{}]+                          { return FunctionBodyContentLexeme(createLexicalAnalyzerContext(), BODY); }
<FUNCTION_BODY> "}"                             { EndFunctionBodyLexeme(createLexicalAnalyzerContext()); BEGIN(INITIAL); }

/* Accion simple  "xx" -> xxx */

<SIMPLE_ACTION> [[:space:]]+                    { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<SIMPLE_ACTION> [a-zA-Z_$]*                     { return ActionNameLexeme(createLexicalAnalyzerContext()); }

[[:space:]]+                                    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
";"                                             { EndLineLexeme(createLexicalAnalyserContext()); BEGIN(INITIAL); }
"\n"                                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"\r"                                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.                                               { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"

