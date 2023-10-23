#include "terminal.h"
#include "pch.h"

void init_editor(struct termios* term){
    tcgetattr(STDIN_FILENO, term);
    struct termios raw = *term; 
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


