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

Program * BlockProgramSemanticAction(BlockDeclaration * blockDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->blockDeclaration = blockDeclaration;
	program->type = BLOCK_PROGRAM;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

BlockDeclaration * SingleBlockDeclarationSemanticAction(BlockDeclaration * blockDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return blockDeclaration;
}

BlockDeclaration * MultipleBlockDeclarationSemanticAction(BlockDeclaration * blockDeclarationList, BlockDeclaration * blockDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	BlockDeclaration * current = blockDeclarationList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = blockDeclaration;
	return blockDeclarationList;
}

BlockDeclaration * ClassBlockDeclarationSemanticAction(ClassDeclaration * classDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	BlockDeclaration * blockDeclaration = calloc(1, sizeof(BlockDeclaration));
	blockDeclaration->classDeclaration = classDeclaration;
	blockDeclaration->type = CLASS_BLOCK;
	blockDeclaration->next = NULL;
	return blockDeclaration;
}

BlockDeclaration * MethodBlockDeclarationSemanticAction(MethodDeclaration * MethodDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	BlockDeclaration * blockDeclaration = calloc(1, sizeof(BlockDeclaration));
	blockDeclaration->methodDeclaration = MethodDeclaration;
	blockDeclaration->type = METHOD_BLOCK;
	blockDeclaration->next = NULL;
	return blockDeclaration;
}

ClassDeclaration * ClassDeclarationSemanticAction(char * identifier, ClassBody * classBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClassDeclaration * classDeclaration = calloc(1, sizeof(ClassDeclaration));
	classDeclaration->identifier = identifier;
	classDeclaration->classBody = classBody;
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

FieldDeclaration * InitializedFieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FieldDeclaration * fieldDeclaration = calloc(1, sizeof(FieldDeclaration));
	fieldDeclaration->typeSpecifier = typeSpecifier;
	fieldDeclaration->identifier = identifier;
	fieldDeclaration->initializationExpression = expression;
	fieldDeclaration->isStatic = 0;
	return fieldDeclaration;
}

FieldDeclaration * InitializedStaticFieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FieldDeclaration * fieldDeclaration = calloc(1, sizeof(FieldDeclaration));
	fieldDeclaration->typeSpecifier = typeSpecifier;
	fieldDeclaration->identifier = identifier;
	fieldDeclaration->initializationExpression = expression;
	fieldDeclaration->isStatic = 1;
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

TypeSpecifier * StringTypeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TypeSpecifier * typeSpecifier = calloc(1, sizeof(TypeSpecifier));
	typeSpecifier->type = STRING_TYPE;
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

Statement * StructuralStatementSemanticAction(Statement * structuralStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return structuralStatement;
}

Statement * DeclarationStatementSemanticAction(Statement * declarationStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return declarationStatement;
}

Statement * ExpressionStatementSemanticAction(Statement * expressionStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expressionStatement;
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

Statement * IfStatementSemanticAction(Statement * ifStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return ifStatement;
}

Statement * IfStatementBodySemanticAction(Expression * expression, Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * ifStatement = calloc(1, sizeof(Statement));
	ifStatement->condition = expression;
	ifStatement->statementList = statement;
	ifStatement->type = IF_STATEMENT;
	ifStatement->next = NULL;
	return ifStatement;
}

Statement * IfElseStatementSemanticAction(Statement * ifStatement, Statement * elseStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ifStatement->next = elseStatement;
	return ifStatement;
}

Statement * ForStatementSemanticAction(Statement * forStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return forStatement;
}

Statement * ForStatementBodySemanticAction(Statement * initialization, Expression * condition, Expression * increment, Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * forStatement = calloc(1, sizeof(Statement));
	forStatement->initialization = initialization;
	forStatement->loopCondition = condition;
	forStatement->postIteration = increment;
	forStatement->statementList = statement;
	forStatement->type = FOR_STATEMENT;
	forStatement->next = NULL;
	return forStatement;
}

Statement * WhileStatementSemanticAction(Statement * whileStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return whileStatement;
}

Statement * WhileStatementBodySemanticAction(Expression * condition, Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * whileStatement = calloc(1, sizeof(Statement));
	whileStatement->loopCondition = condition;
	whileStatement->statementList = statement;
	whileStatement->type = WHILE_STATEMENT;
	whileStatement->next = NULL;
	return whileStatement;
}

Statement * DoWhileStatementSemanticAction(Statement * doWhileStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return doWhileStatement;
}

Statement * DoWhileStatementBodySemanticAction(Statement * statement, Expression * condition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * doWhileStatement = calloc(1, sizeof(Statement));
	doWhileStatement->statementList = statement;
	doWhileStatement->loopCondition = condition;
	doWhileStatement->type = DO_WHILE_STATEMENT;
	doWhileStatement->next = NULL;
	return doWhileStatement;
}

Statement * EmptyDeclarationStatementSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->type = EMPTY_STATEMENT;
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

Expression * EmptyExpressionSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->type = EMPTY_EXPRESSION;
	return expression;
}

Expression * AssignmentExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return expression;
}

Expression * GreaterThanSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = GREATER_THAN_EXPRESSION;
	return expression;
}

Expression * LowerThanSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = LOWER_THAN_EXPRESSION;
	return expression;
}

Expression * GreaterEqualThanSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = GREATER_OR_EQUAL_THAN_EXPRESSION;
	return expression;
}

Expression * LowerEqualThanSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = LOWER_OR_EQUAL_THAN_EXPRESSION;
	return expression;
}

Expression * EqualSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = EQUAL_EXPRESSION;
	return expression;
}

Expression * NotEqualSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = NOT_EQUAL_EXPRESSION;
	return expression;
}

Expression * LogicalAndSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = LOGICAL_AND_EXPRESSION;
	return expression;
}

Expression * LogicalOrSemanticAction(Expression * leftExpression, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = LOGICAL_OR_EXPRESSION;
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

Expression * LogicalNotSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * logicalNot = calloc(1, sizeof(Expression));
	logicalNot->rightExpression = expression;
	logicalNot->type = LOGICAL_NOT_EXPRESSION;
	return logicalNot;
}

Expression * PreIncrementSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * preIncrement = calloc(1, sizeof(Expression));
	preIncrement->rightExpression = expression;
	preIncrement->type = PRE_INCREMENT_EXPRESSION;
	return preIncrement;
}

Expression * PreDecrementSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * preDecrement = calloc(1, sizeof(Expression));
	preDecrement->rightExpression = expression;
	preDecrement->type = PRE_DECREMENT_EXPRESSION;
	return preDecrement;
}

Expression * PostIncrementSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * postIncrement = calloc(1, sizeof(Expression));
	postIncrement->leftExpression = expression;
	postIncrement->type = POST_INCREMENT_EXPRESSION;
	return postIncrement;
}

Expression * PostDecrementSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * postDecrement = calloc(1, sizeof(Expression));
	postDecrement->leftExpression = expression;
	postDecrement->type = POST_DECREMENT_EXPRESSION;
	return postDecrement;
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
	functionCall->precedingExpression = expression;
	functionCall->args = argumentList;
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

Expression * StringLiteralExpressionSemanticAction(char * value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression * expression = calloc(1, sizeof(Expression));
    expression->length = strlen(value) + 1;
    expression->stringLiteralValue = value;  
    expression->type = STRING_LITERAL_EXPRESSION;
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