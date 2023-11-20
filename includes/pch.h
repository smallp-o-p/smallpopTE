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
#include "stack.h"

/*
    Includes that should be present everywhere
*/

typedef enum statusMessageType{
    BAD = 252, 
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

typedef struct rowOfText{
    int len; // length of string, length of array
    char* text; // raw text 
    int renderSize; // length of render string
    char* render; // text converted to work with our text editor
    char* highLighting; 
} tRow;  

typedef struct copyText{
    uint32_t len;
    char* text;
} copyBuffer; 

typedef enum actions{
    INITIAL_STATE, 
    INSERT,
    REMOVE,
    NEWLINE,
    RM_NEWLINE, 
    CURRENT_STATE, 
}actionType;

typedef struct softDeletedRowOfText{
    int len;
    int rowNum; 
    int at; 
    char* text;
} pastTextRow; 

typedef struct pastTextRows{
    pastTextRow** rows; 
    uint32_t* rowIndexes; 
    uint32_t numRows;   
    time_t timestamp;
    actionType action; 
} rememberStruct; 

struct terminalConfig{
    int cursor_x, cursor_y; 
    int render_x; 
    int cx_lower, cx_upper; 
    int rows;
    int rowOffset;
    int cols; 
    bool dirty; 
    int colOffset; 
    int numRowsofText; 
    struct rowOfText* textRows; 
    struct termios terminal; 
    char* filename; 
    char statusmsg[80];
    msgType msgtype; 
    time_t statusmsg_time; 
    Stack* undoStack; 
    Stack* redoStack; 
    copyBuffer cvBuf; 
} extern E; 

#define c_y E.cursor_y

#define c_x E.cursor_x


enum specialKeys{ 
    PAGE_UP,
    PAGE_DOWN,
    DELETE, 
    CTRL_DELETE = 520,
    CTRL_SHIFT_DELETE, 
    CTRL_BACKSPACE = 8,
};



