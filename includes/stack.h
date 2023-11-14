#pragma once 

#include <stdio.h> 
#include <stdlib.h> 

#define INITIAL_STACK_SIZE 32

typedef struct stackImplementation
{
    void **data;
    int top;
    unsigned int stackSize;

} Stack;

void push(Stack* stack, void* ptr);
void* pop(Stack* stack);
void clearStack(Stack* stack); 
Stack* initStack();
void cleanupStack(Stack* stack);
void* peek(Stack* stack);


