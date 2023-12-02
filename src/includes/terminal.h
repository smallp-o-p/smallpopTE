#pragma once
void initTerminal();
void cleanup(); 
void initColors(); 
int readKey(); 
int handleEsc(char c);
int getWindowSize(int* rows, int* cols); 




