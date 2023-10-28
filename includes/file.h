#pragma once 
int openFile(char* filename);
int writeToFile(char* filename); 
char* rowsToCharBuffer(struct rowOfText* rows, int numRows);
