#define _DEFAULT_SOURCE

#include "pch.h"
#include "terminal.h"
int init_editor(struct termios* term){
    cfmakeraw(term); 
}
