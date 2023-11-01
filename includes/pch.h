#pragma once 
#include <termios.h>
#include <string.h>
#include <memory.h>
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>

/*
    Includes that should be present everywhere
*/
struct copyBuffer{ // can copy only one character at a time
    int len;
    char copied; 
};
struct rowOfText{
    int len; // length of string, length of array
    char* text; // raw text 
    int renderSize; // length of render string
    char* render; // text converted to work with our text editor
    char* highLighting; 
}; 
typedef struct rowOfText tRow; 
struct terminalConfig{
    int cursor_x, cursor_y; 
    int render_x; 
    int rows;
    int rowOffset;
    int cols; 
    int dirty; 
    int colOffset; 
    int numRowsofText; 
    struct rowOfText* textRows; 
    struct termios terminal;
    struct copyBuffer cvBuf; 
    char* filename; 
    char statusmsg[80];
    time_t statusmsg_time; 
} extern E; 

#define c_y E.cursor_y

#define c_x E.cursor_x

/* Should only be keys that start with an escape (1b or 27)*/
enum specialKeys{
    BACKSPACE = 127, 
    ARROW_LEFT = 300,
    ARROW_RIGHT, 
    ARROW_UP,
    ARROW_DOWN, 
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    DELETE, 
    CTRL_DELETE
};



