#include <stdio.h>

typedef struct StackCDT * StackADT;

StackADT createStack(size_t itemSize);

void pushStack(StackADT stack, const void * data);

void popStack(StackADT stack, void * buffer);

int isEmptyStack(StackADT stack);

void freeStack(StackADT stack);
