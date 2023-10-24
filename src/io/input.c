#include "terminal.h"
#include "input.h"
#include "pch.h"
#include "output.h"

void processKey(){
    char c = readKey();
    switch(c){
        case(CTRL_MACRO('q')):
            clearScreen(); 
            exit(0);
            break; 
    }
}
