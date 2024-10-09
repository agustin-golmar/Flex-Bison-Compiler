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

"[Rr]egex"                                      { BEGIN(REGEX); BeginRegexLexeme(createLexicalAnalyzerContext()); }
<REGEX> [a-zA-Z]                                { BEGIN(REGEX_NAME); BeginRegexName(createLexicalAnalyzerContext()); }

<REGEX_NAME> [[:space:]]+                       { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }
<REGEX_NAME> [a-zA-Z0-9_$]*                     { return RegexNameLexeme(createLexicalAnalyzerContext(), REGEX_ID); BEGIN(REGEX_PRECONTENT); }

<REGEX_PRECONTENT> "["                          { BEGIN(REGEX_CONTENT); BeginRegexContent(createLexicalAnalyzerContext()); }
<REGEX_PRECONTENT> [^[:space:]]                 { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }

<REGEX_CONTENT> [[:space:]]+                    { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }
<REGEX_CONTENT> [a-z]                           { return RegexMinLexeme(createLexicalAnalyzerContext(), MIN); }
<REGEX_CONTENT> [A-Z]                           { return RegexMayuscLexeme(createLexicalAnalyzerContext(), MAYUSC); }
<REGEX_CONTENT> [0-9]                           { return RegexDigitLexeme(createLexicalAnalyzerContext(), DIGIT); }
<REGEX_CONTENT> "-"                             { return RegexRangerLexeme(createLexicalAnalyzerContext(), RANGER); }
<REGEX_CONTENT> [^\-\[\]\\]                     { return RegexSimbolLexeme(createLexicalAnalyzerContext(), SIMBOL); }
<REGEX_CONTENT> "\\"[\^\-\[\]\\]                { return RegexEscapedLexeme(createLexicalAnalyzerContext(), SIMBOL); }
<REGEX_CONTENT> "]"                             { BEGIN(REGEX); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"\""                                            { BEGIN(LEXEME); BeginLexeme(createLexicalAnalyzerContext()); }
<LEXEME> [^"]+                                  { return NameLexeme(createLexicalAnalyzerContext(), STRING); }   
<LEXEME> "\""                                   { EndLexeme(createLexicalAnalyzerContext()); BEGIN(PRE_ACTION); }

"["                                             { BEGIN(REGEX_LEXEME); BeginRegexContent(createLexicalAnalyzerContext()); }
<REGEX_LEXEME> [[:space:]]+                     { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }
<REGEX_LEXEME> [a-z]                            { return RegexMinLexeme(createLexicalAnalyzerContext(), MIN); }
<REGEX_LEXEME> [A-Z]                            { return RegexMayuscLexeme(createLexicalAnalyzerContext(), MAYUSC); }
<REGEX_LEXEME> [0-9]                            { return RegexDigitLexeme(createLexicalAnalyzerContext(), DIGIT); }
<REGEX_LEXEME> "-"                              { return RegexRangerLexeme(createLexicalAnalyzerContext(), RANGER); }
<REGEX_LEXEME> [^\-\[\]\\]                      { return RegexSimbolLexeme(createLexicalAnalyzerContext(), SIMBOL); }
<REGEX_LEXEME> "\\"[\^\-\[\]\\]                 { return RegexEscapedLexeme(createLexicalAnalyzerContext(), SIMBOL); }
<REGEX_LEXEME> "]"                              { BEGIN(PRE_ACTION); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"{"                                             { BEGIN(OUR_REGEX_LEXEME); BeginRegexContent(createLexicalAnalyzerContext()); }
<OUR_REGEX_LEXEME> [[:space:]]+                 { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }
<OUR_REGEX_LEXEME> [a-z]*                       { return RegexNameLexeme(createLexicalAnalyzerContext(), REGEX_ID); }
<OUR_REGEX_LEXEME> "}"                          { BEGIN(PRE_ACTION); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<PRE_ACTION> "->"                               { IgnoredLexemeAction(createLexicalAnalyzerContext()); BEGIN(ACTION); }
<ACTION> [a-zA-Z_$]                             { BEGIN(SIMPLE_ACTION); BeginSimpleActionLexeme(createLexicalAnalyzerContext()); }
<ACTION> "{".*"}"                               { return FunctionLexeme(createLexicalAnalyzerContext(), FUNCTION); }

<SIMPLE_ACTION> [[:space:]]+                    { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }
<SIMPLE_ACTION> [a-zA-Z_$]*                     { return ActionNameLexeme(createLexicalAnalyzerContext(), ACTION_ID); }

[[:space:]]+                                    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
";"                                             { CleanIgnoredLexemes(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
"\n"                                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"\r"                                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.                                               { return UnknownLexemeAction(createLexicalAnalyzerContext(), UNKNOWN); }

%%

#include "FlexExport.h"

