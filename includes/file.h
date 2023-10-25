#pragma once 
int openFile(char* filename);
void addRow(char* str, size_t len);

void updateRow(struct rowOfText* row);
int rowCx2Rx(struct rowOfText* row, int cx);
