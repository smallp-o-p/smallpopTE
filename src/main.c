#include "pch.h"
#include "terminal.h"

struct termios terminal;

void cleanup(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal); 
}

int main(){
    atexit(cleanup); 

    init_editor(&terminal);

    char c; 
    while(read(STDIN_FILENO, &c, 1) && c != 'q'){
        printf("%c\n", c); 
    }
    return 0; 
}
