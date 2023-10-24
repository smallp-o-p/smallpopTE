#pragma once 
#include <termios.h>
#include <string.h>
#include <memory.h>
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/ioctl.h>
struct terminalConfig{
    int rows;
    int cols; 
    struct termios terminal;
} extern E; 


/*
    Includes that should be present everywhere

*/
