#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	if (expression != NULL) {
	logDebugging(_logger, "Executing destructor: %s of type %d", __FUNCTION__, expression->type);
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
			case ASSIGNMENT:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case GREATER_THAN_EXPRESSION:
			case LOWER_THAN_EXPRESSION:
			case GREATER_OR_EQUAL_THAN_EXPRESSION:
			case LOWER_OR_EQUAL_THAN_EXPRESSION:
			case EQUAL_EXPRESSION:
			case NOT_EQUAL_EXPRESSION:
			case LOGICAL_AND_EXPRESSION:
			case LOGICAL_OR_EXPRESSION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
			case NEGATION:
				destroyExpression(expression->rightExpression);
				break;
			case MEMBER_ACCESS:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FUNCTION_CALL:
				destroyExpression(expression->leftExpression);
				destroyArgumentList(expression->argumentList);
				break;
			case IDENTIFIER_EXPRESSION:
				if (expression->identifier != NULL) {
					free(expression->identifier);
				}
				break;
			case INTEGER_EXPRESSION:
			case THIS_EXPRESSION:
				// No dynamic memory to free
				break;
			case NEW_EXPRESSION:
				if (expression->identifier != NULL) {
					free(expression->identifier);
				}
				destroyArgumentList(expression->argumentList);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		switch (program->type) {
			case BLOCK_PROGRAM:
				destroyBlockDeclaration(program->blockDeclaration);
				break;
		}
		free(program);
	}
}

void destroyBlockDeclaration(BlockDeclaration * BlockDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (BlockDeclaration != NULL) {
		switch (BlockDeclaration->type) {
			case CLASS_BLOCK:
				destroyClassDeclaration(BlockDeclaration->classDeclaration);
				break;
			case METHOD_BLOCK:
				destroyMethodDeclaration(BlockDeclaration->methodDeclaration);
				break;
		}
		destroyBlockDeclaration(BlockDeclaration->next);
		free(BlockDeclaration);
	}
}

void destroyClassDeclaration(ClassDeclaration * classDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (classDeclaration != NULL) {
		if (classDeclaration->identifier != NULL) {
			free(classDeclaration->identifier);
		}
		destroyClassBody(classDeclaration->classBody);
		free(classDeclaration);
	}
}

void destroyClassBody(ClassBody * classBody) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (classBody != NULL) {
		destroyMemberDeclaration(classBody->memberList);
		free(classBody);
	}
}

void destroyMemberDeclaration(MemberDeclaration * memberDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (memberDeclaration != NULL) {
		destroyAccessSpecifier(memberDeclaration->accessSpecifier);
		switch (memberDeclaration->type) {
			case FIELD_MEMBER:
				destroyFieldDeclaration(memberDeclaration->fieldDeclaration);
				break;
			case METHOD_MEMBER:
			case CONSTRUCTOR_MEMBER:
			case DESTRUCTOR_MEMBER:
				destroyMethodDeclaration(memberDeclaration->methodDeclaration);
				break;
		}
		destroyMemberDeclaration(memberDeclaration->next);
		free(memberDeclaration);
	}
}

void destroyFieldDeclaration(FieldDeclaration * fieldDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (fieldDeclaration != NULL) {
		destroyTypeSpecifier(fieldDeclaration->typeSpecifier);
		if (fieldDeclaration->identifier != NULL) {
			free(fieldDeclaration->identifier);
		}
		if (fieldDeclaration->initializationExpression != NULL) {
			destroyExpression(fieldDeclaration->initializationExpression);
		}
		free(fieldDeclaration);
	}
}

void destroyMethodDeclaration(MethodDeclaration * methodDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (methodDeclaration != NULL) {
		destroyTypeSpecifier(methodDeclaration->returnType);
		if (methodDeclaration->identifier != NULL) {
			free(methodDeclaration->identifier);
		}
		destroyParameter(methodDeclaration->parameterList);
		destroyStatement(methodDeclaration->statementList);
		free(methodDeclaration);
	}
}

void destroyStatement(Statement * statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (statement != NULL) {
		switch (statement->type) {
			case EXPRESSION_STATEMENT:
			case RETURN_STATEMENT:
				destroyExpression(statement->expression);
				break;
			case DECLARATION_STATEMENT:
				destroyTypeSpecifier(statement->typeSpecifier);
				if (statement->identifier != NULL) {
					free(statement->identifier);
				}
				break;
			case INITIALIZED_DECLARATION_STATEMENT:
				destroyTypeSpecifier(statement->typeSpecifier);
				if (statement->identifier != NULL) {
					free(statement->identifier);
				}
				destroyExpression(statement->expression);
				break;
			case RETURN_VOID_STATEMENT:
				// No dynamic memory to free
				break;
			case COMPOUND_STATEMENT:
				destroyStatement(statement->statementList);
				break;
			case IF_STATEMENT:
				destroyExpression(statement->condition);
				destroyStatement(statement->statementList);
				break;
			case FOR_STATEMENT:
				destroyStatement(statement->initialization);
				destroyExpression(statement->loopCondition);
				destroyExpression(statement->postIteration);
				destroyStatement(statement->statementList);
				break;
			case WHILE_STATEMENT:
				destroyExpression(statement->loopCondition);
				destroyStatement(statement->statementList);
				break;
			case DO_WHILE_STATEMENT:
				destroyExpression(statement->loopCondition);
				destroyStatement(statement->statementList);
				break;
		}
		destroyStatement(statement->next);
		free(statement);
	}
}

void destroyTypeSpecifier(TypeSpecifier * typeSpecifier) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (typeSpecifier != NULL) {
		if (typeSpecifier->identifier != NULL) {
			free(typeSpecifier->identifier);
		}
		free(typeSpecifier);
	}
}

void destroyAccessSpecifier(AccessSpecifier * accessSpecifier) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (accessSpecifier != NULL) {
		free(accessSpecifier);
	}
}

void destroyParameter(Parameter * parameter) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (parameter != NULL) {
		destroyTypeSpecifier(parameter->typeSpecifier);
		if (parameter->identifier != NULL) {
			free(parameter->identifier);
		}
		destroyParameter(parameter->next);
		free(parameter);
	}
}

void destroyArgumentList(ArgumentList * argumentList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (argumentList != NULL) {
		destroyExpression(argumentList->expression);
		destroyArgumentList(argumentList->next);
		free(argumentList);
	}
}