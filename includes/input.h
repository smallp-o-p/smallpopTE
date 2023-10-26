#pragma once

#define CTRL_MACRO(k) ((k) & 0x1f)
struct dynamic_text_buffer{
    char* buf; 
    int len; 
};

void processKey(); 
void writeChar(char c); 
void moveCursor(int direction); 
void append2Buffer(struct dynamic_text_buffer* buf, char* str, int addedLen); 
void delChar(int op, struct rowOfText* row, int col);
void insertChar(int c, struct rowOfText* row, int col);


