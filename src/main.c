#include "pch.h"
#include "terminal.h"
#include "error.h"
#include "editorFeatures.h"
#include "input.h"
#include "output.h"
#include "file.h"

void init_editor()
{
    E.cursor_x = 0;
    E.cursor_y = 0;
    E.render_x = 0;
    E.numRowsofText = 0;
    E.textRows = NULL;
    E.rowOffset = 0;
    E.filename = NULL;
    memset(E.statusmsg, '\0', sizeof(E.statusmsg));
    E.undoStack = initStack();
    E.redoStack = initStack(); 
    E.statusmsg_time = 0;
    
    getmaxyx(stdscr, E.rows, E.cols);
}

static void sig_handler(int sig)
{
    switch(sig)
    {
        case SIGSEGV:
            cleanup();
            exit(-1);
            break; 
    }
}


int main(int argc, char **argv)
{

    if (argc > 2)
    {
        printf("Invalid no. of arguments, exiting\n");
        exit(0);
    }
    atexit(cleanup);
    initTerminal();
    init_editor();
    if (argc >= 2)
    {
        openFile(argv[1]);
    }
    setStatusMessage(NORMAL, "Ctrl-Q to Quit!");
    while (1)
    {
        signal(SIGSEGV, sig_handler);
        refreshScreen();
        processKey();
    }
    return 0;
}

