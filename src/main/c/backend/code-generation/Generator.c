#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include <stdio.h>
#include <stdarg.h>

#define VOID_KEYWORD "void"
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
        case VOID_TYPE: _output(indentationLevel, VOID_KEYWORD); break;
        case CHAR_TYPE: _output(indentationLevel, "char"); break;
        case STRING_TYPE: _output(indentationLevel, "char *"); break;
        case IDENTIFIER_TYPE: _output(indentationLevel, "%s", type->identifier); break;
        default: _output(indentationLevel, "unknown_type"); break;
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
        else _output(indentationLevel, VOID_KEYWORD);
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
                    _output(indentationLevel + 1, "// TODO: Implementation\n");
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
             _output(indentationLevel + 1, "// TODO: Implementation\n");
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
