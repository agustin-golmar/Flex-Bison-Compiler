#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include <stdio.h>
#include <stdarg.h>

#define CDT_NAME_FORMAT "%s_CDT"
#define CONSTRUCTOR_SUFFIX "_constructor"

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
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static void _emitType(int indentationLevel, TypeSpecifier * type);
static void _generateTypedefs(int indentationLevel, Program * program);
static void _generateStructs(int indentationLevel, Program * program);
static void _generateStaticGlobals(int indentationLevel, Program * program);
static void _emitFunctionSignature(int indentationLevel, char * className, MethodDeclaration * method, int isStatic, int isConstructor);
static const char * _expressionOperatorString(ExpressionType type);
static void _generateExpressionInline(Expression * expression);
static void _generateFactorInline(Factor * factor);
static void _generateArgumentListInline(ArgumentList * args);
static void _generateStatement(int indentationLevel, Statement * statement);
static void _generateStatementList(int indentationLevel, Statement * statement);
static void _generateForInitInline(Statement * init);
static void _generatePrototypes(int indentationLevel, Program * program);
static void _generateBodies(int indentationLevel, Program * program);

static char * _indentation(const unsigned int level) {
    return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
static void _output(const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	fflush(stdout);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

static void _emitType(int indentationLevel, TypeSpecifier * type) {
    if (type == NULL) return;
    switch(type->type) {
        case INT_TYPE: _output(indentationLevel, "int"); break;
        case VOID_TYPE: _output(indentationLevel, "void"); break;
        case CHAR_TYPE: _output(indentationLevel, "char"); break;
        case STRING_TYPE: _output(indentationLevel, "char *"); break;
        case IDENTIFIER_TYPE: _output(indentationLevel, "%s", type->identifier); break;
        default:
            logWarning(_logger, "Unknown TypeSpecifierType %d", type->type);
            _output(indentationLevel, "unknown_type"); break;
    }
}

static void _generateTypedefs(int indentationLevel, Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * name = block->classDeclaration->identifier;
            _output(indentationLevel, "typedef struct " CDT_NAME_FORMAT " *%s;\n", name, name);
        }
        block = block->next;
    }
    _output(0, "\n");
}

static void _generateStructs(int indentationLevel, Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            _output(indentationLevel, "struct " CDT_NAME_FORMAT " {\n", className);
            
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == FIELD_MEMBER) {
                    FieldDeclaration * field = member->fieldDeclaration;
                    if (!field->isStatic) {
                        _emitType(indentationLevel + 1, field->typeSpecifier);
                        _output(0, " %s;\n", field->identifier);
                    }
                }
                member = member->next;
            }
            _output(indentationLevel, "};\n\n");
        }
        block = block->next;
    }
}

static void _generateStaticGlobals(int indentationLevel, Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == FIELD_MEMBER) {
                    FieldDeclaration * field = member->fieldDeclaration;
                    if (field->isStatic) {
                        _emitType(indentationLevel, field->typeSpecifier);
                        _output(0, " %s_%s;\n", className, field->identifier);
                    }
                }
                member = member->next;
            }
        }
        block = block->next;
    }
    _output(0, "\n");
}

static void _emitFunctionSignature(int indentationLevel, char * className, MethodDeclaration * method, int isStatic, int isConstructor) {
    if (isConstructor) {
        if (method->returnType) _emitType(indentationLevel, method->returnType);
        else _output(indentationLevel, "void");
        _output(0, " %s" CONSTRUCTOR_SUFFIX, className);
    } else {
        _emitType(indentationLevel, method->returnType);
        _output(0, " %s_%s", className, method->identifier);
    }

    _output(0, "(");
    int first = 1;
    
    if (!isStatic && !isConstructor) {
        _output(0, "%s this", className);
        first = 0;
    }

    Parameter * param = method->parameterList;
    while (param != NULL) {
        if (!first) _output(0, ", ");
        _emitType(0, param->typeSpecifier);
        _output(0, " %s", param->identifier);
        first = 0;
        param = param->next;
    }
    _output(0, ")");
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

static void _generateFactorInline(Factor * factor) {
    if (factor == NULL) return;
    switch (factor->type) {
        case CONSTANT:
            _output(0, "%d", factor->constant->value);
            break;
        case EXPRESSION:
            _output(0, "(");
            _generateExpressionInline(factor->expression);
            _output(0, ")");
            break;
        default:
            logWarning(_logger, "Unknown FactorType %d", factor->type);
            _output(0, "/* unknown factor */");
            break;
    }
}

static void _generateArgumentListInline(ArgumentList * args) {
    ArgumentList * cur = args;
    int first = 1;
    while (cur != NULL) {
        if (!first) _output(0, ", ");
        _generateExpressionInline(cur->expression);
        first = 0;
        cur = cur->next;
    }
}

static void _generateExpressionInline(Expression * expression) {
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
            _output(0, "(");
            _generateExpressionInline(expression->leftExpression);
            op = _expressionOperatorString(expression->type);
            if (op) _output(0, " %s ", op);
            _generateExpressionInline(expression->rightExpression);
            _output(0, ")");
            break;
        case FACTOR:
            _generateFactorInline(expression->factor);
            break;
        case ASSIGNMENT:
            _generateExpressionInline(expression->leftExpression);
            _output(0, " = ");
            _generateExpressionInline(expression->rightExpression);
            break;
        case MEMBER_ACCESS:
            _generateExpressionInline(expression->leftExpression);
            _output(0, "->");
            if (expression->rightExpression != NULL && expression->rightExpression->type == IDENTIFIER_EXPRESSION) {
                _output(0, "%s", expression->rightExpression->identifier);
            } else {
                _generateExpressionInline(expression->rightExpression);
            }
            break;
        case FUNCTION_CALL:
            if (expression->identifier != NULL) {
                _output(0, "%s(", expression->identifier);
                _generateArgumentListInline(expression->argumentList);
                _output(0, ")");
            } else {
                // Could be indirect call; symbol table needed to resolve — leave TODO
                logWarning(_logger, "Unknown function call (unknown id)");
                _output(0, "/* TODO: function call (unknown id) */");
            }
            break;
        case IDENTIFIER_EXPRESSION:
            _output(0, "%s", expression->identifier);
            break;
        case INTEGER_EXPRESSION:
            _output(0, "%d", expression->integerValue);
            break;
        case STRING_LITERAL_EXPRESSION:
            _output(0, "\"%s\"", expression->stringLiteralValue);
            break;
        case THIS_EXPRESSION:
            _output(0, "this");
            break;
        case NEW_EXPRESSION:
            if (expression->identifier != NULL) {
                _output(0, "malloc(sizeof(struct " CDT_NAME_FORMAT "))", expression->identifier);
            } else {
                logWarning(_logger, "Unknown type in 'new' expression");
                _output(0, "/* TODO: new (unknown type) */");
            }
            break;
        case PRE_INCREMENT_EXPRESSION:
            _output(0, "++");
            _generateExpressionInline(expression->leftExpression);
            break;
        case POST_INCREMENT_EXPRESSION:
            _generateExpressionInline(expression->leftExpression);
            _output(0, "++");
            break;
        case PRE_DECREMENT_EXPRESSION:
            _output(0, "--");
            _generateExpressionInline(expression->leftExpression);
            break;
        case POST_DECREMENT_EXPRESSION:
            _generateExpressionInline(expression->leftExpression);
            _output(0, "--");
            break;
        case LOGICAL_NOT_EXPRESSION:
            _output(0, "!");
            _generateExpressionInline(expression->leftExpression);
            break;
        case NEGATION:
            _output(0, "-");
            _generateExpressionInline(expression->leftExpression);
            break;
        case EMPTY_EXPRESSION:
            break;
        default:
            logWarning(_logger, "Unknown ExpressionType %d", expression->type);
            _output(0, "/* TODO: expr type %d */", expression->type);
            break;
    }
}

static void _generateStatementList(int indentationLevel, Statement * statement) {
    Statement * cur = statement;
    while (cur != NULL) {
        _generateStatement(indentationLevel, cur);
        cur = cur->next;
    }
}

static void _generateForInitInline(Statement * init) {
    if (init == NULL) return;
    switch (init->type) {
        case DECLARATION_STATEMENT:
            _emitType(0, init->typeSpecifier);
            _output(0, " %s", init->identifier);
            break;
        case INITIALIZED_DECLARATION_STATEMENT:
            _emitType(0, init->typeSpecifier);
            _output(0, " %s = ", init->identifier);
            _generateExpressionInline(init->expression);
            break;
        case EXPRESSION_STATEMENT:
            _generateExpressionInline(init->expression);
            break;
        default:
            logWarning(_logger, "Unknown for-init statement type %d", init->type);
            _output(0, "/* TODO: for-init */");
            break;
    }
}

static void _generateStatement(int indentationLevel, Statement * statement) {
    if (statement == NULL) return;
    switch (statement->type) {
        case EXPRESSION_STATEMENT:
            _output(indentationLevel, "");
            _generateExpressionInline(statement->expression);
            _output(0, ";\n");
            break;
        case DECLARATION_STATEMENT:
            _emitType(indentationLevel, statement->typeSpecifier);
            _output(0, " %s;\n", statement->identifier);
            break;
        case INITIALIZED_DECLARATION_STATEMENT:
            _emitType(indentationLevel, statement->typeSpecifier);
            _output(0, " %s = ", statement->identifier);
            _generateExpressionInline(statement->expression);
            _output(0, ";\n");
            break;
        case RETURN_STATEMENT:
            _output(indentationLevel, "return ");
            _generateExpressionInline(statement->expression);
            _output(0, ";\n");
            break;
        case RETURN_VOID_STATEMENT:
            _output(indentationLevel, "return;\n");
            break;
        case COMPOUND_STATEMENT:
            _output(indentationLevel, "{\n");
            _generateStatementList(indentationLevel + 1, statement->statementList);
            _output(indentationLevel, "}\n");
            break;
        case IF_STATEMENT:
            _output(indentationLevel, "if (");
            _generateExpressionInline(statement->condition);
            _output(0, ") ");
            if (statement->statementList != NULL && statement->statementList->type == COMPOUND_STATEMENT) {
                _generateStatement(indentationLevel, statement->statementList);
            } else {
                _output(0, "{\n");
                _generateStatementList(indentationLevel + 1, statement->statementList);
                _output(indentationLevel, "}\n");
            }
            /* Note: else handling requires AST extension / symbol table for disambiguation */
            break;
        case WHILE_STATEMENT:
            _output(indentationLevel, "while (");
            _generateExpressionInline(statement->condition);
            _output(0, ") ");
            if (statement->statementList != NULL && statement->statementList->type == COMPOUND_STATEMENT) {
                _generateStatement(indentationLevel, statement->statementList);
            } else {
                _output(0, "{\n");
                _generateStatementList(indentationLevel + 1, statement->statementList);
                _output(indentationLevel, "}\n");
            }
            break;
        case DO_WHILE_STATEMENT:
            _output(indentationLevel, "do {\n");
            _generateStatementList(indentationLevel + 1, statement->statementList);
            _output(indentationLevel, "} while (");
            _generateExpressionInline(statement->condition);
            _output(0, ");\n");
            break;
        case FOR_STATEMENT:
            _output(indentationLevel, "for (");
            _generateForInitInline(statement->initialization);
            _output(0, "; ");
            if (statement->loopCondition != NULL) _generateExpressionInline(statement->loopCondition);
            _output(0, "; ");
            if (statement->postIteration != NULL) _generateExpressionInline(statement->postIteration);
            _output(0, ") ");
            if (statement->statementList != NULL && statement->statementList->type == COMPOUND_STATEMENT) {
                _generateStatement(indentationLevel, statement->statementList);
            } else {
                _output(0, "{\n");
                _generateStatementList(indentationLevel + 1, statement->statementList);
                _output(indentationLevel, "}\n");
            }
            break;
        case EMPTY_STATEMENT:
            _output(indentationLevel, ";\n");
            break;
        default:
            logWarning(_logger, "Unknown statement type %d", statement->type);
            _output(indentationLevel, "/* TODO: statement type %d */\n", statement->type);
            break;
    }
}

static void _generatePrototypes(int indentationLevel, Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == METHOD_MEMBER || member->type == CONSTRUCTOR_MEMBER) {
                    MethodDeclaration * method = member->methodDeclaration;
                    int isConstructor = (member->type == CONSTRUCTOR_MEMBER);
                    _emitFunctionSignature(indentationLevel, className, method, method->isStatic, isConstructor);
                    _output(0, ";\n");
                }
                member = member->next;
            }
        } else if (block->type == METHOD_BLOCK) {
             MethodDeclaration * method = block->methodDeclaration;
             _emitType(indentationLevel, method->returnType);
             _output(0, " %s(", method->identifier);
             Parameter * param = method->parameterList;
             int first = 1;
             while (param != NULL) {
                if (!first) _output(0, ", ");
                _emitType(0, param->typeSpecifier);
                _output(0, " %s", param->identifier);
                first = 0;
                param = param->next;
             }
             _output(0, ");\n");
        }
        block = block->next;
    }
    _output(0, "\n");
}

static void _generateBodies(int indentationLevel, Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == METHOD_MEMBER || member->type == CONSTRUCTOR_MEMBER) {
                    MethodDeclaration * method = member->methodDeclaration;
                    int isConstructor = (member->type == CONSTRUCTOR_MEMBER);
                    _emitFunctionSignature(indentationLevel, className, method, method->isStatic, isConstructor);
                    _output(0, " {\n");
                    // Generate method body from its Statement list
                    if (method->statementList != NULL) {
                        _generateStatementList(indentationLevel + 1, method->statementList);
                    } else {
                        _output(indentationLevel + 1, "// empty body\n");
                    }
                    _output(indentationLevel, "}\n\n");
                }
                member = member->next;
            }
        } else if (block->type == METHOD_BLOCK) {
             MethodDeclaration * method = block->methodDeclaration;
             _emitType(indentationLevel, method->returnType);
             _output(0, " %s(", method->identifier);
             Parameter * param = method->parameterList;
             int first = 1;
             while (param != NULL) {
                if (!first) _output(0, ", ");
                _emitType(0, param->typeSpecifier);
                _output(0, " %s", param->identifier);
                first = 0;
                param = param->next;
             }
             _output(0, ") {\n");
             if (method->statementList != NULL) {
                 _generateStatementList(indentationLevel + 1, method->statementList);
             } else {
                 _output(indentationLevel + 1, "// empty body\n");
             }
             _output(indentationLevel, "}\n\n");
        }
        block = block->next;
    }
}

void executeGenerator(CompilerState * compilerState) {
    logDebugging(_logger, "Generating C code...");
    Program * program = compilerState->abstractSyntaxtTree;
    
    _generateTypedefs(0, program);
    _generateStructs(0, program);
    _generateStaticGlobals(0, program);
    _generatePrototypes(0, program);
    _generateBodies(0, program);
    
    logDebugging(_logger, "Generation done.");
}
