#include "PrettyPrinter.h"
#include <stdio.h>
#include <string.h>

/* Internal helper: print indentation */
static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) fputs("  ", stdout);
}

/* Describe the node type as short string */
static const char *exprTypeName(ExpressionType t) {
    switch (t) {
        case ADDITION: return "ADDITION";
        case DIVISION: return "DIVISION";
        case FACTOR: return "FACTOR";
        case MULTIPLICATION: return "MULTIPLICATION";
        case SUBTRACTION: return "SUBTRACTION";
        case ASSIGNMENT: return "ASSIGNMENT";
        case NEGATION: return "NEGATION";
        case MEMBER_ACCESS: return "MEMBER_ACCESS";
        case FUNCTION_CALL: return "FUNCTION_CALL";
        case IDENTIFIER_EXPRESSION: return "IDENTIFIER";
        case INTEGER_EXPRESSION: return "INTEGER";
        case THIS_EXPRESSION: return "THIS";
        case NEW_EXPRESSION: return "NEW";
        default: return "UNKNOWN";
    }
}

/* Recursive printer that focuses on left/right children. Leaves print identifying data. */
static void printExpressionRec(const Expression *e, int indent) {
    if (e == NULL) {
        printIndent(indent);
        puts("<null>");
        return;
    }

    printIndent(indent);
    printf("%s", exprTypeName(e->type));

    /* For leaves that are integer or identifier, print value/identifier */
    if (e->type == INTEGER_EXPRESSION) {
        printf(" : %d", e->integerValue);
    } else if (e->type == IDENTIFIER_EXPRESSION) {
        printf(" : %s", e->identifier ? e->identifier : "(null)");
    }
    putchar('\n');

    /* If the node has left/right expressions, print them under labels */
    switch (e->type) {
        case ADDITION:
        case DIVISION:
        case MULTIPLICATION:
        case SUBTRACTION:
        case ASSIGNMENT:
            printIndent(indent);
            puts("Left:");
            printExpressionRec(e->leftExpression, indent + 1);
            printIndent(indent);
            puts("Right:");
            printExpressionRec(e->rightExpression, indent + 1);
            break;

        case FACTOR:
            /* factor is a wrapper; recurse into factor->expression if present */
            if (e->factor && e->factor->type == EXPRESSION) {
                printIndent(indent);
                puts("Factor->Expression:");
                printExpressionRec(e->factor->expression, indent + 1);
            }
            break;

        case FUNCTION_CALL:
            /* function call might have identifier and an argument list; show identifier and first argument */
            printIndent(indent);
            printf("Function: %s\n", e->identifier ? e->identifier : "(anonymous)");
            if (e->argumentList) {
                printIndent(indent);
                puts("Args:");
                for (ArgumentList *a = e->argumentList; a; a = a->next) {
                    printExpressionRec(a->expression, indent + 1);
                }
            }
            break;

        default:
            /* other node types either don't have left/right or are handled above */
            break;
    }
}

void printExpressionBrief(const Expression *expr) {
    printExpressionRec(expr, 0);
}
