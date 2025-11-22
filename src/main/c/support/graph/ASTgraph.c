#include "ASTgraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define LABEL_BUF_SIZE 256
#define MAX_CHILDREN 64

#define COLOR_LEAF   "\x1B[38;5;208m"
#define COLOR_IDENTIFIER "\x1B[34m"
#define COLOR_RESET  "\x1B[0m"

/* --------------------------
 * Generic wrapper and enums
 * -------------------------- */

 typedef enum {
    AST_PROGRAM,
    AST_BLOCK_DECLARATION,
    AST_CLASS_DECLARATION,
    AST_CLASS_BODY,
    AST_MEMBER_DECLARATION,
    AST_FIELD_DECLARATION,
    AST_METHOD_DECLARATION,
    AST_STATEMENT,
    AST_EXPRESSION,
    AST_FACTOR,
    AST_CONSTANT,
    AST_TYPE_SPECIFIER,
    AST_ACCESS_SPECIFIER,
    AST_PARAMETER,
    AST_ARGUMENT_LIST,
    AST_UNKNOWN
} ASTNodeKind;

typedef struct {
    ASTNodeKind kind;
    void *ptr;
} ASTNode;

static inline ASTNode makeASTNode(ASTNodeKind k, void *p) {
    ASTNode n;
    n.kind = k;
    n.ptr = p;
    return n;
}

typedef struct stem_t *stem;
struct stem_t { const char *str; stem next; };

/* --------------------------
 * Label helper
 * -------------------------- */

static void fmtLabel(char *buf, size_t bufsize, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, bufsize, fmt, ap);
    va_end(ap);
}

static const char *expressionTypeName(ExpressionType t) {
    switch (t) {
#define X(e) case e: return #e;
        X(ADDITION) X(DIVISION) X(FACTOR) X(MULTIPLICATION) X(SUBTRACTION)
        X(GREATER_THAN_EXPRESSION) X(LOWER_THAN_EXPRESSION) X(GREATER_OR_EQUAL_THAN_EXPRESSION)
        X(LOWER_OR_EQUAL_THAN_EXPRESSION) X(EQUAL_EXPRESSION) X(NOT_EQUAL_EXPRESSION)
        X(LOGICAL_AND_EXPRESSION) X(LOGICAL_OR_EXPRESSION) X(LOGICAL_NOT_EXPRESSION)
        X(ASSIGNMENT) X(NEGATION) X(MEMBER_ACCESS) X(FUNCTION_CALL) X(IDENTIFIER_EXPRESSION)
        X(INTEGER_EXPRESSION) X(THIS_EXPRESSION) X(NEW_EXPRESSION) X(POST_INCREMENT_EXPRESSION)
        X(PRE_INCREMENT_EXPRESSION) X(POST_DECREMENT_EXPRESSION) X(PRE_DECREMENT_EXPRESSION)
        X(EMPTY_EXPRESSION) X(STRING_LITERAL_EXPRESSION)
#undef X
    default: return "UNKNOWN_EXPRESSION";
    }
}

static const char *factorTypeName(FactorType t) {
    switch (t) {
    case CONSTANT: return "CONSTANT";
    case EXPRESSION: return "EXPRESSION";
    default: return "UNKNOWN_FACTOR";
    }
}

static const char *statementTypeName(StatementType t) {
    switch (t) {
#define S(x) case x: return #x;
        S(EXPRESSION_STATEMENT) S(DECLARATION_STATEMENT) S(INITIALIZED_DECLARATION_STATEMENT)
        S(RETURN_STATEMENT) S(RETURN_VOID_STATEMENT) S(COMPOUND_STATEMENT)
        S(IF_STATEMENT) S(FOR_STATEMENT) S(WHILE_STATEMENT) S(DO_WHILE_STATEMENT)
        S(EMPTY_STATEMENT)
#undef S
    default: return "UNKNOWN_STATEMENT";
    }
}

static const char *typeSpecifierName(TypeSpecifierType t) {
    switch (t) {
    case INT_TYPE: return "int";
    case VOID_TYPE: return "void";
    case CHAR_TYPE: return "char";
    case STRING_TYPE: return "string";
    case IDENTIFIER_TYPE: return "identifier";
    default: return "unknown_type";
    }
}

static const char *accessSpecifierName(AccessSpecifierType t) {
    switch (t) {
    case PRIVATE_ACCESS: return "private";
    case PUBLIC_ACCESS: return "public";
    default: return "unknown_access";
    }
}

/* --------------------------
 * ASTNodeLabel
 * -------------------------- */

static void ASTNodeLabel(ASTNode node, char *buf, size_t bufsz) {
    if (!buf || bufsz == 0) return;
    buf[0] = '\0';

    if (node.ptr == NULL) {
        fmtLabel(buf, bufsz, "NULL");
        return;
    }

    switch (node.kind) {
    case AST_PROGRAM: {
        Program *p = (Program*)node.ptr;
        fmtLabel(buf, bufsz, "Program");
        if (!p) strncat(buf, " (NULL)", bufsz - strlen(buf) - 1);
        break;
    }

    case AST_BLOCK_DECLARATION: {
        BlockDeclaration *b = (BlockDeclaration*)node.ptr;
        fmtLabel(buf, bufsz, "BlockDeclaration");
        if (b && b->type == CLASS_BLOCK) strncat(buf, " (CLASS_BLOCK)", bufsz - strlen(buf) - 1);
        else if (b && b->type == METHOD_BLOCK) strncat(buf, " (METHOD_BLOCK)", bufsz - strlen(buf) - 1);
        break;
    }

    case AST_CLASS_DECLARATION: {
        ClassDeclaration *c = (ClassDeclaration*)node.ptr;
        fmtLabel(buf, bufsz, "ClassDeclaration");
        if (c && c->identifier) fmtLabel(buf, bufsz, " \"%s%s%s\"", COLOR_IDENTIFIER, c->identifier, COLOR_RESET);
        break;
    }

    case AST_CLASS_BODY: {
        fmtLabel(buf, bufsz, "ClassBody");
        break;
    }

    case AST_MEMBER_DECLARATION: {
        MemberDeclaration *m = (MemberDeclaration*)node.ptr;
        if (!m) { fmtLabel(buf, bufsz, "MemberDeclaration (NULL)"); break; }
        fmtLabel(buf, bufsz, "MemberDeclaration (%s)",
                  m->type == FIELD_MEMBER ? "FIELD" :
                  m->type == METHOD_MEMBER ? "METHOD" :
                  m->type == CONSTRUCTOR_MEMBER ? "CONSTRUCTOR" :
                  m->type == DESTRUCTOR_MEMBER ? "DESTRUCTOR" : "UNKNOWN");
        break;
    }

    case AST_FIELD_DECLARATION: {
        FieldDeclaration *f = (FieldDeclaration*)node.ptr;
        if (!f) { fmtLabel(buf, bufsz, "FieldDeclaration (NULL)"); break; }
        if (f->typeSpecifier && f->typeSpecifier->type == IDENTIFIER_TYPE) {
            fmtLabel(buf, bufsz, "Field: %s%s%s : %s%s",
                COLOR_IDENTIFIER, f->identifier ? f->identifier : "(anon)", COLOR_RESET,
                      f->typeSpecifier->identifier ? f->typeSpecifier->identifier : "identifier",
                      f->isStatic ? " [static]" : "");
        } else {
            fmtLabel(buf, bufsz, "Field: %s%s%s : %s%s",
                COLOR_IDENTIFIER, f->identifier ? f->identifier : "(anon)", COLOR_RESET,
                      f->typeSpecifier ? typeSpecifierName(f->typeSpecifier->type) : "unknown",
                      f->isStatic ? " [static]" : "");
        }
        break;
    }

    case AST_METHOD_DECLARATION: {
        MethodDeclaration *m = (MethodDeclaration*)node.ptr;
        if (!m) { fmtLabel(buf, bufsz, "MethodDeclaration (NULL)"); break; }
        const char *ret = m->returnType ? (m->returnType->type == IDENTIFIER_TYPE ? (m->returnType->identifier ? m->returnType->identifier : "identifier") : typeSpecifierName(m->returnType->type)) : "??";
        fmtLabel(buf, bufsz, "Method: %s%s%s -> %s%s",
            COLOR_IDENTIFIER, m->identifier ? m->identifier : "(anon)", COLOR_RESET,
                  ret,
                  m->isStatic ? " [static]" : "");
        break;
    }

    case AST_STATEMENT: {
        Statement *s = (Statement*)node.ptr;
        if (!s) { fmtLabel(buf, bufsz, "Statement (NULL)"); break; }
        fmtLabel(buf, bufsz, "Statement (%s)", statementTypeName(s->type));
        break;
    }

    case AST_EXPRESSION: {
        Expression *e = (Expression*)node.ptr;
        if (!e) { fmtLabel(buf, bufsz, "Expression (NULL)"); break; }
        const char *etype = expressionTypeName(e->type);

        if (e->type == IDENTIFIER_EXPRESSION) {
            fmtLabel(buf, bufsz, "Expression (%s) \"%s%s%s\"",
                      etype,
                      COLOR_IDENTIFIER, e->identifier ? e->identifier : "(anon)", COLOR_RESET);
        } else if (e->type == INTEGER_EXPRESSION) {
            fmtLabel(buf, bufsz, "Expression (%s) %d", etype, e->integerValue);
        } else if (e->type == STRING_LITERAL_EXPRESSION) {
            fmtLabel(buf, bufsz, "Expression (%s) \"%s\"", etype, e->stringLiteralValue ? e->stringLiteralValue : "");
        } else if (e->type == FUNCTION_CALL) {
            fmtLabel(buf, bufsz, "Expression (%s) \"%s%s%s\"()", etype, COLOR_IDENTIFIER, e->identifier ? e->identifier : "(fn)", COLOR_RESET);
        } else if (e->type == MEMBER_ACCESS) {
            fmtLabel(buf, bufsz, "Expression (%s) \"%s%s%s\"", etype, COLOR_IDENTIFIER, e->identifier ? e->identifier : "(member)", COLOR_RESET);
        } else if (e->type == THIS_EXPRESSION) {
            fmtLabel(buf, bufsz, "Expression (THIS)");
        } else if (e->type == NEW_EXPRESSION) {
            fmtLabel(buf, bufsz, "Expression (NEW) \"%s%s%s\"", COLOR_IDENTIFIER, e->identifier ? e->identifier : "(type)", COLOR_RESET);
        } else {
            fmtLabel(buf, bufsz, "Expression (%s)", etype);
        }
        break;
    }

    case AST_FACTOR: {
        Factor *f = (Factor*)node.ptr;
        if (!f) { fmtLabel(buf, bufsz, "Factor (NULL)"); break; }
        fmtLabel(buf, bufsz, "Factor (%s)", factorTypeName(f->type));
        break;
    }

    case AST_CONSTANT: {
        Constant *c = (Constant*)node.ptr;
        if (!c) { fmtLabel(buf, bufsz, "Constant (NULL)"); break; }
        fmtLabel(buf, bufsz, "Constant (%d)", c->value);
        break;
    }

    case AST_TYPE_SPECIFIER: {
        TypeSpecifier *t = (TypeSpecifier*)node.ptr;
        if (!t) { fmtLabel(buf, bufsz, "TypeSpecifier (NULL)"); break; }
        if (t->type == IDENTIFIER_TYPE) fmtLabel(buf, bufsz, "Type: %s", t->identifier ? t->identifier : "identifier");
        else fmtLabel(buf, bufsz, "Type: %s", typeSpecifierName(t->type));
        break;
    }

    case AST_ACCESS_SPECIFIER: {
        AccessSpecifier *a = (AccessSpecifier*)node.ptr;
        if (!a) { fmtLabel(buf, bufsz, "AccessSpecifier (NULL)"); break; }
        fmtLabel(buf, bufsz, "Access: %s", accessSpecifierName(a->type));
        break;
    }

    case AST_PARAMETER: {
        Parameter *p = (Parameter*)node.ptr;
        if (!p) { fmtLabel(buf, bufsz, "Parameter (NULL)"); break; }
        if (p->typeSpecifier) {
            if (p->typeSpecifier->type == IDENTIFIER_TYPE)
            fmtLabel(buf, bufsz, "Param: %s%s%s : %s", COLOR_IDENTIFIER, p->identifier ? p->identifier : "(anon)", COLOR_RESET, p->typeSpecifier->identifier ? p->typeSpecifier->identifier : "identifier");
            else
            fmtLabel(buf, bufsz, "Param: %s%s%s : %s", COLOR_IDENTIFIER, p->identifier ? p->identifier : "(anon)", COLOR_RESET, typeSpecifierName(p->typeSpecifier->type));
        } else {
            fmtLabel(buf, bufsz, "Param: %s%s%s", COLOR_IDENTIFIER, p->identifier ? p->identifier : "(anon)", COLOR_RESET);
        }
        break;
    }

    case AST_ARGUMENT_LIST: {
        fmtLabel(buf, bufsz, "ArgumentList");
        break;
    }

    default:
        fmtLabel(buf, bufsz, "UnknownNode");
        break;
    }
}

/* --------------------------
 * ASTChildren
 * -------------------------- */

static void ASTChildren(ASTNode node, ASTNode children[], int *out_count) {
    int count = 0;
    if (!out_count) return;
    *out_count = 0;
    if (node.ptr == NULL) { return; }

    switch (node.kind) {

    case AST_PROGRAM: {
        Program *p = (Program*)node.ptr;
        BlockDeclaration *b = p ? p->blockDeclaration : NULL;
        while (b && count < MAX_CHILDREN) {
            children[count++] = makeASTNode(AST_BLOCK_DECLARATION, b);
            b = b->next;
        }
        break;
    }

    case AST_BLOCK_DECLARATION: {
        BlockDeclaration *b = (BlockDeclaration*)node.ptr;
        if (!b) break;
        if (b->type == CLASS_BLOCK && b->classDeclaration) {
            children[count++] = makeASTNode(AST_CLASS_DECLARATION, b->classDeclaration);
        } else if (b->type == METHOD_BLOCK && b->methodDeclaration) {
            children[count++] = makeASTNode(AST_METHOD_DECLARATION, b->methodDeclaration);
        }
        break;
    }

    case AST_CLASS_DECLARATION: {
        ClassDeclaration *c = (ClassDeclaration*)node.ptr;
        if (!c) break;
        if (c->classBody) children[count++] = makeASTNode(AST_CLASS_BODY, c->classBody);
        break;
    }

    case AST_CLASS_BODY: {
        ClassBody *cb = (ClassBody*)node.ptr;
        if (!cb) break;
        MemberDeclaration *m = cb->memberList;
        while (m && count < MAX_CHILDREN) {
            children[count++] = makeASTNode(AST_MEMBER_DECLARATION, m);
            m = m->next;
        }
        break;
    }

    case AST_MEMBER_DECLARATION: {
        MemberDeclaration *m = (MemberDeclaration*)node.ptr;
        if (!m) break;
        if (m->accessSpecifier) children[count++] = makeASTNode(AST_ACCESS_SPECIFIER, m->accessSpecifier);
        if (m->type == FIELD_MEMBER && m->fieldDeclaration) {
            children[count++] = makeASTNode(AST_FIELD_DECLARATION, m->fieldDeclaration);
        } else if ((m->type == METHOD_MEMBER || m->type == CONSTRUCTOR_MEMBER || m->type == DESTRUCTOR_MEMBER) && m->methodDeclaration) {
            children[count++] = makeASTNode(AST_METHOD_DECLARATION, m->methodDeclaration);
        }
        break;
    }

    case AST_FIELD_DECLARATION: {
        FieldDeclaration *f = (FieldDeclaration*)node.ptr;
        if (!f) break;
        if (f->typeSpecifier) children[count++] = makeASTNode(AST_TYPE_SPECIFIER, f->typeSpecifier);
        if (f->initializationExpression) children[count++] = makeASTNode(AST_EXPRESSION, f->initializationExpression);
        break;
    }

    case AST_METHOD_DECLARATION: {
        MethodDeclaration *m = (MethodDeclaration*)node.ptr;
        if (!m) break;
        if (m->returnType) children[count++] = makeASTNode(AST_TYPE_SPECIFIER, m->returnType);
        Parameter *p = m->parameterList;
        while (p && count < MAX_CHILDREN) {
            children[count++] = makeASTNode(AST_PARAMETER, p);
            p = p->next;
        }
        Statement *s = m->statementList;
        while (s && count < MAX_CHILDREN) {
            children[count++] = makeASTNode(AST_STATEMENT, s);
            s = s->next;
        }
        break;
    }

    case AST_STATEMENT: {
        Statement *s = (Statement*)node.ptr;
        if (!s) break;
        switch (s->type) {
        case EXPRESSION_STATEMENT:
            if (s->expression) children[count++] = makeASTNode(AST_EXPRESSION, s->expression);
            break;
        case DECLARATION_STATEMENT:
            if (s->typeSpecifier) children[count++] = makeASTNode(AST_TYPE_SPECIFIER, s->typeSpecifier);
            if (s->identifier) {
            }
            break;
        case INITIALIZED_DECLARATION_STATEMENT:
            if (s->typeSpecifier) children[count++] = makeASTNode(AST_TYPE_SPECIFIER, s->typeSpecifier);
            if (s->expression) children[count++] = makeASTNode(AST_EXPRESSION, s->expression);
            break;
        case RETURN_STATEMENT:
            if (s->expression) children[count++] = makeASTNode(AST_EXPRESSION, s->expression);
            break;
        case RETURN_VOID_STATEMENT:
            break;
        case COMPOUND_STATEMENT:
            {
                Statement *ss = s->statementList;
                while (ss && count < MAX_CHILDREN) {
                    children[count++] = makeASTNode(AST_STATEMENT, ss);
                    ss = ss->next;
                }
            }
            break;
        case IF_STATEMENT:
            if (s->condition) children[count++] = makeASTNode(AST_EXPRESSION, s->condition);
            if (s->statementList) {
                Statement *ss = s->statementList;
                while (ss && count < MAX_CHILDREN) {
                    children[count++] = makeASTNode(AST_STATEMENT, ss);
                    ss = ss->next;
                }
            }
            break;
        case FOR_STATEMENT:
            if (s->initialization) children[count++] = makeASTNode(AST_STATEMENT, s->initialization);
            if (s->loopCondition) children[count++] = makeASTNode(AST_EXPRESSION, s->loopCondition);
            if (s->postIteration) children[count++] = makeASTNode(AST_EXPRESSION, s->postIteration);
            if (s->statementList) {
                Statement *ss = s->statementList;
                while (ss && count < MAX_CHILDREN) {
                    children[count++] = makeASTNode(AST_STATEMENT, ss);
                    ss = ss->next;
                }
            }
            break;
        case WHILE_STATEMENT:
            if (s->loopCondition) children[count++] = makeASTNode(AST_EXPRESSION, s->loopCondition);
            if (s->statementList) {
                Statement *ss = s->statementList;
                while (ss && count < MAX_CHILDREN) {
                    children[count++] = makeASTNode(AST_STATEMENT, ss);
                    ss = ss->next;
                }
            }
            break;
        case DO_WHILE_STATEMENT:
            if (s->statementList) {
                Statement *ss = s->statementList;
                while (ss && count < MAX_CHILDREN) {
                    children[count++] = makeASTNode(AST_STATEMENT, ss);
                    ss = ss->next;
                }
            }
            if (s->loopCondition) children[count++] = makeASTNode(AST_EXPRESSION, s->loopCondition);
            break;
        case EMPTY_STATEMENT:
            /* nothing */
            break;
        default:
            break;
        }
        break;
    }

    case AST_EXPRESSION: {
        Expression *e = (Expression*)node.ptr;
        if (!e) break;
        switch (e->type) {
        case ADDITION: case SUBTRACTION: case MULTIPLICATION: case DIVISION:
        case GREATER_THAN_EXPRESSION: case LOWER_THAN_EXPRESSION:
        case GREATER_OR_EQUAL_THAN_EXPRESSION: case LOWER_OR_EQUAL_THAN_EXPRESSION:
        case EQUAL_EXPRESSION: case NOT_EQUAL_EXPRESSION:
        case LOGICAL_AND_EXPRESSION: case LOGICAL_OR_EXPRESSION:
            if (e->leftExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->leftExpression);
            if (e->rightExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->rightExpression);
            break;

        case ASSIGNMENT:
            if (e->leftExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->leftExpression);
            if (e->rightExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->rightExpression);
            break;
        case FUNCTION_CALL:
            if (e->argumentList) children[count++] = makeASTNode(AST_ARGUMENT_LIST, e->argumentList);
            break;

        case MEMBER_ACCESS:
            if (e->factor) children[count++] = makeASTNode(AST_FACTOR, e->factor);
            break;

        case IDENTIFIER_EXPRESSION:
            if (e->argumentList) children[count++] = makeASTNode(AST_ARGUMENT_LIST, e->argumentList);
            break;

        case INTEGER_EXPRESSION:
        case STRING_LITERAL_EXPRESSION:
        case THIS_EXPRESSION:
        case EMPTY_EXPRESSION:
            break;

        case PRE_INCREMENT_EXPRESSION: case POST_INCREMENT_EXPRESSION:
        case PRE_DECREMENT_EXPRESSION: case POST_DECREMENT_EXPRESSION:
            if (e->leftExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->leftExpression);
            break;
        case NEGATION: case LOGICAL_NOT_EXPRESSION:
            if (e->factor) children[count++] = makeASTNode(AST_FACTOR, e->factor);
            break;

        case FACTOR:
            if (e->factor) children[count++] = makeASTNode(AST_FACTOR, e->factor);
            break;

        case NEW_EXPRESSION:
            if (e->argumentList) children[count++] = makeASTNode(AST_ARGUMENT_LIST, e->argumentList);
            break;

        default:
            if (e->factor) children[count++] = makeASTNode(AST_FACTOR, e->factor);
            if (e->leftExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->leftExpression);
            if (e->rightExpression) children[count++] = makeASTNode(AST_EXPRESSION, e->rightExpression);
            break;
        }
        break;
    }

    case AST_FACTOR: {
        Factor *f = (Factor*)node.ptr;
        if (!f) break;
        if (f->type == CONSTANT && f->constant) children[count++] = makeASTNode(AST_CONSTANT, f->constant);
        else if (f->type == EXPRESSION && f->expression) children[count++] = makeASTNode(AST_EXPRESSION, f->expression);
        break;
    }

    case AST_CONSTANT:
        break;

    case AST_TYPE_SPECIFIER:
        break;

    case AST_ACCESS_SPECIFIER:
        break;

    case AST_PARAMETER: {
        Parameter *p = (Parameter*)node.ptr;
        if (!p) break;
        if (p->typeSpecifier) children[count++] = makeASTNode(AST_TYPE_SPECIFIER, p->typeSpecifier);
        break;
    }

    case AST_ARGUMENT_LIST: {
        ArgumentList *a = (ArgumentList*)node.ptr;
        while (a && count < MAX_CHILDREN) {
            if (a->expression) children[count++] = makeASTNode(AST_EXPRESSION, a->expression);
            a = a->next;
        }
        break;
    }

    default:
        break;
    }

    *out_count = count;
}

/* --------------------------
 * printAST
 * -------------------------- */

void printAST(ASTNode node, stem head) {
    static const char *sdown = "  |", *slAST = "  `", *snone = "   ";
    struct stem_t col = {0, 0}, *tail;

    ASTNode children[MAX_CHILDREN];
    int child_count = 0;
    ASTChildren(node, children, &child_count);

    for (tail = head; tail; tail = tail->next) {
        printf("%s", tail->str);
        if (!tail->next) break;
    }

    char label[LABEL_BUF_SIZE];
    ASTNodeLabel(node, label, sizeof(label));
    
    if (child_count == 0) {
        printf("--%s%s%s\n", COLOR_LEAF, label, COLOR_RESET);
        return;
    } else {
        printf("--%s\n", label);
    }

    if (tail && tail->str == slAST)
        tail->str = snone;

    if (!tail) head = &col, tail = &col;
    else tail->next = &col;

    for (int i = 0; i < child_count; ++i) {
        col.str = (i + 1 < child_count) ? sdown : slAST;
        printAST(children[i], head);
    }

    tail->next = 0;
}

/* --------------------------
 * printASTgraph
 * -------------------------- */

void printASTgraph(Program * program) {
    if (!program) {
        printf("--Program (NULL)\n");
        return;
    }
    printAST(makeASTNode(AST_PROGRAM, program), NULL);
}
