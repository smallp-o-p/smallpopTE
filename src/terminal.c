#define _DEFAULT_SOURCE
#include "terminal.h"
#include "pch.h"
#include "error.h"

struct terminalConfig E;

void initTerminal()
{
    if (tcgetattr(STDIN_FILENO, &E.terminal) == -1)
    {
        die("tcgetattr");
    };
    initscr(); 
    raw(); 
    noecho(); 
    keypad(stdscr, TRUE); 
    if(has_colors()){
        initColors(); 
    }
}

void initColors(){
    start_color(); 
    assume_default_colors(-1, -1);
    short DEFAULT_COLOR = -1; 
    init_pair(BAD, COLOR_RED, DEFAULT_COLOR);
    init_pair(GOOD, COLOR_GREEN, DEFAULT_COLOR);
    init_pair(CONCERNING, COLOR_YELLOW, DEFAULT_COLOR);
}

void cleanup()
{
    cleanupStack(E.undoStack);
    cleanupStack(E.redoStack);
    endwin(); 
}
