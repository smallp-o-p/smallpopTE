#pragma once
void init_terminal();
void cleanup(); 
int readKey(); 
int handleEsc(char c);
int getWindowSize(int* rows, int* cols); 

/* Should only be keys that start with an escape (1b or 27)*/
enum specialKeys{
    ARROW_LEFT = 300,
    ARROW_RIGHT, 
    ARROW_UP,
    ARROW_DOWN, 
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    DELETE, 
};


