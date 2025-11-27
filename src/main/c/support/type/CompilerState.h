#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

#include "../data-structures/HashMapADT.h"
#include "../data-structures/stack/stack.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
/**
 * The global state of the compiler. Should transport every data structure
 * needed across the different phases of a compilation.
 */

typedef struct {
  int id;
  HashMapADT symbols;
} Scope;

typedef struct {
	/**
	 * The root node of the AST.
	 */
	void * abstractSyntaxtTree;

	// TODO: Add a symbol table.
	StackADT scopeStack; 

	HashMapADT globalTable;

	Scope * currentScope;

	// TODO: Add an stack to handle nested scopes.
	// TODO: Add more configuration.
	// TODO: Add whatever you need.
	// TODO: ...
} CompilerState;

#endif
