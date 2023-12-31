#include "stack.h"
#include <string.h>

Stack *initStack()
{
    Stack *newStack = malloc(sizeof(Stack));
    newStack->data = malloc(sizeof(void*) * INITIAL_STACK_SIZE);
    newStack->top = -1;
    newStack->stackSize = INITIAL_STACK_SIZE;

    return newStack;
}

void push(Stack *stack, void *ptr)
{
    if(stack == NULL){
        return; 
    }
    if (stack->top == stack->stackSize - 1)
    {
        stack->data = realloc(stack->data, stack->stackSize * 2);
        stack->stackSize *= 2;
    }
    stack->top++; 
    stack->data[stack->top] = ptr;
}

void *pop(Stack *stack)
{
    if(stack == NULL){
        return NULL; 
    }
    if (stack->top < 0)
    {
        return NULL;
    }
    else
    {
        void *popped = stack->data[stack->top];
        stack->data[stack->top--] = NULL;
        return popped;
    }
}


void *peek(Stack *stack)
{
    if(stack->top < 0){
        return NULL; 
    }
    return stack->data[stack->top];
}

void clearStack(Stack *stack, void(*structClearFunc)(void*)){ 
    if(stack == NULL){
        return; 
    }
    if(stack->top != -1){
        void* popped;
        while((popped = pop(stack))){
            structClearFunc(popped);
        }
    } 
}

void cleanupStack(Stack *stack, void(*structClearFunc)(void*))
{
    if(stack == NULL){
        return; 
    }
    if(stack->top != -1){
        void* popped; 
        while((popped = pop(stack))){
            structClearFunc(popped);
        }
    }
    free(stack->data);  
    free(stack);
}
