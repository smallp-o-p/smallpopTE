#include "pch.h"
#include "terminal.h"
#include "error.h"
#include "input.h"
#include "output.h"

void init_editor(){
    if(getWindowSize(&E.rows, &E.cols) == -1 ){
        die("getWindowSize");
    }
}

int main(){
    char buffer[128];
    atexit(cleanup); 

    init_terminal();
    init_editor(); 

    while(1){
        refreshScreen();
        processKey(); 
    }
    return 0; 
}
