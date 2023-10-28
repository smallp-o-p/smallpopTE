#pragma once 
int openFile(char* filename);
int writeToFile(char* filename); 
char* rowsToCharBuffer(int* len, tRow* rows, int numRows); 
