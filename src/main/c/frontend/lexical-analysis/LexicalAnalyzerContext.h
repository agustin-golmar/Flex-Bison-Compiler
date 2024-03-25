#ifndef LEXICAL_ANALYZER_CONTEXT_HEADER
#define LEXICAL_ANALYZER_CONTEXT_HEADER

#include <stdlib.h>
#include <string.h>

/**
 * The state of a lexical-analyzer context.
 */
typedef struct {
	unsigned int currentContext;
	unsigned int length;
	char * lexeme;
	unsigned int line;
	union SemanticValue * semanticValue;
} LexicalAnalyzerContext;

/**
 * Creates a new context with the current state of the lexical-analyzer over
 * the lexeme just consumed. The lexeme is a deep-copy of the original,
 * allocated in heap-memory.
 */
LexicalAnalyzerContext * createLexicalAnalyzerContext();

/**
 * Destroy a lexical-analyzer context and its resources.
 */
void destroyLexicalAnalyzerContext(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif
