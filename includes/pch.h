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
#include <ncurses.h> 

/*
    Includes that should be present everywhere
*/

typedef enum statusMessageType{
    BAD = 1, 
    CONCERNING,
    NORMAL,
    GOOD,
} msgType;

typedef enum syntaxTokenType{
    KEYWORD,
    IDENTIFER,
    CONSTANT,
    STRING_LITERAL,
    OPERATOR,
    PUNCTUATOR
} tokens; 
struct copyBuffer{ // can copy only one character at a time
    int len;
    char copied; 
};
typedef struct rowOfText{
    int len; // length of string, length of array
    char* text; // raw text 
    int renderSize; // length of render string
    char* render; // text converted to work with our text editor
    char* highLighting; 
} tRow;  
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
    msgType msgtype; 
    time_t statusmsg_time; 
} extern E; 

#define c_y E.cursor_y

#define c_x E.cursor_x

/* Should only be keys that start with an escape (1b or 27)*/
enum specialKeys{ 
    PAGE_UP,
    PAGE_DOWN,
    DELETE, 
    CTRL_DELETE = 520,
    CTRL_BACKSPACE = 8
};



