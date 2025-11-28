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

static Logger * _logger = NULL;
static CompilerState * _cs = NULL;

static ComputationResult _invalidComputation() {
  ComputationResult r = { .succeeded = false };
  return r;
}

static ComputationResult _ok() {
  ComputationResult r = { .succeeded = true };
  return r;
}

typedef enum {
	FIELD,
	FUNCTION,
	CLASS
} EntryType;

typedef struct SymbolTableValue {
	char * identifier;
	EntryType entryType;
	union {
		struct {
			TypeSpecifier * type; 
			union {
				// Variable
				Expression * initialization;
				// Function
				Parameter * parameters;
			};
		};
		struct {
			// Class
			MemberDeclaration * methods;
		};
	};
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

void _shutdownCplusSemanticAnalyzerModule() {
  logDebugging(_logger, "Freeing symbol table and scopes...");
  
  if(_cs != NULL && _cs->scopeStack != NULL && _cs->currentScope != NULL){
    while (!isEmptyStack(_cs->scopeStack)) {
      Scope *scope;
      popStack(_cs->scopeStack, &scope);
      hash_map_free(scope->symbols);
      free(scope);
    }
    freeStack(_cs->scopeStack);
  }

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

ComputationResult computeProgram(Program * program);
ComputationResult computeBlock(BlockDeclaration * blockDeclaration);
ComputationResult computeClassDeclaration(ClassDeclaration * classDecl);
ComputationResult computeClassBody(ClassBody * body, char * identifier);
ComputationResult computeMemberDeclaration(MemberDeclaration * member, char * classIdentifier);
ComputationResult computeFieldDeclaration(FieldDeclaration * field);
ComputationResult computeMethodDeclaration(MethodDeclaration * method, char* classIdentifier);
ComputationResult computeStatement(Statement * statement);
ComputationResult computeExpression(Expression * expression);
ComputationResult computeFactor(Factor * factor);
ComputationResult computeArgumentList(ArgumentList * args);
ComputationResult computeParameterList(Parameter * params);

ComputationResult executeSemanticalAnalysis(CompilerState * compilerState) {
	logDebugging(_logger, "Executing Cplus semantic analyzer...");

	_cs = compilerState;
	_cs->scopeStack = createStack(sizeof(Scope*));
	_cs->currentScope = pushNewScope();
	_cs->globalTable = newHashMap();

	Program *program = (Program *) _cs->abstractSyntaxtTree;
	return computeProgram(program);
}

ComputationResult computeProgram(Program * program) {
	logDebugging(_logger, "Computing program...");

	if (program == NULL) {
		logError(_logger, "computeProgram: program is NULL.");
		return _invalidComputation();
	}

	return computeBlock(program->blockDeclaration);
}

ComputationResult computeBlock(BlockDeclaration * blockDeclaration) {
	logDebugging(_logger, "Computing block...");

	if (blockDeclaration == NULL) {
		logError(_logger, "computeBlock: blockDeclaration is NULL.");
		return _invalidComputation();
	}

	bool allSucceeded = true;
	BlockDeclaration *current = blockDeclaration;
	while (current != NULL) {
    pushNewScope();
    
		switch (current->type) {
			case CLASS_BLOCK:
				logDebugging(_logger, "Computing CLASS_BLOCK.");
				if (!computeClassDeclaration(current->classDeclaration).succeeded)
					allSucceeded = false;
				break;

			case METHOD_BLOCK:
				logDebugging(_logger, "Computing METHOD_BLOCK.");
				if(!computeFunctionBlock(current->methodDeclaration).succeeded) {
					allSucceeded = false;
				}
				break;

			default:
				logError(_logger, "computeBlock: unknown BlockType %d.", current->type);
				allSucceeded = false;
		}
		current = current->next;
    popAndDestroyScope();
	}
	return allSucceeded ? _ok() : _invalidComputation();
}

ComputationResult computeFunctionBlock(MethodDeclaration * function) {
	logDebugging(_logger, "Computing function declaration...");

 	pushNewScope();
	bool allSucceeded = true;

	if (function == NULL) {
		logError(_logger, "computeFunctionBlock: function is NULL.");
		return _invalidComputation();
	}

	if (function->identifier != NULL)
		logDebugging(_logger, "Function identifier: %s", function->identifier);
	else
		logError(_logger, "computeFunctionBlock: function has no identifier");

	if (function->returnType != NULL) {
		logDebugging(_logger, "function return type: %d", function->returnType->type);
		if (function->returnType->type == IDENTIFIER_TYPE && function->returnType->identifier != NULL) {
			if(hash_map_get(_cs->globalTable, &function->returnType->identifier) == NULL) {
				logError(_logger, "computeFunctionBlock: unknow return type");
				allSucceeded = false;
			} 
			logDebugging(_logger, "Function return type identifier: %s", function->returnType->identifier);
		}
	}

	if (function->parameterList != NULL) {
		if (!computeParameterList(function->parameterList).succeeded)
			allSucceeded = false;
	}

	Statement *statement = function->statementList;
	while (statement != NULL) {
		if (!computeStatement(statement).succeeded)
			allSucceeded = false;
		statement = statement->next;
	}

	if (function->isStatic) logDebugging(_logger, "Function is static.");

	char * key = function->identifier; 
	if (hash_map_get(_cs->globalTable, &key) != NULL) {
		logError(_logger, "Duplicate function: %s", function->identifier);
		allSucceeded = false;
	}
	logDebugging(_logger, "Inserting new symbol in global scope");

	SymbolTableValue value = {
		.type = function->returnType,
		.entryType = FUNCTION,
		.identifier = function->identifier,
		.parameters = function->parameterList
	};

	hash_map_put(_cs->globalTable, &key, &value);

  	popAndDestroyScope();
	return allSucceeded ? _ok() : _invalidComputation();
}

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

	// Add class to global symbol table
	char * key = classDecl->identifier; 
	if (hash_map_get(_cs->globalTable, &key) != NULL) {
		logError(_logger, "Duplicate class: %s", classDecl->identifier);
		return _invalidComputation();
	}
	logDebugging(_logger, "Inserting new class on global scope");

	SymbolTableValue value = {
		.entryType = CLASS,
		.identifier = classDecl->identifier
	};

	hash_map_put(_cs->globalTable, &key, &value);

	// Compute class body
	return computeClassBody(classDecl->classBody, classDecl->identifier);
}

ComputationResult computeClassBody(ClassBody * body, char * classIdentifier) {
	logDebugging(_logger, "Computing class body...");

	if (body == NULL) {
		return _ok();
	}

	bool allSucceeded = true;
	MemberDeclaration *member = body->memberList;
	while (member != NULL) {
		if (!computeMemberDeclaration(member, classIdentifier).succeeded)
			allSucceeded = false;
		member = member->next;
	}

	return allSucceeded ? _ok() : _invalidComputation();
}

ComputationResult computeMemberDeclaration(MemberDeclaration * member, char * classIdentifier) {
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
			return computeMethodDeclaration(member->methodDeclaration, classIdentifier);

		case CONSTRUCTOR_MEMBER:
			logDebugging(_logger, "Member: CONSTRUCTOR_MEMBER");
			return computeMethodDeclaration(member->methodDeclaration, classIdentifier);

		case DESTRUCTOR_MEMBER:
			logDebugging(_logger, "Member: DESTRUCTOR_MEMBER");
			return _ok();

		default:
			logError(_logger, "computeMemberDeclaration: unknown MemberType %d.", member->type);
			return _invalidComputation();
	}
}

ComputationResult computeFieldDeclaration(FieldDeclaration * field) {
	logDebugging(_logger, "Computing field declaration...");
	bool allSucceeded = true;

	if (field == NULL) {
		logError(_logger, "computeFieldDeclaration: field is NULL.");
		return _invalidComputation();
	}

	if (field->typeSpecifier != NULL) {
		logDebugging(_logger, "Field type: %d", field->typeSpecifier->type);
		if (field->typeSpecifier->type == IDENTIFIER_TYPE && field->typeSpecifier->identifier != NULL) {
			if(hash_map_get(_cs->globalTable, field->typeSpecifier->identifier) == NULL) {
				logError(_logger, "computeFieldDeclaration: variable type unkown");
				allSucceeded = false;
			}
			logDebugging(_logger, "Field named type identifier: %s", field->typeSpecifier->identifier);
		}
	}

	if (field->identifier != NULL) {
		logDebugging(_logger, "Field identifier: %s", field->identifier);
	}

	if (field->initializationExpression != NULL) {
		if (!computeExpression(field->initializationExpression).succeeded)
			allSucceeded = false;
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
		.entryType = FIELD,
		.identifier = field->identifier,
		.initialization = field->initializationExpression
	};

	hash_map_put(_cs->currentScope->symbols, &key, &value);

	return allSucceeded ? _ok() : _invalidComputation();
}

ComputationResult computeMethodDeclaration(MethodDeclaration * method, char * classIdentifier) {
	logDebugging(_logger, "Computing method declaration...");

	if(hash_map_get(_cs->globalTable, &classIdentifier) == NULL) {
		logError(_logger, "computeMethodDeclaration: class %s not defined", classIdentifier);
		return _invalidComputation();
	}

 	pushNewScope();
	bool allSucceeded = true;

	if (method == NULL) {
		logError(_logger, "computeMethodDeclaration: method is NULL.");
		return _invalidComputation();
	}

	if (method->identifier != NULL)
		logDebugging(_logger, "Method identifier: %s", method->identifier);

	if (method->returnType != NULL) {
		logDebugging(_logger, "Method return type: %d", method->returnType->type);
		if (method->returnType->type == IDENTIFIER_TYPE && method->returnType->identifier != NULL) {
			if(hash_map_get(_cs->globalTable, &method->returnType->identifier) == NULL) {
				logError(_logger, "computeMethodDeclaration: unknow return type");
				allSucceeded = false;
			} 
			logDebugging(_logger, "Method return type identifier: %s", method->returnType->identifier);
		}
	}

	if (method->parameterList != NULL) {
		if (!computeParameterList(method->parameterList).succeeded)
			allSucceeded = false;
	}

	Statement *stmt = method->statementList;
	while (stmt != NULL) {
		if (!computeStatement(stmt).succeeded)
			allSucceeded = false;
		stmt = stmt->next;
	}

	if (method->isStatic) logDebugging(_logger, "Method is static.");

	char * key = method->identifier;

	SymbolTableValue* classEntry;

	if ((classEntry = (SymbolTableValue*)hash_map_get(_cs->globalTable, &classIdentifier)) == NULL) {
		logError(_logger, "computeMethodDeclaration: no class %s found", classIdentifier);
		return _invalidComputation();
	}
	
	logDebugging(_logger, "Inserting new method on class %s scope", classIdentifier);

	// We add the method to the class entry on the symbol table
	MemberDeclaration* methods = classEntry->methods;
	while(methods != NULL) {
		if(strcmp(methods->methodDeclaration->identifier, method->identifier) == 0) {
			allSucceeded = false;
			logError(_logger, "computeMethodDeclaration: method already defined inside class %s", classIdentifier);
		}
		methods = methods->next;
	}

	if(allSucceeded == true) {
		methods = (MemberDeclaration*)malloc(sizeof(MemberDeclaration));
		methods->methodDeclaration = method;
		methods->next = NULL;
	}

  	popAndDestroyScope();
	return allSucceeded ? _ok() : _invalidComputation();
}

ComputationResult computeParameterList(Parameter * params) {
	logDebugging(_logger, "Computing parameter list...");

	bool allSucceeded = true;
	Parameter *p = params;
	while (p != NULL) {
		if (p->typeSpecifier != NULL) {
			logDebugging(_logger, "Parameter type: %d", p->typeSpecifier->type);
			if (p->typeSpecifier->type == IDENTIFIER_TYPE && p->typeSpecifier->identifier != NULL) {
				if(hash_map_get(_cs->globalTable, p->typeSpecifier->identifier) == NULL) {
					logError(_logger, "computeParameterList: parameter type not found");
					allSucceeded = false;
				}
				logDebugging(_logger, "Parameter type identifier: %s", p->typeSpecifier->identifier);
			}
		}
		if (p->identifier != NULL)
			logDebugging(_logger, "Parameter identifier: %s", p->identifier);

    char * key = p->identifier; 
    if (hash_map_get(_cs->currentScope->symbols, &key) != NULL) {
      logError(_logger, "Redefinition of parameter: %s", p->identifier);
      allSucceeded = false;
    } else {
      logDebugging(_logger, "Inserting new symbol on scope %d", _cs->currentScope->id);
      
      SymbolTableValue value = {
        .type = p->typeSpecifier,
		.entryType = FIELD,
        .identifier = p->identifier,
        .initialization = NULL,
      };
      
      hash_map_put(_cs->currentScope->symbols, &key, &value); 
    }
		p = p->next;
	}

	return allSucceeded ? _ok() : _invalidComputation();
}

ComputationResult computeStatement(Statement * statement) {
	if (statement == NULL) {
		logError(_logger, "computeStatement: statement is NULL.");
		return _invalidComputation();
	}

	bool allSucceeded = true;
	switch (statement->type) {
		case EXPRESSION_STATEMENT:
			logDebugging(_logger, "Statement: EXPRESSION_STATEMENT");
			if (statement->expression != NULL)
				if (!computeExpression(statement->expression).succeeded)
					allSucceeded = false;
			break;

		case DECLARATION_STATEMENT:
    case INITIALIZED_DECLARATION_STATEMENT:
			logDebugging(_logger, "Statement: DECLARATION_STATEMENT");
			if (statement->typeSpecifier != NULL)
				logDebugging(_logger, "Declaration type: %d", statement->typeSpecifier->type);
			if (statement->identifier != NULL)
				logDebugging(_logger, "Declaration identifier: %s", statement->identifier);
      
      {
        char * key = statement->identifier; 
        if (hash_map_get(_cs->currentScope->symbols, &key) != NULL) {
          logError(_logger, "Duplicate symbol: %s", statement->identifier);
          allSucceeded = false;
        } else {
          logDebugging(_logger, "Inserting new symbol on scope %d", _cs->currentScope->id);
          
          SymbolTableValue value = {
            .type = statement->typeSpecifier,
            .identifier = statement->identifier,
            .initialization = statement->expression
          };
          
          hash_map_put(_cs->currentScope->symbols, &key, &value);
        }
      }
			break;

		case RETURN_STATEMENT:
			logDebugging(_logger, "Statement: RETURN_STATEMENT");
			if (statement->expression != NULL)
				if (!computeExpression(statement->expression).succeeded)
					allSucceeded = false;
			break;

		case RETURN_VOID_STATEMENT:
			logDebugging(_logger, "Statement: RETURN_VOID_STATEMENT");
			break;

		case COMPOUND_STATEMENT:
			logDebugging(_logger, "Statement: COMPOUND_STATEMENT");
			{
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						allSucceeded = false;
					s = s->next;
				}
			}
			break;

		case IF_STATEMENT:
			logDebugging(_logger, "Statement: IF_STATEMENT");
      pushNewScope();

			if (statement->condition != NULL) {
				if (!computeExpression(statement->condition).succeeded)
					allSucceeded = false;
			}
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						allSucceeded = false;
					s = s->next;
				}
			}
      popAndDestroyScope();
			break;

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

			if (statement->initialization != NULL) {
				if (!computeStatement(statement->initialization).succeeded)
					allSucceeded = false;
			}
			if (statement->loopCondition != NULL) {
				if (!computeExpression(statement->loopCondition).succeeded)
					allSucceeded = false;
			}
			if (statement->postIteration != NULL) {
				if (!computeExpression(statement->postIteration).succeeded)
					allSucceeded = false;
			}
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						allSucceeded = false;
					s = s->next;
				}
			}

      popAndDestroyScope();
			break;

		case WHILE_STATEMENT:
			logDebugging(_logger, "Statement: WHILE_STATEMENT");
			if (statement->loopCondition != NULL)
				if (!computeExpression(statement->loopCondition).succeeded)
					allSucceeded = false;
			break;

		case DO_WHILE_STATEMENT:
			logDebugging(_logger, "Statement: DO_WHILE_STATEMENT");
			if (statement->statementList != NULL) {
				Statement *s = statement->statementList;
				while (s != NULL) {
					if (!computeStatement(s).succeeded)
						allSucceeded = false;
					s = s->next;
				}
			}
			if (statement->loopCondition != NULL)
				if (!computeExpression(statement->loopCondition).succeeded)
					allSucceeded = false;
			break;

		case EMPTY_STATEMENT:
			break;

		default:
			logError(_logger, "computeStatement: unknown StatementType %d.", statement->type);
			allSucceeded = false;
	}

	return allSucceeded ? _ok() : _invalidComputation();
}

ComputationResult computeExpression(Expression *expression) {
  if (expression == NULL)
      return _invalidComputation();

  logDebugging(_logger, "Computing expression of type %d...", expression->type);

  bool allSucceeded = true;
  switch (expression->type) {
    case ADDITION:
    case SUBTRACTION:
    case MULTIPLICATION:
    case DIVISION:
      if (!expression->leftExpression || !expression->rightExpression)
          return _invalidComputation();
      if (!computeExpression(expression->leftExpression).succeeded)
          allSucceeded = false;
      if (!computeExpression(expression->rightExpression).succeeded)
          allSucceeded = false;
      break;

    case FACTOR:
      if (!computeFactor(expression->factor).succeeded)
          allSucceeded = false;
      break;

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
          allSucceeded = false;
      if (expression->rightExpression &&
          !computeExpression(expression->rightExpression).succeeded)
          allSucceeded = false;
      break;

    case LOGICAL_NOT_EXPRESSION:
    case NEGATION:
      logDebugging(_logger, "NEGATION or LOGICAL_NOT_EXPRESSION, on the right there is a %d", expression->rightExpression ? expression->rightExpression->type : -1);
      if (!expression->rightExpression)
          return _invalidComputation();
      if (!computeExpression(expression->rightExpression).succeeded)
          allSucceeded = false;
      break;

    case ASSIGNMENT:
      logDebugging(_logger, "Assignment expression");
      if (!expression->leftExpression || !expression->rightExpression)
          return _invalidComputation();
      if (!computeExpression(expression->leftExpression).succeeded)
          allSucceeded = false;
      if (!computeExpression(expression->rightExpression).succeeded)
          allSucceeded = false;
      break;

    case MEMBER_ACCESS:
      logDebugging(_logger, "Expression: MEMBER_ACCESS: accessing member %s", expression->rightExpression->identifier ? expression->rightExpression->identifier : "NULL");
      if (expression->rightExpression && !computeExpression(expression->rightExpression).succeeded)
          allSucceeded = false;
      break;

    case FUNCTION_CALL:
      logDebugging(_logger, "Expression: FUNCTION_CALL");
      if (expression->argumentList &&
          !computeArgumentList(expression->argumentList).succeeded)
          allSucceeded = false;
      break;

    case IDENTIFIER_EXPRESSION:
      {
        StackADT pusher = createStack(sizeof(Scope*));
         
        char * key = expression->identifier;
        unsigned short found = 0;
        while (!isEmptyStack(_cs->scopeStack) && !found) {
          Scope * scope = (Scope*) popScope();
          HashMapADT symbols = scope->symbols;
          if (hash_map_get(symbols, &key) != NULL) {
            found = 1;  
          }
          pushStack(pusher, &scope);
        }
        
        while (!isEmptyStack(pusher)) {
          Scope * scope;
          popStack(pusher, &scope);
          logDebugging(_logger, "Restoring scope id %d into the stack", scope->id);
          pushStack(_cs->scopeStack, &scope);
        }
        
        freeStack(pusher);
        if (!found) {
          logError(_logger, "Unidentified symbol: %s", key);
          allSucceeded = false;
        }
      }
      break;

    case INTEGER_EXPRESSION:
    case THIS_EXPRESSION:
    case NEW_EXPRESSION:
    case EMPTY_EXPRESSION:
    case STRING_LITERAL_EXPRESSION:
      break;

    case POST_INCREMENT_EXPRESSION:
    case PRE_INCREMENT_EXPRESSION:
    case POST_DECREMENT_EXPRESSION:
    case PRE_DECREMENT_EXPRESSION:
      if (expression->leftExpression)
          if (!computeExpression(expression->leftExpression).succeeded)
              allSucceeded = false;
      break;

    default:
      logError(_logger, "computeExpression: unknown ExpressionType %d.", expression->type);
      allSucceeded = false;
  }

  return allSucceeded ? _ok() : _invalidComputation();
}

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

ComputationResult computeArgumentList(ArgumentList * args) {
	bool allSucceeded = true;
	ArgumentList *a = args;
	while (a != NULL) {
		if (a->expression != NULL) {
			if (!computeExpression(a->expression).succeeded)
				allSucceeded = false;
		}
		a = a->next;
	}
	return allSucceeded ? _ok() : _invalidComputation();
}
