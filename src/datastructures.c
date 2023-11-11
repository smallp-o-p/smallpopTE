#include "pch.h"

Stack *initStack()
{
    Stack* newStack = malloc(sizeof(Stack));
    newStack->data = malloc(INITIAL_STACK_SIZE);
    newStack->top = -1;
    newStack->stackSize = INITIAL_STACK_SIZE;

    return newStack;
}

pastTextRow* pop(Stack *stack)
{
    if (stack->top == -1)
    {
        return NULL;
    }
    else
    {
        pastTextRow *ptrToData = stack->data[stack->top];
        stack->data[stack->top--] = NULL;
        return ptrToData;
    }
}

void push(Stack *stack, pastTextRow *row)
{
    if (stack->top == stack->stackSize - 1)
    {
        stack->data = realloc(stack->data, stack->stackSize * 2);
        stack->stackSize *= 2;
    }
    stack->data[++stack->top] = row;
}

pastTextRow *peek(Stack *stack)
{
    return stack->data[stack->top];
}

void cleanupStack(Stack* stack){
    for(int i = 0; i < stack->top; i++){
        free(stack->data[i]);
    }
    free(stack->data);
    free(stack); 
}
