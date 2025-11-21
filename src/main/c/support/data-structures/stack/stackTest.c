#include "stack.h"
#include <assert.h>

void stackTest(void) {
    int buffer;

    // single int stack
    StackADT stack1 = createStack(sizeof(int));
    assert(isEmptyStack(stack1) == 1);
    int int1 = 1;
    pushStack(stack1, &int1);
    assert(isEmptyStack(stack1) == 0);
    popStack(stack1, &buffer);
    assert(buffer == int1);
    freeStack(stack1);

    // multiple chars stack
    StackADT stack2 = createStack(sizeof(char));
    assert(isEmptyStack(stack2) == 1);
    char chars[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};
    for(int i = 0; i < 12; i++) {
        pushStack(stack2, &chars[i]);
    }
    assert(isEmptyStack(stack2) == 0);
    for(int i = 11; i >= 0; i--) {
        popStack(stack2, &chars[i]);
    }
    assert(isEmptyStack(stack2) == 1);
    freeStack(stack2);
}
