#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ClassProgramSemanticAction(ClassDeclaration * classDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->classDeclaration = classDeclaration;
	program->type = CLASS_PROGRAM;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

ClassDeclaration * SingleClassDeclarationSemanticAction(ClassDeclaration * classDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return classDeclaration;
}

ClassDeclaration * MultipleClassDeclarationSemanticAction(ClassDeclaration * classDeclarationList, ClassDeclaration * classDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClassDeclaration * current = classDeclarationList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = classDeclaration;
	return classDeclarationList;
}

ClassDeclaration * ClassDeclarationSemanticAction(char * identifier, ClassBody * classBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClassDeclaration * classDeclaration = calloc(1, sizeof(ClassDeclaration));
	classDeclaration->identifier = identifier;
	classDeclaration->classBody = classBody;
	classDeclaration->next = NULL;
	return classDeclaration;
}

ClassBody * EmptyClassBodySemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClassBody * classBody = calloc(1, sizeof(ClassBody));
	classBody->memberList = NULL;
	return classBody;
}

ClassBody * MemberListClassBodySemanticAction(MemberDeclaration * memberList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClassBody * classBody = calloc(1, sizeof(ClassBody));
	classBody->memberList = memberList;
	return classBody;
}

MemberDeclaration * SingleMemberSemanticAction(MemberDeclaration * member) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return member;
}

MemberDeclaration * MultipleMemberSemanticAction(MemberDeclaration * memberList, MemberDeclaration * member) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MemberDeclaration * current = memberList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = member;
	return memberList;
}

MemberDeclaration * AccessSpecifiedMemberSemanticAction(AccessSpecifier * accessSpecifier, MemberDeclaration * memberDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	memberDeclaration->accessSpecifier = accessSpecifier;
	return memberDeclaration;
}

MemberDeclaration * DefaultAccessMemberSemanticAction(MemberDeclaration * memberDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	memberDeclaration->accessSpecifier = NULL;
	return memberDeclaration;
}

AccessSpecifier * PrivateAccessSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AccessSpecifier * accessSpecifier = calloc(1, sizeof(AccessSpecifier));
	accessSpecifier->type = PRIVATE_ACCESS;
	return accessSpecifier;
}

AccessSpecifier * PublicAccessSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AccessSpecifier * accessSpecifier = calloc(1, sizeof(AccessSpecifier));
	accessSpecifier->type = PUBLIC_ACCESS;
	return accessSpecifier;
}

MemberDeclaration * FieldMemberSemanticAction(FieldDeclaration * fieldDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MemberDeclaration * memberDeclaration = calloc(1, sizeof(MemberDeclaration));
	memberDeclaration->fieldDeclaration = fieldDeclaration;
	memberDeclaration->type = FIELD_MEMBER;
	memberDeclaration->next = NULL;
	return memberDeclaration;
}

MemberDeclaration * MethodMemberSemanticAction(MethodDeclaration * methodDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MemberDeclaration * memberDeclaration = calloc(1, sizeof(MemberDeclaration));
	memberDeclaration->methodDeclaration = methodDeclaration;
	memberDeclaration->type = METHOD_MEMBER;
	memberDeclaration->next = NULL;
	return memberDeclaration;
}

MemberDeclaration * ConstructorMemberSemanticAction(MethodDeclaration * constructorDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MemberDeclaration * memberDeclaration = calloc(1, sizeof(MemberDeclaration));
	memberDeclaration->methodDeclaration = constructorDeclaration;
	memberDeclaration->type = CONSTRUCTOR_MEMBER;
	memberDeclaration->next = NULL;
	return memberDeclaration;
}

MemberDeclaration * DestructorMemberSemanticAction(MethodDeclaration * destructorDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MemberDeclaration * memberDeclaration = calloc(1, sizeof(MemberDeclaration));
	memberDeclaration->methodDeclaration = destructorDeclaration;
	memberDeclaration->type = DESTRUCTOR_MEMBER;
	memberDeclaration->next = NULL;
	return memberDeclaration;
}

FieldDeclaration * FieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FieldDeclaration * fieldDeclaration = calloc(1, sizeof(FieldDeclaration));
	fieldDeclaration->typeSpecifier = typeSpecifier;
	fieldDeclaration->identifier = identifier;
	fieldDeclaration->isStatic = 0;
	return fieldDeclaration;
}

FieldDeclaration * StaticFieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FieldDeclaration * fieldDeclaration = calloc(1, sizeof(FieldDeclaration));
	fieldDeclaration->typeSpecifier = typeSpecifier;
	fieldDeclaration->identifier = identifier;
	fieldDeclaration->isStatic = 1;
	return fieldDeclaration;
}

MethodDeclaration * MethodDeclarationSemanticAction(TypeSpecifier * returnType, char * identifier, Parameter * parameterList, Statement * statementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodDeclaration * methodDeclaration = calloc(1, sizeof(MethodDeclaration));
	methodDeclaration->returnType = returnType;
	methodDeclaration->identifier = identifier;
	methodDeclaration->parameterList = parameterList;
	methodDeclaration->statementList = statementList;
	methodDeclaration->isStatic = 0;
	return methodDeclaration;
}

MethodDeclaration * StaticMethodDeclarationSemanticAction(TypeSpecifier * returnType, char * identifier, Parameter * parameterList, Statement * statementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodDeclaration * methodDeclaration = calloc(1, sizeof(MethodDeclaration));
	methodDeclaration->returnType = returnType;
	methodDeclaration->identifier = identifier;
	methodDeclaration->parameterList = parameterList;
	methodDeclaration->statementList = statementList;
	methodDeclaration->isStatic = 1;
	return methodDeclaration;
}

MethodDeclaration * ConstructorDeclarationSemanticAction(Parameter * parameterList, Statement * statementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodDeclaration * methodDeclaration = calloc(1, sizeof(MethodDeclaration));
	methodDeclaration->returnType = NULL;
	methodDeclaration->identifier = NULL;
	methodDeclaration->parameterList = parameterList;
	methodDeclaration->statementList = statementList;
	methodDeclaration->isStatic = 0;
	return methodDeclaration;
}

MethodDeclaration * DestructorDeclarationSemanticAction(Statement * statementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodDeclaration * methodDeclaration = calloc(1, sizeof(MethodDeclaration));
	methodDeclaration->returnType = NULL;
	methodDeclaration->identifier = NULL;
	methodDeclaration->parameterList = NULL;
	methodDeclaration->statementList = statementList;
	methodDeclaration->isStatic = 0;
	return methodDeclaration;
}

Parameter * EmptyParameterListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

Parameter * ParametersSemanticAction(Parameter * parameters) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return parameters;
}

Parameter * SingleParameterSemanticAction(Parameter * parameter) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return parameter;
}

Parameter * MultipleParameterSemanticAction(Parameter * parameterList, Parameter * parameter) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * current = parameterList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = parameter;
	return parameterList;
}

Parameter * ParameterSemanticAction(TypeSpecifier * typeSpecifier, char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * parameter = calloc(1, sizeof(Parameter));
	parameter->typeSpecifier = typeSpecifier;
	parameter->identifier = identifier;
	parameter->next = NULL;
	return parameter;
}

TypeSpecifier * IntTypeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TypeSpecifier * typeSpecifier = calloc(1, sizeof(TypeSpecifier));
	typeSpecifier->type = INT_TYPE;
	typeSpecifier->identifier = NULL;
	return typeSpecifier;
}

TypeSpecifier * VoidTypeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TypeSpecifier * typeSpecifier = calloc(1, sizeof(TypeSpecifier));
	typeSpecifier->type = VOID_TYPE;
	typeSpecifier->identifier = NULL;
	return typeSpecifier;
}

TypeSpecifier * CharTypeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TypeSpecifier * typeSpecifier = calloc(1, sizeof(TypeSpecifier));
	typeSpecifier->type = CHAR_TYPE;
	typeSpecifier->identifier = NULL;
	return typeSpecifier;
}

TypeSpecifier * IdentifierTypeSemanticAction(char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TypeSpecifier * typeSpecifier = calloc(1, sizeof(TypeSpecifier));
	typeSpecifier->type = IDENTIFIER_TYPE;
	typeSpecifier->identifier = identifier;
	return typeSpecifier;
}

Statement * EmptyStatementListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

Statement * StatementListSemanticAction(Statement * statementList, Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (statementList == NULL) {
		return statement;
	}
	Statement * current = statementList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = statement;
	return statementList;
}

Statement * ExpressionStatementSemanticAction(Statement * expressionStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expressionStatement;
}

Statement * DeclarationStatementSemanticAction(Statement * declarationStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return declarationStatement;
}

Statement * ReturnStatementSemanticAction(Statement * returnStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return returnStatement;
}

Statement * CompoundStatementSemanticAction(Statement * compoundStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return compoundStatement;
}

Statement * CompoundStatementBodySemanticAction(Statement * statementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->statementList = statementList;
	statement->type = COMPOUND_STATEMENT;
	statement->next = NULL;
	return statement;
}

Statement * VariableDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->typeSpecifier = typeSpecifier;
	statement->identifier = identifier;
	statement->expression = NULL;
	statement->type = DECLARATION_STATEMENT;
	statement->next = NULL;
	return statement;
}

Statement * InitializedVariableDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->typeSpecifier = typeSpecifier;
	statement->identifier = identifier;
	statement->expression = expression;
	statement->type = INITIALIZED_DECLARATION_STATEMENT;
	statement->next = NULL;
	return statement;
}

Statement * ExpressionStatementBodySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->expression = expression;
	statement->type = EXPRESSION_STATEMENT;
	statement->next = NULL;
	return statement;
}

Statement * ReturnExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->expression = expression;
	statement->type = RETURN_STATEMENT;
	statement->next = NULL;
	return statement;
}

Statement * ReturnVoidSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->expression = NULL;
	statement->type = RETURN_VOID_STATEMENT;
	statement->next = NULL;
	return statement;
}

Expression * AssignmentExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * AdditiveExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * AssignmentSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = ASSIGNMENT;
	return expression;
}

Expression * MultiplicativeExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * AdditionSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = ADDITION;
	return expression;
}

Expression * SubtractionSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = SUBTRACTION;
	return expression;
}

Expression * UnaryExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * MultiplicationSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = MULTIPLICATION;
	return expression;
}

Expression * DivisionSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = DIVISION;
	return expression;
}

Expression * PostfixExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * NegationSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * negation = calloc(1, sizeof(Expression));
	negation->rightExpression = expression;
	negation->type = NEGATION;
	return negation;
}

Expression * PrimaryExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * MemberAccessSemanticAction(Expression * expression, char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * memberAccess = calloc(1, sizeof(Expression));
	memberAccess->leftExpression = expression;
	Expression * rightExpression = calloc(1, sizeof(Expression));
	rightExpression->identifier = identifier;
	rightExpression->type = IDENTIFIER_EXPRESSION;
	memberAccess->rightExpression = rightExpression;
	memberAccess->type = MEMBER_ACCESS;
	return memberAccess;
}

Expression * FunctionCallSemanticAction(Expression * expression, ArgumentList * argumentList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * functionCall = calloc(1, sizeof(Expression));
	functionCall->leftExpression = expression;
	functionCall->argumentList = argumentList;
	functionCall->type = FUNCTION_CALL;
	return functionCall;
}

Expression * IdentifierExpressionSemanticAction(char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->identifier = identifier;
	expression->type = IDENTIFIER_EXPRESSION;
	return expression;
}

Expression * IntegerExpressionSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->integerValue = value;
	expression->type = INTEGER_EXPRESSION;
	return expression;
}

Expression * ThisExpressionSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->type = THIS_EXPRESSION;
	return expression;
}

Expression * ParenthesizedExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * NewExpressionSemanticAction(char * identifier, ArgumentList * argumentList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->identifier = identifier;
	expression->argumentList = argumentList;
	expression->type = NEW_EXPRESSION;
	return expression;
}

ArgumentList * EmptyArgumentListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

ArgumentList * ArgumentsSemanticAction(ArgumentList * arguments) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return arguments;
}

ArgumentList * SingleArgumentSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ArgumentList * argumentList = calloc(1, sizeof(ArgumentList));
	argumentList->expression = expression;
	argumentList->next = NULL;
	return argumentList;
}

ArgumentList * MultipleArgumentSemanticAction(ArgumentList * argumentList, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ArgumentList * current = argumentList;
	while (current->next != NULL) {
		current = current->next;
	}
	ArgumentList * newArgument = calloc(1, sizeof(ArgumentList));
	newArgument->expression = expression;
	newArgument->next = NULL;
	current->next = newArgument;
	return argumentList;
}