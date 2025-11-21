#include "stack.h"
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 10

struct StackCDT {
    void * items;
    size_t itemSize;
    size_t itemsCount;
};

static void resize(StackADT stack);

StackADT createStack(size_t itemSize) {
    if(itemSize == 0) {
        return NULL;
    }

    StackADT stack = calloc(1, sizeof(struct StackCDT));
    stack->itemSize = itemSize;
    return stack;
}

void pushStack(StackADT stack, const void * data) {
    if(stack == NULL || data == NULL) {
        return;
    }

    resize(stack);
    
    memcpy((char *)stack->items + stack->itemsCount * stack->itemSize, data, stack->itemSize);
    
    stack->itemsCount++;
}

static void resize(StackADT stack) {
    if(stack->itemsCount % BLOCK_SIZE == 0) {
        stack->items = realloc(stack->items, (stack->itemsCount + BLOCK_SIZE) * stack->itemSize);
    }
}

void popStack(StackADT stack, void * buffer) {
    if(stack == NULL || buffer == NULL || isEmptyStack(stack)) {
        return;
    }

    stack->itemsCount--;

    memcpy(buffer, (char *)stack->items + stack->itemsCount * stack->itemSize, stack->itemSize);
}

int isEmptyStack(StackADT stack) {
    return stack == NULL || stack->itemsCount == 0;
}

void freeStack(StackADT stack) {
    if(stack == NULL) {
        return;
    }
    
    free(stack->items);
    free(stack);
}
