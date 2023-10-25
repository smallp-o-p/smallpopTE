#pragma once
void init_terminal();
void cleanup(); 
int readKey(); 
int handleEsc(char c);
int getWindowSize(int* rows, int* cols); 

enum specialKeys{
    ARROW_LEFT = 100,
    ARROW_RIGHT, 
    ARROW_UP,
    ARROW_DOWN, 
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    DELETE,
    BACKSPACE 
};


