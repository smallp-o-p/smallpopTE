#include "pch.h"
#include "output.h"
#include "terminal.h"

void refreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4); 
    write(STDOUT_FILENO, "\x1b[H", 3);
    drawRows('-');
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void clearScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4); 
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void drawRows(char c){
    char formatted[4];
    snprintf(formatted, 4, "%c\r\n", c);
    for(int y = 0; y<E.rows; y++){
        if(y < E.rows -1 ){
            write(STDOUT_FILENO, formatted, 3);
        }
        else{
            write(STDOUT_FILENO, &c, 1); 
        }
    }
}
