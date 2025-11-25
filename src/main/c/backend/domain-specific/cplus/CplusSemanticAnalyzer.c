/* src/main/c/backend/domain-specific/semantic/CplusSemanticAnalyzer.c */

#include "CplusSemanticAnalyzer.h"
#include "../../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../../support/logging/Logger.h"
#include "../../../support/type/ModuleDestructor.h"
#include "../../../support/type/CompilerState.h"
#include "../../../support/data-structures/HashMapADT.h"
#include "../../../support/data-structures/stack/stack.h"

#include <stdlib.h>
#include <string.h>

/* MODULE INTERNAL STATE */

/* -------------------------------------------------------------------------- */
/* Result helpers                                                              */
/* -------------------------------------------------------------------------- */

static ComputationResult _invalidComputation() {
  ComputationResult r = { .succeeded = false };
  return r;
}

static ComputationResult _ok() {
  ComputationResult r = { .succeeded = true };
  return r;
}

static Logger * _logger = NULL;
static CompilerState * _cs = NULL;


typedef struct SymbolTableValue {

  TypeSpecifier * type; 
  char * identifier;  
  Expression * initialization;

} SymbolTableValue;


static int hash(void *str) {
    char *string = *(char **)str; 
    unsigned long hash = 5381;
    int c;
    while ((c = *string++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static bool stringEquals(void *a, void *b) {
    char *str_a = *(char **)a; 
    char *str_b = *(char **)b; 
    return strcmp(str_a, str_b) == 0;
}

static int getNewScopeId() {
  static int currentScopeId = 0;
  return currentScopeId++;
}

static HashMapADT newHashMap() {
  return hash_map_new(
    sizeof(char*), 
    sizeof(SymbolTableValue), 
    &hash,
    &stringEquals);
}

static Scope * pushNewScope() {
  Scope * newScope = (Scope*)calloc(1, sizeof(Scope));
  newScope->id = getNewScopeId();
  newScope->symbols = newHashMap();
  logDebugging(_logger, "Pushing scope with id %d onto the stack.", newScope->id);

  _cs->currentScope = newScope;
  pushStack(_cs->scopeStack, &_cs->currentScope);
  return newScope;
}

static Scope * popScope() {
  Scope * poppedScope;
  if (isEmptyStack(_cs->scopeStack)) {
    logDebugging(_logger, "Scope stack is empty, cannot pop scope.");
    return NULL;
  } 
  popStack(_cs->scopeStack, &poppedScope);
  logDebugging(_logger, "Popping scope id %d from the stack.", poppedScope->id);
  return poppedScope;
}

void popAndDestroyScope() {
  Scope *oldScope = popScope();
  if (oldScope != NULL) {
    hash_map_free(oldScope->symbols);
    free(oldScope);
  }
}



/** Shutdown module's internal state. */
void _shutdownCplusSemanticAnalyzerModule() {

  logDebugging(_logger, "Freeing symbol table and scopes...");
  while (!isEmptyStack(_cs->scopeStack)) {
    Scope *scope;
    popStack(_cs->scopeStack, &scope);
    hash_map_free(scope->symbols);
    free(scope);
  }
  
  freeStack(_cs->scopeStack);

	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: CplusSemanticAnalyzer...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeCplusSemanticAnalyzerModule() {
	_logger = createLogger("CplusSemanticAnalyzer");
	return _shutdownCplusSemanticAnalyzerModule;
}

/* -------------------------------------------------------------------------- */
/* Forward declarations                                                        */
/* -------------------------------------------------------------------------- */

ComputationResult computeProgram(Program * program);
ComputationResult computeBlock(BlockDeclaration * blockDeclaration);
ComputationResult computeClassDeclaration(ClassDeclaration * classDecl);
ComputationResult computeClassBody(ClassBody * body);
ComputationResult computeMemberDeclaration(MemberDeclaration * member);
ComputationResult computeFieldDeclaration(FieldDeclaration * field);
ComputationResult computeMethodDeclaration(MethodDeclaration * method);
ComputationResult computeStatement(Statement * statement);
ComputationResult computeExpression(Expression * expression);
ComputationResult computeFactor(Factor * factor);
ComputationResult computeArgumentList(ArgumentList * args);
ComputationResult computeParameterList(Parameter * params);

/* -------------------------------------------------------------------------- */
/* Main function                                                              */
/* -------------------------------------------------------------------------- */

ComputationResult executeSemanticalAnalysis(CompilerState * compilerState) {
	logDebugging(_logger, "Executing Cplus semantic analyzer...");

  _cs = compilerState;
  _cs->scopeStack = createStack(sizeof(Scope*));
  _cs->currentScope = pushNewScope();

	Program *program = (Program *) _cs->abstractSyntaxtTree;
	return computeProgram(program);
}

/* -------------------------------------------------------------------------- */
/* computeProgram                                                               */
/* -------------------------------------------------------------------------- */

ComputationResult computeProgram(Program * program) {
	logDebugging(_logger, "Computing program...");

	if (program == NULL) {
		logError(_logger, "computeProgram: program is NULL.");
		return _invalidComputation();
	}

	return computeBlock(program->blockDeclaration);
}

/* -------------------------------------------------------------------------- */
/* computeBlock                                                                 */
/* -------------------------------------------------------------------------- */

ComputationResult computeBlock(BlockDeclaration * blockDeclaration) {
	logDebugging(_logger, "Computing block...");

	if (blockDeclaration == NULL) {
		logError(_logger, "computeBlock: blockDeclaration is NULL.");
		return _invalidComputation();
	}

	BlockDeclaration *current = blockDeclaration;
	while (current != NULL) {

    pushNewScope();
    
		switch (current->type) {
			case CLASS_BLOCK:
				logDebugging(_logger, "Computing CLASS_BLOCK.");
				if (!computeClassDeclaration(current->classDeclaration).succeeded)
					return _invalidComputation();
				break;

			case METHOD_BLOCK:
				logDebugging(_logger, "Computing METHOD_BLOCK.");
				if (!computeMethodDeclaration(current->methodDeclaration).succeeded)
					return _invalidComputation();
				break;

			default:
				logError(_logger, "computeBlock: unknown BlockType %d.", current->type);
				return _invalidComputation();
		}
		current = current->next;

    popAndDestroyScope();
	}
	return _ok();
}

/* -------------------------------------------------------------------------- */
/* computeClassDeclaration                                                      */
/* -------------------------------------------------------------------------- */

ComputationResult computeClassDeclaration(ClassDeclaration * classDecl) {
	logDebugging(_logger, "Computing class declaration...");

	if (classDecl == NULL) {
		logError(_logger, "computeClassDeclaration: classDecl is NULL.");
		return _invalidComputation();
	}

	if (classDecl->identifier == NULL) {
		logError(_logger, "computeClassDeclaration: class has NULL identifier.");
		return _invalidComputation();
	}

	logDebugging(_logger, "Class identifier: %s", classDecl->identifier);

	// Compute class body
	return computeClassBody(classDecl->classBody);
}

/* -------------------------------------------------------------------------- */
/* computeClassBody                                                             */
/* -------------------------------------------------------------------------- */

ComputationResult computeClassBody(ClassBody * body) {
	logDebugging(_logger, "Computing class body...");

	if (body == NULL) {
		// It's valid (empty class body) depending on grammar; treat as ok.
		return _ok();
	}

	MemberDeclaration *member = body->memberList;
	while (member != NULL) {
		if (!computeMemberDeclaration(member).succeeded)
			return _invalidComputation();
		member = member->next;
	}

	return _ok();
}

/* -------------------------------------------------------------------------- */
/* computeMemberDeclaration                                                     */
/* -------------------------------------------------------------------------- */

ComputationResult computeMemberDeclaration(MemberDeclaration * member) {
	logDebugging(_logger, "Computing member declaration...");

	if (member == NULL) {
		logError(_logger, "computeMemberDeclaration: member is NULL.");
		return _invalidComputation();
	}

	switch (member->type) {
		case FIELD_MEMBER:
			logDebugging(_logger, "Member: FIELD_MEMBER");
			return computeFieldDeclaration(member->fieldDeclaration);

		case METHOD_MEMBER:
			logDebugging(_logger, "Member: METHOD_MEMBER");
			return computeMethodDeclaration(member->methodDeclaration);

		case CONSTRUCTOR_MEMBER:
			logDebugging(_logger, "Member: CONSTRUCTOR_MEMBER");
			return computeMethodDeclaration(member->methodDeclaration);

		case DESTRUCTOR_MEMBER:
			logDebugging(_logger, "Member: DESTRUCTOR_MEMBER");
			return _ok();

		default:
			logError(_logger, "computeMemberDeclaration: unknown MemberType %d.", member->type);
			return _invalidComputation();
	}
}

/* -------------------------------------------------------------------------- */
/* computeFieldDeclaration                                                      */
/* -------------------------------------------------------------------------- */

ComputationResult computeFieldDeclaration(FieldDeclaration * field) {
	logDebugging(_logger, "Computing field declaration...");

	if (field == NULL) {
		logError(_logger, "computeFieldDeclaration: field is NULL.");
		return _invalidComputation();
	}

	if (field->typeSpecifier != NULL) {
		logDebugging(_logger, "Field type: %d", field->typeSpecifier->type);
		if (field->typeSpecifier->type == IDENTIFIER_TYPE && field->typeSpecifier->identifier != NULL) {
			logDebugging(_logger, "Field named type identifier: %s", field->typeSpecifier->identifier);
		}
	}

	if (field->identifier != NULL) {
		logDebugging(_logger, "Field identifier: %s", field->identifier);
	}

	if (field->initializationExpression != NULL) {
		if (!computeExpression(field->initializationExpression).succeeded)
			return _invalidComputation();
	}

	if (field->isStatic) {
		logDebugging(_logger, "Field is static.");
	}
  
    // in this case, where are looking at a field declaration, for example public int a; 
  char * key = field->identifier; 
  if (hash_map_get(_cs->currentScope->symbols, &key) != NULL) {
    logError(_logger, "Duplicate symbol: %s", field->identifier);
    return _invalidComputation();
  }
  logDebugging(_logger, "Inserting new symbol on scope %d", _cs->currentScope->id);
  
  SymbolTableValue value = {
    .type = field->typeSpecifier,
    .identifier = field->identifier,
    .initialization = field->initializationExpression
  };
  
  hash_map_put(_cs->currentScope->symbols, &key, &value);

	return _ok();
}

/* -------------------------------------------------------------------------- */
/* computeMethodDeclaration                                                     */
/* -------------------------------------------------------------------------- */

ComputationResult computeMethodDeclaration(MethodDeclaration * method) {
	logDebugging(_logger, "Computing method declaration...");

  pushNewScope();

	if (method == NULL) {
		logError(_logger, "computeMethodDeclaration: method is NULL.");
		return _invalidComputation();
	}

	if (method->identifier != NULL)
		logDebugging(_logger, "Method identifier: %s", method->identifier);

	// Return type info
	if (method->returnType != NULL) {
		logDebugging(_logger, "Method return type: %d", method->returnType->type);
		if (method->returnType->type == IDENTIFIER_TYPE && method->returnType->identifier != NULL) {
			logDebugging(_logger, "Method return type identifier: %s", method->returnType->identifier);
		}
	}

	if (method->parameterList != NULL) {
		if (!computeParameterList(method->parameterList).succeeded)
			return _invalidComputation();
	}


	Statement *stmt = method->statementList;
	while (stmt != NULL) {
		if (!computeStatement(stmt).succeeded)
			return _invalidComputation();
		stmt = stmt->next;
	}

	if (method->isStatic) logDebugging(_logger, "Method is static.");

  popAndDestroyScope();
	return _ok();
}

/* -------------------------------------------------------------------------- */
/* computeParameterList                                                         */
/* -------------------------------------------------------------------------- */

ComputationResult computeParameterList(Parameter * params) {

	logDebugging(_logger, "Computing parameter list...");

	Parameter *p = params;
	while (p != NULL) {
		if (p->typeSpecifier != NULL) {
			logDebugging(_logger, "Parameter type: %d", p->typeSpecifier->type);
			if (p->typeSpecifier->type == IDENTIFIER_TYPE && p->typeSpecifier->identifier != NULL)
				logDebugging(_logger, "Parameter type identifier: %s", p->typeSpecifier->identifier);
		}
		if (p->identifier != NULL)
			logDebugging(_logger, "Parameter identifier: %s", p->identifier);

    
  char * key = p->identifier; 
  if (hash_map_get(_cs->currentScope->symbols, &key) != NULL) {
    logError(_logger, "Redefinition of parameter: %s", p->identifier);
    return _invalidComputation();
  }
  logDebugging(_logger, "Inserting new symbol on scope %d", _cs->currentScope->id);
  
  SymbolTableValue value = {
    .type = p->typeSpecifier,
    .identifier = p->identifier,
    .initialization = NULL,
  };
  
  hash_map_put(_cs->currentScope->symbols, &key, &value); 
		p = p->next;
	}

	return _ok();
}

/* -------------------------------------------------------------------------- */
/* computeStatement                                                             */
/* -------------------------------------------------------------------------- */

ComputationResult computeStatement(Statement * statement) {
	if (statement == NULL) {
		logError(_logger, "computeStatement: statement is NULL.");
		return _invalidComputation();
	}

	switch (statement->type) {
		case EXPRESSION_STATEMENT:
			logDebugging(_logger, "Statement: EXPRESSION_STATEMENT");
			if (statement->expression != NULL)
				return computeExpression(statement->expression);
			return _ok();

		case DECLARATION_STATEMENT:
    case INITIALIZED_DECLARATION_STATEMENT:
			logDebugging(_logger, "Statement: DECLARATION_STATEMENT");
			// typeSpecifier, identifier, expression (may be null)
			if (statement->typeSpecifier != NULL)
				logDebugging(_logger, "Declaration type: %d", statement->typeSpecifier->type);
			if (statement->identifier != NULL)
				logDebugging(_logger, "Declaration identifier: %s", statement->identifier);
			// no initialization expression in plain declaration
      
      char * key = statement->identifier; 
      if (hash_map_get(_cs->currentScope->symbols, &key) != NULL) {
        logError(_logger, "Duplicate symbol: %s", statement->identifier);
        return _invalidComputation();
      }
      logDebugging(_logger, "Inserting new symbol on scope %d", _cs->currentScope->id);
      
      SymbolTableValue value = {
        .type = statement->typeSpecifier,
        .identifier = statement->identifier,
        .initialization = statement->expression
      };
      
      hash_map_put(_cs->currentScope->symbols, &key, &value);

			return _ok();

		case RETURN_STATEMENT:
			logDebugging(_logger, "Statement: RETURN_STATEMENT");
			if (statement->expression != NULL)
				return computeExpression(statement->expression);
			return _ok();

		case RETURN_VOID_STATEMENT:
			logDebugging(_logger, "Statement: RETURN_VOID_STATEMENT");
			return _ok();

		case COMPOUND_STATEMENT:
			logDebugging(_logger, "Statement: COMPOUND_STATEMENT");
			{
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						return _invalidComputation();
					s = s->next;
				}
				return _ok();
			}

		case IF_STATEMENT:

			logDebugging(_logger, "Statement: IF_STATEMENT");
      pushNewScope();

			if (statement->condition != NULL) {
				if (!computeExpression(statement->condition).succeeded)
					return _invalidComputation();
			}
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						return _invalidComputation();
					s = s->next;
				}
			}
      popAndDestroyScope();
			return _ok();

		case IF_ELSE_STATEMENT:
			logDebugging(_logger, "Statement: IF_ELSE_STATEMENT");
			if (statement->condition != NULL) {
				if (!computeExpression(statement->condition).succeeded)
					return _invalidComputation();
			}
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						return _invalidComputation();
					s = s->next;
				}
			}
			if(statement->elseStatementList != NULL) {
				Statement *s = statement->elseStatementList;
				while(s != NULL) {
					if (!computeStatement(s).succeeded)
						return _invalidComputation();
					s = s->next;
				}
			}
			return _ok();

		case FOR_STATEMENT:

			logDebugging(_logger, "Statement: FOR_STATEMENT");
      pushNewScope();

			// initialization (Statement *), loopCondition (Expression*), postIteration (Expression*)
			if (statement->initialization != NULL) {
				if (!computeStatement(statement->initialization).succeeded)
					return _invalidComputation();
			}
			if (statement->loopCondition != NULL) {
				if (!computeExpression(statement->loopCondition).succeeded)
					return _invalidComputation();
			}
			if (statement->postIteration != NULL) {
				if (!computeExpression(statement->postIteration).succeeded)
					return _invalidComputation();
			}
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						return _invalidComputation();
					s = s->next;
				}
			}

      popAndDestroyScope();
			return _ok();

		case WHILE_STATEMENT:
			logDebugging(_logger, "Statement: WHILE_STATEMENT");
			if (statement->loopCondition != NULL)
				return computeExpression(statement->loopCondition);
			return _ok();

		case DO_WHILE_STATEMENT:
			logDebugging(_logger, "Statement: DO_WHILE_STATEMENT");
			// body first
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						return _invalidComputation();
					s = s->next;
				}
			}
			// then condition
			if (statement->loopCondition != NULL)
				return computeExpression(statement->loopCondition);
			return _ok();

		case EMPTY_STATEMENT:
			return _ok();

		default:
			logError(_logger, "computeStatement: unknown StatementType %d.", statement->type);
			return _invalidComputation();
	}
}

/* -------------------------------------------------------------------------- */
/* computeExpression                                                            */

    //NOTE: We still need to add type checking and a symbol table
/* -------------------------------------------------------------------------- */

ComputationResult computeExpression(Expression *expression) {
  if (expression == NULL)
      return _invalidComputation();

  logDebugging(_logger, "Computing expression of type %d...", expression->type);

  switch (expression->type) {

    // Mathematical expressions
    case ADDITION:
    case SUBTRACTION:
    case MULTIPLICATION:
    case DIVISION:
      if (!expression->leftExpression || !expression->rightExpression)
          return _invalidComputation();
      if (!computeExpression(expression->leftExpression).succeeded)
          return _invalidComputation();
      if (!computeExpression(expression->rightExpression).succeeded)
          return _invalidComputation();
      return _ok();

    case FACTOR:
      return computeFactor(expression->factor);

    // Comparison and logical expressions
    case GREATER_THAN_EXPRESSION:
    case LOWER_THAN_EXPRESSION:
    case GREATER_OR_EQUAL_THAN_EXPRESSION:
    case LOWER_OR_EQUAL_THAN_EXPRESSION:
    case EQUAL_EXPRESSION:
    case NOT_EQUAL_EXPRESSION:
    case LOGICAL_AND_EXPRESSION:
    case LOGICAL_OR_EXPRESSION:
      if (expression->leftExpression &&
          !computeExpression(expression->leftExpression).succeeded)
          return _invalidComputation();
      if (expression->rightExpression &&
          !computeExpression(expression->rightExpression).succeeded)
          return _invalidComputation();
      return _ok();

    case LOGICAL_NOT_EXPRESSION:
    case NEGATION:
      // see right expression
      logDebugging(_logger, "NEGATION or LOGICAL_NOT_EXPRESSION, on the right there is a %d", expression->rightExpression ? expression->rightExpression->type : -1);
      if (!expression->rightExpression)
          return _invalidComputation();
      return computeExpression(expression->rightExpression);

    case ASSIGNMENT:
      //log what's assigned
      logDebugging(_logger, "Assignment expression");
      if (!expression->leftExpression || !expression->rightExpression)
          return _invalidComputation();
      if (!computeExpression(expression->leftExpression).succeeded)
          return _invalidComputation();
      if (!computeExpression(expression->rightExpression).succeeded)
          return _invalidComputation();
      return _ok();

    case MEMBER_ACCESS:
      // left is only 'this' keyword, right has the identifier which always should be type IDENTIFIER_EXPRESSION
      logDebugging(_logger, "Expression: MEMBER_ACCESS: accessing member %s", expression->rightExpression->identifier ? expression->rightExpression->identifier : "NULL");
      if (expression->rightExpression && !computeExpression(expression->rightExpression).succeeded)
          return _invalidComputation();
      return _ok();

    case FUNCTION_CALL:
      logDebugging(_logger, "Expression: FUNCTION_CALL");
      if (expression->argumentList &&
          !computeArgumentList(expression->argumentList).succeeded)
          return _invalidComputation();
      return _ok();

    case IDENTIFIER_EXPRESSION:
    
      //check on every scope if it exists
      StackADT pusher = createStack(sizeof(Scope*));
       
      char * key = expression->identifier;
      unsigned short found = 0;
      while (!isEmptyStack(_cs->scopeStack) && !found) {
        Scope * scope = (Scope*) popScope(); // pops from main _cs->scopeStack stack
        HashMapADT symbols = scope->symbols;
        if (hash_map_get(symbols, &key) != NULL) {
          found = 1;  
        }
        pushStack(pusher, &scope);
      }
      
      //restoring
      while (!isEmptyStack(pusher)) {
        Scope * scope;
        popStack(pusher, &scope);
        logDebugging(_logger, "Restoring scope id %d into the stack", scope->id);
        pushStack(_cs->scopeStack, &scope);
      }
      
      freeStack(pusher);
      if (!found) {
        logError(_logger, "Unidentified symbol: %s", key);
        return _invalidComputation();
      }
      return _ok();

    case INTEGER_EXPRESSION:
    case THIS_EXPRESSION:
    case NEW_EXPRESSION:
    case EMPTY_EXPRESSION:
    case STRING_LITERAL_EXPRESSION:
      return _ok();

    case POST_INCREMENT_EXPRESSION:
    case PRE_INCREMENT_EXPRESSION:
    case POST_DECREMENT_EXPRESSION:
    case PRE_DECREMENT_EXPRESSION:
      if (expression->leftExpression)
          return computeExpression(expression->leftExpression);
      return _ok();

    default:
      logError(_logger, "computeExpression: unknown ExpressionType %d.", expression->type);
      return _invalidComputation();
  }
}

/* -------------------------------------------------------------------------- */
/* computeFactor                                                                */
/* -------------------------------------------------------------------------- */


ComputationResult computeFactor(Factor * factor) {
	if (factor == NULL) {
		logError(_logger, "computeFactor: factor is NULL.");
		return _invalidComputation();
	}

	switch (factor->type) {
		case CONSTANT:
			if (factor->constant == NULL) {
				logError(_logger, "computeFactor: CONSTANT with NULL constant.");
				return _invalidComputation();
			}
			return _ok();

		case EXPRESSION:
			if (factor->expression == NULL) {
				logError(_logger, "computeFactor: EXPRESSION with NULL expression.");
				return _invalidComputation();
			}
			return computeExpression(factor->expression);

		default:
			logError(_logger, "computeFactor: unknown FactorType %d.", factor->type);
			return _invalidComputation();
	}
}

/* -------------------------------------------------------------------------- */
/* computeArgumentList                                                          */
/* -------------------------------------------------------------------------- */

ComputationResult computeArgumentList(ArgumentList * args) {
	ArgumentList *a = args;
	while (a != NULL) {
		if (a->expression != NULL) {
			if (!computeExpression(a->expression).succeeded)
				return _invalidComputation();
		}
		a = a->next;
	}
	return _ok();
}

