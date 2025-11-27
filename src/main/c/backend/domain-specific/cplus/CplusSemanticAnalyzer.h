#ifndef __CPLUS_SEMANTIC_ANALYZER_H__
#define __CPLUS_SEMANTIC_ANALYZER_H__

#include "../../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../../support/logging/Logger.h"
#include "../../../support/type/CompilerState.h"
#include "../../../support/type/ModuleDestructor.h"
#include <limits.h>
#include <stdbool.h>

typedef struct {
    bool succeeded;
} ComputationResult;

ModuleDestructor initializeCplusSemanticAnalyzerModule();


// NOTE: we might only need to expose this function
ComputationResult executeSemanticalAnalysis(CompilerState * compilerState);

ComputationResult computeProgram(Program * program);
ComputationResult computeBlock(BlockDeclaration * blockDeclaration);
ComputationResult computeFunctionBlock(MethodDeclaration * function);
ComputationResult computeClassDeclaration(ClassDeclaration * classDecl);
ComputationResult computeClassBody(ClassBody * body, char * classIdentifier);
ComputationResult computeMemberDeclaration(MemberDeclaration * member, char * classIdentifier);
ComputationResult computeFieldDeclaration(FieldDeclaration * field);
ComputationResult computeMethodDeclaration(MethodDeclaration * method, char * classIdentifier);
ComputationResult computeStatement(Statement * statement);
ComputationResult computeExpression(Expression * expression);
ComputationResult computeFactor(Factor * factor);
ComputationResult computeArgumentList(ArgumentList * args);
ComputationResult computeParameterList(Parameter * params);


#endif
