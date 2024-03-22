#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

#include "Type.h"

/**
 * The global state of the compiler. Should transport every data structure
 * needed across the different phases of a compilation.
 */
typedef struct {
	// The root node of the AST.
	void * abstractSyntaxtTree;

	// A flag that indicates the current state of the compilation so far.
	boolean succeed;

	// TODO: Add an stack to handle nested scopes.
	// TODO: Add a symbol table.
	// TODO: Add configuration.
	// TODO: ...
} CompilerState;

#endif