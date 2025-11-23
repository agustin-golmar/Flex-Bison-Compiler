#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include <stdio.h>
#include <stdarg.h>

#define CDT_NAME_FORMAT "%s_CDT"
#define ADT_NAME_SUFFIX "_ADT"
#define INCLUDE_PREFIX "#include \""
#define CONSTRUCTOR_SUFFIX "_constructor"
#define MAIN_FILE_NAME "main.c"

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

void _shutdownGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeGeneratorModule() {
    _logger = createLogger("Generator");
    return _shutdownGeneratorModule;
}

static char * _indentation(const unsigned int level);
static void _output(FILE * outputFile, const unsigned int indentationLevel, const char * const format, ...);
static void _emitType(FILE * outputFile, int indentationLevel, TypeSpecifier * type);
static void _generateTypedef(FILE * headerFile, int indentationLevel, ClassDeclaration * classDeclaration);
static void _generateStructs(FILE * sourceFile, int indentationLevel, ClassDeclaration * classDeclaration);
static void _generateStaticGlobals(FILE * sourceFile, int indentationLevel, ClassDeclaration * classDeclaration);
static void _emitFunctionSignature(FILE * sourceFile, int indentationLevel, char * className, MethodDeclaration * method, int isStatic, int isConstructor);
static const char * _expressionOperatorString(ExpressionType type);
static void _generateExpressionInline(FILE * sourceFile, Expression * expression);
static void _generateFactorInline(FILE * sourceFile, Factor * factor);
static void _generateArgumentListInline(FILE * sourceFile, ArgumentList * args);
static void _generateStatement(FILE * sourceFile, int indentationLevel, Statement * statement);
static void _generateStatementList(FILE * sourceFile, int indentationLevel, Statement * statement);
static void _generateForInitInline(FILE * sourceFile, Statement * init);
static void _generateClassPrototypes(FILE * outputFile, int indentationLevel, ClassDeclaration * classDeclaration);
static void _generateMethodsPrototypes(FILE * outputFile, int indentationLevel, BlockDeclaration * block);
static void _generateClassBody(FILE * outputFile, int indentationLevel, ClassDeclaration * classDeclaration);
static void _generateMethodsBodies(FILE * outputFile, int indentationLevel, BlockDeclaration * block);
static void _generateHeaderIncludes(FILE * mainFile, FILE * sourceFile, int indentationLevel, char * headerFileName);
static void _generateADTHeader(char ** headerFileName, ClassDeclaration * classDeclaration);
static void _generateADTSource(FILE * mainFile, char * headerFileName, ClassDeclaration * classDeclaration);
static void _generateADT(FILE * mainFile, ClassDeclaration * classDeclaration);
static void _generateMethod(FILE * mainFile, MethodDeclaration * methodDeclaration);
static void _generateProgram(Program * program);

static char * _indentation(const unsigned int level) {
    return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
static void _output(FILE * outputFile, const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(outputFile, effectiveFormat, arguments);
	fflush(outputFile);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

static void _emitType(FILE * outputFile, int indentationLevel, TypeSpecifier * type) {
    if (type == NULL) return;
    switch(type->type) {
        case INT_TYPE: _output(outputFile, indentationLevel, "int"); break;
        case VOID_TYPE: _output(outputFile, indentationLevel, "void"); break;
        case CHAR_TYPE: _output(outputFile, indentationLevel, "char"); break;
        case STRING_TYPE: _output(outputFile, indentationLevel, "char *"); break;
        case IDENTIFIER_TYPE: _output(outputFile, indentationLevel, "%s", type->identifier); break;
        default:
            logWarning(_logger, "Unknown TypeSpecifierType %d", type->type);
            _output(outputFile, indentationLevel, "unknown_type"); break;
    }
}

static void _emitFunctionSignature(FILE * outputFile, int indentationLevel, char * className, MethodDeclaration * method, int isStatic, int isConstructor) {
    if (isConstructor) {
        _output(outputFile, indentationLevel, className);
        _output(outputFile, 0, " %s" CONSTRUCTOR_SUFFIX, className);
    } else {
        _emitType(outputFile, indentationLevel, method->returnType);
        _output(outputFile, 0, " %s_%s", className, method->identifier);
    }

    _output(outputFile, 0, "(");
    int first = 1;
    
    if (!isStatic && !isConstructor) {
        _output(outputFile, 0, "%s this", className);
        first = 0;
    }

    Parameter * param = method->parameterList;
    while (param != NULL) {
        if (!first) _output(outputFile, 0, ", ");
        _emitType(outputFile, 0, param->typeSpecifier);
        _output(outputFile, 0, " %s", param->identifier);
        first = 0;
        param = param->next;
    }
    _output(outputFile, 0, ")");
}

/** Expression / Statement generation helpers (inline, no leading indentation) */
static const char * _expressionOperatorString(ExpressionType type) {
    switch (type) {
        case ADDITION: return "+";
        case SUBTRACTION: return "-";
        case MULTIPLICATION: return "*";
        case DIVISION: return "/";
        case GREATER_THAN_EXPRESSION: return ">";
        case LOWER_THAN_EXPRESSION: return "<";
        case GREATER_OR_EQUAL_THAN_EXPRESSION: return ">=";
        case LOWER_OR_EQUAL_THAN_EXPRESSION: return "<=";
        case EQUAL_EXPRESSION: return "==";
        case NOT_EQUAL_EXPRESSION: return "!=";
        case LOGICAL_AND_EXPRESSION: return "&&";
        case LOGICAL_OR_EXPRESSION: return "||";
        default:
            logWarning(_logger, "Unknown ExpressionType %d", type);
            return NULL;
    }
}

static void _generateFactorInline(FILE * outputFile, Factor * factor) {
    if (factor == NULL) return;
    switch (factor->type) {
        case CONSTANT:
            _output(outputFile, 0, "%d", factor->constant->value);
            break;
        case EXPRESSION:
            _output(outputFile, 0, "(");
            _generateExpressionInline(outputFile, factor->expression);
            _output(outputFile, 0, ")");
            break;
        default:
            logWarning(_logger, "Unknown FactorType %d", factor->type);
            _output(outputFile, 0, "/* unknown factor */");
            break;
    }
}

static void _generateArgumentListInline(FILE * outputFile, ArgumentList * args) {
    ArgumentList * cur = args;
    int first = 1;
    while (cur != NULL) {
        if (!first) _output(outputFile, 0, ", ");
        _generateExpressionInline(outputFile, cur->expression);
        first = 0;
        cur = cur->next;
    }
}

static void _generateExpressionInline(FILE * outputFile, Expression * expression) {
    if (expression == NULL) return;
    const char * op;
    switch (expression->type) {
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
        case GREATER_THAN_EXPRESSION:
        case LOWER_THAN_EXPRESSION:
        case GREATER_OR_EQUAL_THAN_EXPRESSION:
        case LOWER_OR_EQUAL_THAN_EXPRESSION:
        case EQUAL_EXPRESSION:
        case NOT_EQUAL_EXPRESSION:
        case LOGICAL_AND_EXPRESSION:
        case LOGICAL_OR_EXPRESSION:
            _output(outputFile, 0, "(");
            _generateExpressionInline(outputFile, expression->leftExpression);
            op = _expressionOperatorString(expression->type);
            if (op) _output(outputFile, 0, " %s ", op);
            _generateExpressionInline(outputFile, expression->rightExpression);
            _output(outputFile, 0, ")");
            break;
        case FACTOR:
            _generateFactorInline(outputFile, expression->factor);
            break;
        case ASSIGNMENT:
            _generateExpressionInline(outputFile, expression->leftExpression);
            _output(outputFile, 0, " = ");
            _generateExpressionInline(outputFile, expression->rightExpression);
            break;
        case MEMBER_ACCESS:
            _generateExpressionInline(outputFile, expression->leftExpression);
            _output(outputFile, 0, "->");
            if (expression->rightExpression != NULL && expression->rightExpression->type == IDENTIFIER_EXPRESSION) {
                _output(outputFile, 0, "%s", expression->rightExpression->identifier);
            } else {
                _generateExpressionInline(outputFile, expression->rightExpression);
            }
            break;
        case FUNCTION_CALL:
            if (expression->identifier != NULL) {
                _generateExpressionInline(outputFile, expression->precedingExpression);
                _output(outputFile, 0, "(");
                _generateArgumentListInline(outputFile, expression->args);
                _output(outputFile, 0, ")");
            } else {
                // Could be indirect call; symbol table needed to resolve — leave TODO
                logWarning(_logger, "Unknown function call (unknown id)");
                _output(outputFile, 0, "/* TODO: function call (unknown id) */");
            }
            break;
        case IDENTIFIER_EXPRESSION:
            _output(outputFile, 0, "%s", expression->identifier);
            break;
        case INTEGER_EXPRESSION:
            _output(outputFile, 0, "%d", expression->integerValue);
            break;
        case STRING_LITERAL_EXPRESSION:
            _output(outputFile, 0, "\"%s\"", expression->stringLiteralValue);
            break;
        case THIS_EXPRESSION:
            _output(outputFile, 0, "this");
            break;
        case NEW_EXPRESSION:
            if (expression->identifier != NULL) {
                _output(outputFile, 0, "%s" CONSTRUCTOR_SUFFIX "(", expression->identifier);
                _generateArgumentListInline(outputFile, expression->argumentList);
                _output(outputFile, 0, ")");
            } else {
                logWarning(_logger, "Unknown type in 'new' expression");
                _output(outputFile, 0, "/* TODO: new (unknown type) */");
            }
            break;
        case PRE_INCREMENT_EXPRESSION:
            _output(outputFile, 0, "++");
            _generateExpressionInline(outputFile, expression->leftExpression);
            break;
        case POST_INCREMENT_EXPRESSION:
            _generateExpressionInline(outputFile, expression->leftExpression);
            _output(outputFile, 0, "++");
            break;
        case PRE_DECREMENT_EXPRESSION:
            _output(outputFile, 0, "--");
            _generateExpressionInline(outputFile, expression->leftExpression);
            break;
        case POST_DECREMENT_EXPRESSION:
            _generateExpressionInline(outputFile, expression->leftExpression);
            _output(outputFile, 0, "--");
            break;
        case LOGICAL_NOT_EXPRESSION:
            _output(outputFile, 0, "!");
            _generateExpressionInline(outputFile, expression->leftExpression);
            break;
        case NEGATION:
            _output(outputFile, 0, "-");
            _generateExpressionInline(outputFile, expression->leftExpression);
            break;
        case EMPTY_EXPRESSION:
            break;
        default:
            logWarning(_logger, "Unknown ExpressionType %d", expression->type);
            _output(outputFile, 0, "/* TODO: expr type %d */", expression->type);
            break;
    }
}

static void _generateStatementList(FILE * outputFile, int indentationLevel, Statement * statement) {
    Statement * cur = statement;
    while (cur != NULL) {
        _generateStatement(outputFile, indentationLevel, cur);
        cur = cur->next;
    }
}

static void _generateForInitInline(FILE * outputFile, Statement * init) {
    if (init == NULL) return;
    switch (init->type) {
        case DECLARATION_STATEMENT:
            _emitType(outputFile, 0, init->typeSpecifier);
            _output(outputFile, 0, " %s", init->identifier);
            break;
        case INITIALIZED_DECLARATION_STATEMENT:
            _emitType(outputFile, 0, init->typeSpecifier);
            _output(outputFile, 0, " %s = ", init->identifier);
            _generateExpressionInline(outputFile, init->expression);
            break;
        case EXPRESSION_STATEMENT:
            _generateExpressionInline(outputFile, init->expression);
            break;
        default:
            logWarning(_logger, "Unknown for-init statement type %d", init->type);
            _output(outputFile, 0, "/* TODO: for-init */");
            break;
    }
}

static void _generateStatement(FILE * outputFile, int indentationLevel, Statement * statement) {
    if (statement == NULL) return;
    switch (statement->type) {
        case EXPRESSION_STATEMENT:
            _output(outputFile, indentationLevel, "");
            _generateExpressionInline(outputFile, statement->expression);
            _output(outputFile, 0, ";\n");
            break;
        case DECLARATION_STATEMENT:
            _emitType(outputFile, indentationLevel, statement->typeSpecifier);
            _output(outputFile, 0, " %s;\n", statement->identifier);
            break;
        case INITIALIZED_DECLARATION_STATEMENT:
            _emitType(outputFile, indentationLevel, statement->typeSpecifier);
            _output(outputFile, 0, " %s = ", statement->identifier);
            _generateExpressionInline(outputFile, statement->expression);
            _output(outputFile, 0, ";\n");
            break;
        case RETURN_STATEMENT:
            _output(outputFile, indentationLevel, "return ");
            _generateExpressionInline(outputFile, statement->expression);
            _output(outputFile, 0, ";\n");
            break;
        case RETURN_VOID_STATEMENT:
            _output(outputFile, indentationLevel, "return;\n");
            break;
        case COMPOUND_STATEMENT:
            _output(outputFile, indentationLevel, "{\n");
            _generateStatementList(outputFile, indentationLevel + 1, statement->statementList);
            _output(outputFile, indentationLevel, "}\n");
            break;
        case IF_STATEMENT:
            _output(outputFile, indentationLevel, "if (");
            _generateExpressionInline(outputFile, statement->condition);
            _output(outputFile, 0, ") ");
            if (statement->statementList != NULL && statement->statementList->type == COMPOUND_STATEMENT) {
                _generateStatement(outputFile, indentationLevel, statement->statementList);
            } else {
                _output(outputFile, 0, "{\n");
                _generateStatementList(outputFile, indentationLevel + 1, statement->statementList);
                _output(outputFile, indentationLevel, "}\n");
            }
            /* Note: else handling requires AST extension / symbol table for disambiguation */
            break;
        case WHILE_STATEMENT:
            _output(outputFile, indentationLevel, "while (");
            _generateExpressionInline(outputFile, statement->condition);
            _output(outputFile, 0, ") ");
            if (statement->statementList != NULL && statement->statementList->type == COMPOUND_STATEMENT) {
                _generateStatement(outputFile, indentationLevel, statement->statementList);
            } else {
                _output(outputFile, 0, "{\n");
                _generateStatementList(outputFile, indentationLevel + 1, statement->statementList);
                _output(outputFile, indentationLevel, "}\n");
            }
            break;
        case DO_WHILE_STATEMENT:
            _output(outputFile, indentationLevel, "do {\n");
            _generateStatementList(outputFile, indentationLevel + 1, statement->statementList);
            _output(outputFile, indentationLevel, "} while (");
            _generateExpressionInline(outputFile, statement->condition);
            _output(outputFile, 0, ");\n");
            break;
        case FOR_STATEMENT:
            _output(outputFile, indentationLevel, "for (");
            _generateForInitInline(outputFile, statement->initialization);
            _output(outputFile, 0, "; ");
            if (statement->loopCondition != NULL) _generateExpressionInline(outputFile, statement->loopCondition);
            _output(outputFile, 0, "; ");
            if (statement->postIteration != NULL) _generateExpressionInline(outputFile, statement->postIteration);
            _output(outputFile, 0, ") ");
            if (statement->statementList != NULL && statement->statementList->type == COMPOUND_STATEMENT) {
                _generateStatement(outputFile, indentationLevel, statement->statementList);
            } else {
                _output(outputFile, 0, "{\n");
                _generateStatementList(outputFile, indentationLevel + 1, statement->statementList);
                _output(outputFile, indentationLevel, "}\n");
            }
            break;
        case EMPTY_STATEMENT:
            _output(outputFile, indentationLevel, ";\n");
            break;
        default:
            logWarning(_logger, "Unknown statement type %d", statement->type);
            _output(outputFile, indentationLevel, "/* TODO: statement type %d */\n", statement->type);
            break;
    }
}


// <=========================================================== ADT ===========================================================>

// <--------------------------------------------------------- source --------------------------------------------------------->

static void _generateClassBody(FILE * outputFile, int indentationLevel, ClassDeclaration * classDeclaration) {
    char * className = classDeclaration->identifier;
    MemberDeclaration * member = classDeclaration->classBody->memberList;
    while (member != NULL) {
        if (member->type == METHOD_MEMBER || member->type == CONSTRUCTOR_MEMBER) {
            MethodDeclaration * method = member->methodDeclaration;
            int isConstructor = (member->type == CONSTRUCTOR_MEMBER);
            _emitFunctionSignature(outputFile, indentationLevel, className, method, method->isStatic, isConstructor);
            _output(outputFile, 0, " {\n");
            if (isConstructor) {
                _output(outputFile, indentationLevel + 1, "%s this = malloc(sizeof(struct " CDT_NAME_FORMAT "));\n", className, className);
            }
            // Generate method body from its Statement list
            if (method->statementList != NULL) {
                _generateStatementList(outputFile, indentationLevel + 1, method->statementList);
            } else {
                _output(outputFile, indentationLevel + 1, "// empty body\n");
            }
            if (isConstructor) {
                _output(outputFile, indentationLevel + 1, "return this;\n");
            }
            _output(outputFile, indentationLevel, "}\n\n");
        }
        member = member->next;
    }
}

static void _generateStaticGlobals(FILE * sourceFile, int indentationLevel, ClassDeclaration * classDeclaration) {
    char * className = classDeclaration->identifier;
    MemberDeclaration * member = classDeclaration->classBody->memberList;
    while (member != NULL) {
        if (member->type == FIELD_MEMBER) {
            FieldDeclaration * field = member->fieldDeclaration;
            if (field->isStatic) {
                _emitType(sourceFile, indentationLevel, field->typeSpecifier);
                _output(sourceFile, 0, " %s_%s", className, field->identifier);
                if (field->initializationExpression != NULL) {
                    _output(sourceFile, 0, " = ");
                    _generateExpressionInline(sourceFile, field->initializationExpression);
                }
                _output(sourceFile, 0, ";\n");
            }
        }
        member = member->next;
    }
    _output(sourceFile, 0, "\n");
}

static void _generateStructs(FILE * sourceFile, int indentationLevel, ClassDeclaration * classDeclaration) {
    char * className = classDeclaration->identifier;
    _output(sourceFile, indentationLevel, "struct " CDT_NAME_FORMAT " {\n", className);
    
    MemberDeclaration * member = classDeclaration->classBody->memberList;
    while (member != NULL) {
        if (member->type == FIELD_MEMBER) {
            FieldDeclaration * field = member->fieldDeclaration;
            if (!field->isStatic) {
                _emitType(sourceFile, indentationLevel + 1, field->typeSpecifier);
                _output(sourceFile, 0, " %s;\n", field->identifier);
            }
        }
        member = member->next;
    }
    _output(sourceFile, indentationLevel, "};\n\n");
}

static void _generateHeaderIncludes(FILE * mainFile, FILE * sourceFile, int indentationLevel, char * headerFileName) {
    char * headerInclude = concatenate(3, INCLUDE_PREFIX, headerFileName, "\"\n\n");
    _output(mainFile, indentationLevel, headerInclude);
    _output(sourceFile, indentationLevel, headerInclude);
    free(headerInclude);
}

static void _generateADTSource(FILE * mainFile, char * headerFileName, ClassDeclaration * classDeclaration) {
    FILE * sourceFile;
    char * sourceFileName = concatenate(3, classDeclaration->identifier, ADT_NAME_SUFFIX, ".c");
    sourceFile = fopen(sourceFileName, "w");

    _generateHeaderIncludes(mainFile, sourceFile, 0, headerFileName);
    _generateStructs(sourceFile, 0, classDeclaration);
    _generateStaticGlobals(sourceFile, 0, classDeclaration);
    _generateClassBody(sourceFile, 0, classDeclaration);

    fclose(sourceFile);
    free(sourceFileName);
}

// <-------------------------------------------------------------------------------------------------------------------------->

// <--------------------------------------------------------- header --------------------------------------------------------->

static void _generateTypedef(FILE * headerFile, int indentationLevel, ClassDeclaration * classDeclaration) {
    char * name = classDeclaration->identifier;
    _output(headerFile, indentationLevel, "typedef struct " CDT_NAME_FORMAT " * %s;\n\n", name, name);
}

static void _generateClassPrototypes(FILE * outputFile, int indentationLevel, ClassDeclaration * classDeclaration) {
    char * className = classDeclaration->identifier;
    MemberDeclaration * member = classDeclaration->classBody->memberList;
    while (member != NULL) {
        if (member->type == METHOD_MEMBER || member->type == CONSTRUCTOR_MEMBER) {
            MethodDeclaration * method = member->methodDeclaration;
            int isConstructor = (member->type == CONSTRUCTOR_MEMBER);
            _emitFunctionSignature(outputFile, indentationLevel, className, method, method->isStatic, isConstructor);
            _output(outputFile, 0, ";\n\n");
        }
        member = member->next;
    }
}

static void _generateADTHeader(char ** headerFileName, ClassDeclaration * classDeclaration) {
    FILE * headerFile;
    *headerFileName = concatenate(3, classDeclaration->identifier, ADT_NAME_SUFFIX, ".h");
    headerFile = fopen(*headerFileName, "w");

    _generateTypedef(headerFile, 0, classDeclaration);
    _generateClassPrototypes(headerFile, 0, classDeclaration);

    fclose(headerFile);
}

// <-------------------------------------------------------------------------------------------------------------------------->

static void _generateADT(FILE * mainFile, ClassDeclaration * classDeclaration) {
    char * headerFileName;
    _generateADTHeader(&headerFileName, classDeclaration);
    _generateADTSource(mainFile, headerFileName, classDeclaration);
    free(headerFileName);
}

// <===============================================================================================================================>


// <=========================================================== METHODS ===========================================================>

static void _generateMethodsBodies(FILE * mainFile, int indentationLevel, BlockDeclaration * block) {
    while (block != NULL) {
        MethodDeclaration * method = block->methodDeclaration;
        _emitType(mainFile, indentationLevel, method->returnType);
        _output(mainFile, 0, " %s(", method->identifier);
        Parameter * param = method->parameterList;
        int first = 1;
        while (param != NULL) {
            if (!first) _output(mainFile, 0, ", ");
            _emitType(mainFile, 0, param->typeSpecifier);
            _output(mainFile, 0, " %s", param->identifier);
            first = 0;
            param = param->next;
        }
        _output(mainFile, 0, ") {\n");
        if (method->statementList != NULL) {
            _generateStatementList(mainFile, indentationLevel + 1, method->statementList);
        } else {
            _output(mainFile, indentationLevel + 1, "// empty body\n");
        }
        _output(mainFile, indentationLevel, "}\n\n");

        block = block->next;
    }
}

static void _generateMethodsPrototypes(FILE * mainFile, int indentationLevel, BlockDeclaration * block) {
    while(block != NULL) {
        MethodDeclaration * method = block->methodDeclaration;
        if(strcmp(method->identifier, "main") != 0) {
            _emitType(mainFile, indentationLevel, method->returnType);
            _output(mainFile, 0, " %s(", method->identifier);
            Parameter * param = method->parameterList;
            int first = 1;
            while (param != NULL) {
                if (!first) _output(mainFile, 0, ", ");
                _emitType(mainFile, 0, param->typeSpecifier);
                _output(mainFile, 0, " %s", param->identifier);
                first = 0;
                param = param->next;
            }
            _output(mainFile, 0, ");\n");
        }

        block = block->next;
    }
}

static void _generateMethods(FILE * mainFile, BlockDeclaration * methodDeclaration) {
    _generateMethodsPrototypes(mainFile, 0, methodDeclaration);
    _generateMethodsBodies(mainFile, 0, methodDeclaration);
}

// <===============================================================================================================================>


// <=========================================================== PROGRAM ===========================================================>

static void _generateProgram(Program * program) {
    FILE * mainFile = fopen(MAIN_FILE_NAME, "w");

    BlockDeclaration * block = program->blockDeclaration, * aux, * methodDeclarations = NULL;
    
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            _generateADT(mainFile, block->classDeclaration);
            block = block->next;
        } else if(block->type == METHOD_BLOCK) {
            aux = block;
            block = block->next;
            aux->next = methodDeclarations;
            methodDeclarations = aux;
        } else {
            block = block->next;
        }
    }

    _generateMethods(mainFile, methodDeclarations);

    fclose(mainFile);
}

// <===============================================================================================================================>


void executeGenerator(CompilerState * compilerState) {
    logDebugging(_logger, "Generating C code...");
    Program * program = compilerState->abstractSyntaxtTree;

    _generateProgram(program);
    
    logDebugging(_logger, "Generation done.");
}
