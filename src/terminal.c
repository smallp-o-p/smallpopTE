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
    keypad(stdscr, true); 
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
    cleanupStack(E.rememberedText);
    endwin(); 
}

int handleEsc(char c)
{
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
    {
        return '\x1b';
    }
    else if(seq[0] == 'd'){
        return CTRL_DELETE; 
    }
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
    {
        return '\x1b';
    }

    if (seq[0] == '[')
    {
        if (seq[1] >= '0' && seq[1] <= '9')
        {
            if (read(STDIN_FILENO, &seq[2], 1) != 1)
            {
                return '\x1b';
            }
            if (seq[2] == '~')
            {
                switch (seq[1])
                {
                case '1':
                    return KEY_HOME;
                case '3':
                    return DELETE;
                case '4':
                    return KEY_END;
                case '5':
                    return PAGE_UP;
                case '6':
                    return PAGE_DOWN;
                case '7':
                    return KEY_HOME;
                case '8':
                    return KEY_END;
                }
            }
        }
        else
        {
            switch (seq[1])
            {
            case 'A':
                return KEY_UP;
            case 'B':
                return KEY_DOWN;
            case 'C':
                return KEY_RIGHT;
            case 'D':
                return KEY_LEFT;
            case 'H':
                return KEY_HOME;
            case 'F':
                return KEY_END;
            }
        }
    }
    else if (seq[0] == 'O')
    {
        switch (seq[1])
        {
        case 'H':
            return KEY_HOME;
        case 'F':
            return KEY_END;
        }
    }

    return '\x1b';
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

int readKey()
{
    int nread;
    char c = '\0'; // you have to set it or else it will hold some garbage value
    if ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && nread != EAGAIN)
        {
            die("read");
        }
    }
    if (c == '\x1b')
    {
        return handleEsc(c);
    }
    else
    {
        return c;
    }
}
