#define _DEFAULT_SOURCE
#include "terminal.h"
#include "pch.h"
#include "error.h"

struct terminalConfig E; 

void init_terminal(){

    if(tcgetattr(STDIN_FILENO, &E.terminal) == -1){
        die("tcgetattr");
    };
    struct termios raw = E.terminal; 
    cfmakeraw(&raw);     
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)== -1){
        die("tcsetattr");
    };
}

void cleanup(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.terminal) == -1){
        die("tcsetattr");
    }
}

int getWindowSize(int* rows, int* cols){
    struct winsize ws; 
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        return -1;
    }
    else{
        *cols = ws.ws_col;
        *rows = ws.ws_row; 
        return 0; 
    }

}

char readKey(){
    int nread; 
    char c = '\0'; 
    if((nread = read(STDIN_FILENO, &c, sizeof(char))) != 1){
        if(nread == -1 && nread != EAGAIN){
            die("read");
        }
    }
    return c; 
}

