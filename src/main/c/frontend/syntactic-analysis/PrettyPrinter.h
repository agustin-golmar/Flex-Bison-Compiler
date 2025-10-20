/* PrettyPrinter.h
 * Small utility to print Expression nodes with focus on left/right children.
 */
#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "AbstractSyntaxTree.h"

/* Print a brief, indented description of the expression tree rooted at `expr`.
 * The output emphasizes left and right sub-expressions for binary nodes.
 */
void printExpressionBrief(const Expression *expr);

#endif /* PRETTY_PRINTER_H */
