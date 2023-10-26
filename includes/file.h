#pragma once 
int openFile(char* filename);
void addRow(char* str, size_t len);
void addRowAt(int row, char* whatToCopy, int len); 
void addAndShiftRowsDown(int fromRow);
void updateRow(struct rowOfText* row);
void moveRowText(struct rowOfText* from, struct rowOfText* to);
int rowCx2Rx(struct rowOfText* row, int cx);
int writeToFile(char* filename); 
