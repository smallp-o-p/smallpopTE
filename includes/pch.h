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

/*
    Includes that should be present everywhere
*/

struct rowOfText{
    int len; // length of string, length of array
    char* text; // raw text 
    int renderSize; // length of render string
    char* render; // text converted to work with our text editor
}; 
struct terminalConfig{
    int cursor_x, cursor_y; 
    int render_x; 
    int rows;
    int rowOffset;
    int cols; 
    int colOffset; 
    int numRowsofText; 
    struct rowOfText* textRows; 
    struct termios terminal;
    char* filename; 
    char statusmsg[80];
    time_t statusmsg_time; 
} extern E; 



