#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(Expression * expression);

/**
 * Extended semantic actions for class-based grammar.
 */

Program * BlockProgramSemanticAction(BlockDeclaration * blockDeclaration);
BlockDeclaration * SingleBlockDeclarationSemanticAction(BlockDeclaration * blockDeclaration);
BlockDeclaration * MultipleBlockDeclarationSemanticAction(BlockDeclaration * blockDeclarationList, BlockDeclaration * blockDeclaration);
BlockDeclaration * ClassBlockDeclarationSemanticAction(ClassDeclaration * classDeclaration);
BlockDeclaration * MethodBlockDeclarationSemanticAction(MethodDeclaration * MethodDeclaration);
ClassDeclaration * ClassDeclarationSemanticAction(char * identifier, ClassBody * classBody);
ClassBody * EmptyClassBodySemanticAction();
ClassBody * MemberListClassBodySemanticAction(MemberDeclaration * memberList);
MemberDeclaration * SingleMemberSemanticAction(MemberDeclaration * member);
MemberDeclaration * MultipleMemberSemanticAction(MemberDeclaration * memberList, MemberDeclaration * member);
MemberDeclaration * AccessSpecifiedMemberSemanticAction(AccessSpecifier * accessSpecifier, MemberDeclaration * memberDeclaration);
MemberDeclaration * DefaultAccessMemberSemanticAction(MemberDeclaration * memberDeclaration);
AccessSpecifier * PrivateAccessSemanticAction();
AccessSpecifier * PublicAccessSemanticAction();
MemberDeclaration * FieldMemberSemanticAction(FieldDeclaration * fieldDeclaration);
MemberDeclaration * MethodMemberSemanticAction(MethodDeclaration * methodDeclaration);
MemberDeclaration * ConstructorMemberSemanticAction(MethodDeclaration * constructorDeclaration);
MemberDeclaration * DestructorMemberSemanticAction(MethodDeclaration * destructorDeclaration);
FieldDeclaration * FieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier);
FieldDeclaration * StaticFieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier);
FieldDeclaration * InitializedFieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier, Expression * expression);
FieldDeclaration * InitializedStaticFieldDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier, Expression * expression);
MethodDeclaration * MethodDeclarationSemanticAction(TypeSpecifier * returnType, char * identifier, Parameter * parameterList, Statement * statementList);
MethodDeclaration * StaticMethodDeclarationSemanticAction(TypeSpecifier * returnType, char * identifier, Parameter * parameterList, Statement * statementList);
MethodDeclaration * ConstructorDeclarationSemanticAction(Parameter * parameterList, Statement * statementList);
MethodDeclaration * DestructorDeclarationSemanticAction(Statement * statementList);
Parameter * EmptyParameterListSemanticAction();
Parameter * ParametersSemanticAction(Parameter * parameters);
Parameter * SingleParameterSemanticAction(Parameter * parameter);
Parameter * MultipleParameterSemanticAction(Parameter * parameterList, Parameter * parameter);
Parameter * ParameterSemanticAction(TypeSpecifier * typeSpecifier, char * identifier);
TypeSpecifier * IntTypeSemanticAction();
TypeSpecifier * VoidTypeSemanticAction();
TypeSpecifier * CharTypeSemanticAction();
TypeSpecifier * IdentifierTypeSemanticAction(char * identifier);
Statement * EmptyStatementListSemanticAction();
Statement * StatementListSemanticAction(Statement * statementList, Statement * statement);
Statement * ExpressionStatementSemanticAction(Statement * expressionStatement);
Statement * DeclarationStatementSemanticAction(Statement * declarationStatement);
Statement * ReturnStatementSemanticAction(Statement * returnStatement);
Statement * CompoundStatementSemanticAction(Statement * compoundStatement);
Statement * CompoundStatementBodySemanticAction(Statement * statementList);
Statement * IfStatementSemanticAction(Statement * ifStatement);
Statement * IfStatementBodySemanticAction(Expression * expression, Statement * statement);
Statement * IfElseStatementSemanticAction(Statement * ifStatement, Statement * elseStatement);
Statement * VariableDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier);
Statement * InitializedVariableDeclarationSemanticAction(TypeSpecifier * typeSpecifier, char * identifier, Expression * expression);
Statement * ExpressionStatementBodySemanticAction(Expression * expression);
Statement * ReturnExpressionSemanticAction(Expression * expression);
Statement * ReturnVoidSemanticAction();
Expression * AssignmentExpressionSemanticAction(Expression * expression);
Expression * AdditiveExpressionSemanticAction(Expression * expression);
Expression * AssignmentSemanticAction(Expression * leftExpression, Expression * rightExpression);
Expression * MultiplicativeExpressionSemanticAction(Expression * expression);
Expression * AdditionSemanticAction(Expression * leftExpression, Expression * rightExpression);
Expression * SubtractionSemanticAction(Expression * leftExpression, Expression * rightExpression);
Expression * UnaryExpressionSemanticAction(Expression * expression);
Expression * MultiplicationSemanticAction(Expression * leftExpression, Expression * rightExpression);
Expression * DivisionSemanticAction(Expression * leftExpression, Expression * rightExpression);
Expression * PostfixExpressionSemanticAction(Expression * expression);
Expression * NegationSemanticAction(Expression * expression);
Expression * PrimaryExpressionSemanticAction(Expression * expression);
Expression * MemberAccessSemanticAction(Expression * expression, char * identifier);
Expression * FunctionCallSemanticAction(Expression * expression, ArgumentList * argumentList);
Expression * IdentifierExpressionSemanticAction(char * identifier);
Expression * IntegerExpressionSemanticAction(int value);
Expression * ThisExpressionSemanticAction();
Expression * ParenthesizedExpressionSemanticAction(Expression * expression);
Expression * NewExpressionSemanticAction(char * identifier, ArgumentList * argumentList);
ArgumentList * EmptyArgumentListSemanticAction();
ArgumentList * ArgumentsSemanticAction(ArgumentList * arguments);
ArgumentList * SingleArgumentSemanticAction(Expression * expression);
ArgumentList * MultipleArgumentSemanticAction(ArgumentList * argumentList, Expression * expression);

#endif