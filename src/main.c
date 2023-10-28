#include "pch.h"
#include "terminal.h"
#include "error.h"
#include "input.h"
#include "output.h"
#include "file.h"

void init_editor()
{
    E.cursor_x = 1;
    E.cursor_y = 0;
    E.render_x = 0;
    E.numRowsofText = 0;
    E.textRows = NULL;
    E.rowOffset = 0;
    E.filename = NULL;
    E.statusmsg[0] = '\0';
    E.statusmsg_time = 0;
    E.fp = NULL;
    if (getWindowSize(&E.rows, &E.cols) == -1)
    {
        die("getWindowSize");
    }
    E.rows -= 2; // leave two rows at the bottom empty for status bar and message
}

static void sig_handler(int sig)
{
    switch(sig)
    {
        case SIGSEGV:
            cleanup();
            exit(-1);
            break; 
        case SIGWINCH:
        {
            if (getWindowSize(&E.rows, &E.cols) == -1)
            {
                die("getWindowSize");
            }
        }
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
    init_terminal();
    init_editor();
    if (argc >= 2)
    {
        openFile(argv[1]);
    }
    setStatusMessage("Ctrl-Q to Quit!");
    while (1)
    {
        signal(SIGWINCH, sig_handler);
        signal(SIGSEGV, sig_handler);
        refreshScreen();
        processKey();
    }
    return 0;
}
