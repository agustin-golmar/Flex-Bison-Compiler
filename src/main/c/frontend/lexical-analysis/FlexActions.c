#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static bool _logIgnoredLexemes = true;
static InputBuffer * _inputBuffer = NULL;
static LexicalAnalyzer * _lexicalAnalyzer = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownFlexActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: FlexActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	if (_inputBuffer != NULL) {
		destroyInputBuffer(_inputBuffer);
		_inputBuffer = NULL;
	}
	_lexicalAnalyzer = NULL;
}

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer) {
	_inputBuffer = NULL;
	_lexicalAnalyzer = lexicalAnalyzer;
	_logger = createLogger("FlexActions");
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	return _shutdownFlexActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logTokenAction(const char * actionName, Token * token);

/**
 * Logs a lexical-analyzer action over a token in DEBUGGING level.
 */
static void _logTokenAction(const char * actionName, Token * token) {
	char * _lexeme = escape(token->lexeme);
	logDebugging(_logger, WARNING_COLOR "%s" DEFAULT_COLOR ": Token(context=%d, label=%d, length=%d, lexeme=%s\"%s\"%s, line=%d, semanticValue=%p)",
		actionName,
		token->context,
		token->label,
		token->length,
		INFORMATION_COLOR, _lexeme, DEFAULT_COLOR,
		token->line,
		token->semanticValue);
	free(_lexeme);
	_lexeme = NULL;
}

/* PUBLIC FUNCTIONS */

// CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label) {
// 	Token * token = createToken(_lexicalAnalyzer, label);
// 	_logTokenAction(__FUNCTION__, token);
// 	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
// 	destroyToken(token);
// 	return status;
// }

// CompilationStatus EnterImportExpressionLexemeAction(FlexContext context) {
// 	if (_logIgnoredLexemes) {
// 		Token * token = createToken(_lexicalAnalyzer, OPEN_BRACE);
// 		_logTokenAction(__FUNCTION__, token);
// 		destroyToken(token);
// 	}
// 	enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
// 	return IN_PROGRESS;
// }

// CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context) {
// 	if (_logIgnoredLexemes) {
// 		Token * token = createToken(_lexicalAnalyzer, OPEN_COMMENT);
// 		_logTokenAction(__FUNCTION__, token);
// 		destroyToken(token);
// 	}
// 	enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
// 	return IN_PROGRESS;
// }

CompilationStatus EOFLexemeAction() {
	CompilationStatus status = IN_PROGRESS;
	Token * token = createToken(_lexicalAnalyzer, 0);
	_logTokenAction(__FUNCTION__, token);
	if (!popInputBuffer(_lexicalAnalyzer)) {
		status = pushToken(_lexicalAnalyzer, token);
		FlexContext context = currentLexicalAnalyzerContext(_lexicalAnalyzer);
		if (0 < context) {
			logError(_logger, "The final context is not closed (context=%d).", context);
			status = FAILED;
		}
	}
	destroyToken(token);
	return status;
}

CompilationStatus IgnoredLexemeAction() {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, IGNORED);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus IntegerLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, INTEGER);
	token->semanticValue->integer = atoi(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

// CompilationStatus LeaveImportExpressionLexemeAction() {
// 	pushInputBuffer(_inputBuffer);
// 	leaveLexicalAnalyzerContext(_lexicalAnalyzer);
// 	if (_logIgnoredLexemes) {
// 		Token * token = createToken(_lexicalAnalyzer, CLOSE_BRACE);
// 		_logTokenAction(__FUNCTION__, token);
// 		destroyToken(token);
// 	}
// 	return IN_PROGRESS;
// }

// CompilationStatus LeaveMultilineCommentLexemeAction() {
// 	leaveLexicalAnalyzerContext(_lexicalAnalyzer);
// 	if (_logIgnoredLexemes) {
// 		Token * token = createToken(_lexicalAnalyzer, CLOSE_COMMENT);
// 		_logTokenAction(__FUNCTION__, token);
// 		destroyToken(token);
// 	}
// 	return IN_PROGRESS;
// }

// CompilationStatus ParenthesisLexemeAction(TokenLabel label) {
// 	Token * token = createToken(_lexicalAnalyzer, label);
// 	_logTokenAction(__FUNCTION__, token);
// 	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
// 	destroyToken(token);
// 	return status;
// }

// CompilationStatus SubexpressionLexemeAction() {
// 	Token * token = createToken(_lexicalAnalyzer, IGNORED);
// 	_inputBuffer = createInputBuffer(_lexicalAnalyzer, token->lexeme);
// 	if (_logIgnoredLexemes) {
// 		_logTokenAction(__FUNCTION__, token);
// 	}
// 	destroyToken(token);
// 	return IN_PROGRESS;
// }

CompilationStatus UnknownLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, UNKNOWN);
	_logTokenAction(__FUNCTION__, token);
	destroyToken(token);
	return FAILED;
}

CompilationStatus TokenLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus StringLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, STRING);
	token->semanticValue->strVal = strdup(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus DateLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, DATE);
	token->semanticValue->strVal = strdup(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus TimeLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, TIME);
	token->semanticValue->strVal = strdup(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}


CompilationStatus StringValueLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	token->semanticValue->strVal = strdup(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus DayOfMonthLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, DAY_OF_MONTH);
	token->semanticValue->integer = atoi(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}
