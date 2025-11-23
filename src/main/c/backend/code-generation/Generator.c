#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include <stdio.h>
#include <stdarg.h>

#define VOID_KEYWORD "void"
#define CDT_NAME_FORMAT "%s_CDT"
#define CONSTRUCTOR_SUFFIX "_constructor"

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

static void _emitType(TypeSpecifier * type) {
    if (type == NULL) return;
    switch(type->type) {
        case INT_TYPE: printf("int"); break;
        case VOID_TYPE: printf(VOID_KEYWORD); break;
        case CHAR_TYPE: printf("char"); break;
        case STRING_TYPE: printf("char *"); break;
        case IDENTIFIER_TYPE: printf("%s", type->identifier); break;
        default: printf("unknown_type"); break;
    }
}

static void _generateTypedefs(Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * name = block->classDeclaration->identifier;
            printf("typedef struct " CDT_NAME_FORMAT " *%s;\n", name, name);
        }
        block = block->next;
    }
    printf("\n");
}

static void _generateStructs(Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            printf("struct " CDT_NAME_FORMAT " {\n", className);
            
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == FIELD_MEMBER) {
                    FieldDeclaration * field = member->fieldDeclaration;
                    if (!field->isStatic) {
                        printf("    ");
                        _emitType(field->typeSpecifier);
                        printf(" %s;\n", field->identifier);
                    }
                }
                member = member->next;
            }
            printf("};\n\n");
        }
        block = block->next;
    }
}

static void _generateStaticGlobals(Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == FIELD_MEMBER) {
                    FieldDeclaration * field = member->fieldDeclaration;
                    if (field->isStatic) {
                        _emitType(field->typeSpecifier);
                        printf(" %s_%s;\n", className, field->identifier);
                    }
                }
                member = member->next;
            }
        }
        block = block->next;
    }
    printf("\n");
}

static void _emitFunctionSignature(char * className, MethodDeclaration * method, int isStatic, int isConstructor) {
    if (isConstructor) {
        if (method->returnType) _emitType(method->returnType);
        else printf(VOID_KEYWORD);
        printf(" %s" CONSTRUCTOR_SUFFIX, className);
    } else {
        _emitType(method->returnType);
        printf(" %s_%s", className, method->identifier);
    }

    printf("(");
    int first = 1;
    
    if (!isStatic && !isConstructor) {
        printf("%s this", className);
        first = 0;
    }

    Parameter * param = method->parameterList;
    while (param != NULL) {
        if (!first) printf(", ");
        _emitType(param->typeSpecifier);
        printf(" %s", param->identifier);
        first = 0;
        param = param->next;
    }
    printf(")");
}

static void _generatePrototypes(Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == METHOD_MEMBER || member->type == CONSTRUCTOR_MEMBER) {
                    MethodDeclaration * method = member->methodDeclaration;
                    int isConstructor = (member->type == CONSTRUCTOR_MEMBER);
                    _emitFunctionSignature(className, method, method->isStatic, isConstructor);
                    printf(";\n");
                }
                member = member->next;
            }
        } else if (block->type == METHOD_BLOCK) {
             MethodDeclaration * method = block->methodDeclaration;
             _emitType(method->returnType);
             printf(" %s(", method->identifier);
             Parameter * param = method->parameterList;
             int first = 1;
             while (param != NULL) {
                if (!first) printf(", ");
                _emitType(param->typeSpecifier);
                printf(" %s", param->identifier);
                first = 0;
                param = param->next;
             }
             printf(");\n");
        }
        block = block->next;
    }
    printf("\n");
}

static void _generateBodies(Program * program) {
    BlockDeclaration * block = program->blockDeclaration;
    while (block != NULL) {
        if (block->type == CLASS_BLOCK) {
            char * className = block->classDeclaration->identifier;
            MemberDeclaration * member = block->classDeclaration->classBody->memberList;
            while (member != NULL) {
                if (member->type == METHOD_MEMBER || member->type == CONSTRUCTOR_MEMBER) {
                    MethodDeclaration * method = member->methodDeclaration;
                    int isConstructor = (member->type == CONSTRUCTOR_MEMBER);
                    _emitFunctionSignature(className, method, method->isStatic, isConstructor);
                    printf(" {\n    // TODO: Implementation\n}\n\n");
                }
                member = member->next;
            }
        } else if (block->type == METHOD_BLOCK) {
             MethodDeclaration * method = block->methodDeclaration;
             _emitType(method->returnType);
             printf(" %s(", method->identifier);
             Parameter * param = method->parameterList;
             int first = 1;
             while (param != NULL) {
                if (!first) printf(", ");
                _emitType(param->typeSpecifier);
                printf(" %s", param->identifier);
                first = 0;
                param = param->next;
             }
             printf(") {\n    // TODO: Implementation\n}\n\n");
        }
        block = block->next;
    }
}

void executeGenerator(CompilerState * compilerState) {
    logDebugging(_logger, "Generating C code...");
    Program * program = compilerState->abstractSyntaxtTree;
    
    _generateTypedefs(program);
    _generateStructs(program);
    _generateStaticGlobals(program);
    _generatePrototypes(program);
    _generateBodies(program);
    
    logDebugging(_logger, "Generation done.");
}
