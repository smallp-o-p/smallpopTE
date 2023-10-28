#define _DEFAULT_SOURCE
#include "terminal.h"
#include "pch.h"
#include "error.h"

struct terminalConfig E;

void init_terminal()
{
    if (tcgetattr(STDIN_FILENO, &E.terminal) == -1)
    {
        die("tcgetattr");
    };
    struct termios raw = E.terminal;
    cfmakeraw(&raw);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1; 
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        die("tcsetattr");
    };
}

void cleanup()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.terminal) == -1)
    {
        die("tcsetattr");
    }
}

int handleEsc(char c)
{
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
    {
        return '\x1b';
    }
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
    {
        return '\x1b';
    }

    if (seq[0] == '[')
    {
        if (seq[1] >= '0' && seq[1] <= '9')
        {
            if (read(STDIN_FILENO, &seq[2], 1) != 1)
            {
                return '\x1b';
            }
            if (seq[2] == '~')
            {
                switch (seq[1])
                {
                case '1':
                    return HOME;
                case '3':
                    return DELETE;
                case '4':
                    return END;
                case '5':
                    return PAGE_UP;
                case '6':
                    return PAGE_DOWN;
                case '7':
                    return HOME;
                case '8':
                    return END;
                }
            }
        }
        else
        {
            switch (seq[1])
            {
            case 'A':
                return ARROW_UP;
            case 'B':
                return ARROW_DOWN;
            case 'C':
                return ARROW_RIGHT;
            case 'D':
                return ARROW_LEFT;
            case 'H':
                return HOME;
            case 'F':
                return END;
            }
        }
    }
    else if (seq[0] == 'O')
    {
        switch (seq[1])
        {
        case 'H':
            return HOME;
        case 'F':
            return END;
        }
    }
    return '\x1b';
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

int readKey()
{
    int nread;
    char c = '\0'; // you have to set it or else it will hold some garbage value
    if ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && nread != EAGAIN)
        {
            die("read");
        }
    }
    if (c == '\x1b')
    {
        return handleEsc(c);
    }
    else
    {
        return c;
    }
}
