#pragma once
void init_terminal();
void cleanup(); 
int readKey(); 
int handleEsc(char c);
int getWindowSize(int* rows, int* cols); 




