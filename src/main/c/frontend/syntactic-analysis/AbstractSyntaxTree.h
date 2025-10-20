#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum ProgramType ProgramType;
typedef enum MemberType MemberType;
typedef enum StatementType StatementType;
typedef enum TypeSpecifierType TypeSpecifierType;
typedef enum AccessSpecifierType AccessSpecifierType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
typedef struct ClassDeclaration ClassDeclaration;
typedef struct ClassBody ClassBody;
typedef struct MemberDeclaration MemberDeclaration;
typedef struct FieldDeclaration FieldDeclaration;
typedef struct MethodDeclaration MethodDeclaration;
typedef struct Statement Statement;
typedef struct TypeSpecifier TypeSpecifier;
typedef struct AccessSpecifier AccessSpecifier;
typedef struct Parameter Parameter;
typedef struct ArgumentList ArgumentList;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION,
	ASSIGNMENT,
	NEGATION,
	MEMBER_ACCESS,
	FUNCTION_CALL,
	IDENTIFIER_EXPRESSION,
	INTEGER_EXPRESSION,
	THIS_EXPRESSION,
	NEW_EXPRESSION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

enum ProgramType {
	CLASS_PROGRAM
};

enum MemberType {
	FIELD_MEMBER,
	METHOD_MEMBER,
	CONSTRUCTOR_MEMBER,
	DESTRUCTOR_MEMBER
};

enum StatementType {
	EXPRESSION_STATEMENT,
	DECLARATION_STATEMENT,
	INITIALIZED_DECLARATION_STATEMENT,
	RETURN_STATEMENT,
	RETURN_VOID_STATEMENT,
	COMPOUND_STATEMENT
};

enum TypeSpecifierType {
	INT_TYPE,
	VOID_TYPE,
	CHAR_TYPE,
	IDENTIFIER_TYPE
};

enum AccessSpecifierType {
	PRIVATE_ACCESS,
	PUBLIC_ACCESS
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
		struct {
			char * identifier;
			int integerValue;
			ArgumentList * argumentList;
		};
	};
	ExpressionType type;
};

struct Program {
	union {
		Expression * expression;
		ClassDeclaration * classDeclaration;
	};
	ProgramType type;
};

struct ClassDeclaration {
	char * identifier;
	ClassBody * classBody;
	ClassDeclaration * next;
};

struct ClassBody {
	MemberDeclaration * memberList;
};

struct MemberDeclaration {
	AccessSpecifier * accessSpecifier;
	union {
		FieldDeclaration * fieldDeclaration;
		MethodDeclaration * methodDeclaration;
	};
	MemberType type;
	MemberDeclaration * next;
};

struct FieldDeclaration {
	TypeSpecifier * typeSpecifier;
	char * identifier;
	int isStatic;
};

struct MethodDeclaration {
	TypeSpecifier * returnType;
	char * identifier;
	Parameter * parameterList;
	Statement * statementList;
	int isStatic;
};

struct Statement {
	union {
		struct {
			TypeSpecifier * typeSpecifier;
			char * identifier;
			Expression * expression;
		};
		Statement * statementList;
	};
	StatementType type;
	Statement * next;
};

struct TypeSpecifier {
	TypeSpecifierType type;
	char * identifier;
};

struct AccessSpecifier {
	AccessSpecifierType type;
};

struct Parameter {
	TypeSpecifier * typeSpecifier;
	char * identifier;
	Parameter * next;
};

struct ArgumentList {
	Expression * expression;
	ArgumentList * next;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);
void destroyClassDeclaration(ClassDeclaration * classDeclaration);
void destroyClassBody(ClassBody * classBody);
void destroyMemberDeclaration(MemberDeclaration * memberDeclaration);
void destroyFieldDeclaration(FieldDeclaration * fieldDeclaration);
void destroyMethodDeclaration(MethodDeclaration * methodDeclaration);
void destroyStatement(Statement * statement);
void destroyTypeSpecifier(TypeSpecifier * typeSpecifier);
void destroyAccessSpecifier(AccessSpecifier * accessSpecifier);
void destroyParameter(Parameter * parameter);
void destroyArgumentList(ArgumentList * argumentList);

#endif