#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

/*  "This answer is wrong; "--graph" option only prints the LR(1)/LALR(1) automaton, not the AST (Abstract Syntax Tree).
     See gnu.org/software/bison/manual/html_node/Graphviz.html for more detail."
     -- Agus Golmar, https://stackoverflow.com/questions/8566430/how-to-show-abstract-syntax-tree-of-a-grammar-in-bison
*/ 

void printASTgraph(Program * program);
