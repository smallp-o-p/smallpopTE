#define _BSD_SOURCE
#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include "pch.h"
#include "output.h"
#include "terminal.h"
#include "input.h"
#include "row.h"

#define CURSOR_POSITION_FORMAT "\x1b[%d;%dH"

#define CLEAR_LINE "\x1b[K"

void refreshScreen()
{
    scrollHandler();
    erase(); 
    drawRows('-');
    drawStatusBar();
    drawStatusMessage();
    move(E.cursor_y, E.cursor_x);
    refresh(); 
}

void clearScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void drawRows(char c)
{
    for (int y = 0; y < E.rows-2; y++)
    {
        int filerow = y + E.rowOffset;

        if (filerow >= E.numRowsofText)
        {
            printw("%c\n", c);
        }
        else
        {
            int len = E.textRows[filerow].renderSize - E.colOffset;
            if (len < 0)
            {
                len = 0;
            }
                
            if (len > E.cols)
            {
                len = E.cols;
            }
            char* line = &E.textRows[filerow].render[E.colOffset];
            if(filerow == c_y && E.cx_leftmost != E.cx_rightmost)
            {
                addnstr(line, E.cx_leftmost);
                attron(A_REVERSE);
                addnstr(line + E.cx_leftmost, (E.cx_rightmost - E.cx_leftmost) + 1);
                attroff(A_REVERSE);
                printw("%s\n", line + E.cx_rightmost + 1);
            }
            else
            {
                printw("%s\n", line);
            }
        }
    }
}

void scrollHandler()
{
    E.render_x = 0;
    if (E.cursor_y < E.numRowsofText)
    {
        E.render_x = rowCx2Rx(&E.textRows[E.cursor_y], E.cursor_x);
    }
    if (E.cursor_y < E.rowOffset) // are we inside the reference frame
    {
        E.rowOffset = E.cursor_y;
    }
    if (E.cursor_y >= E.rowOffset + E.rows) // are we past the bottom of the reference frame
    {
        E.rowOffset = E.cursor_y - E.rows + 1;
    }
    if (E.render_x < E.colOffset)
    {
        E.colOffset = E.render_x;
    }
    if (E.render_x >= E.colOffset + E.cols)
    {
        E.colOffset = E.render_x - E.cols + 1;
    }
}

void drawStatusBar()
{
    attron(A_BOLD | A_REVERSE); 
    char statusText[80], rstatus[80];
    int len = snprintf(statusText, sizeof(statusText),
                       "%.20s - %d lines %s",
                       E.filename ? E.filename : "[Blank File]", E.numRowsofText, E.dirty ? "(modified)" : "");
    if (len > E.cols)
    {
        len = E.cols;
    }
    addstr(statusText); 

    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", E.cursor_y + 1, E.numRowsofText + 1);
    while (len < E.cols)
    {
        if (E.cols - len == rlen)
        {
            printw("%s\n", rstatus); 
            break;
        }
        else
        {
            addch(' '); 
            len++;
        }
    }
    attroff(A_BOLD | A_REVERSE); 
}

void setStatusMessage(msgType type, const char *stat,  ...)
{
    memset(E.statusmsg, '\0', sizeof(E.statusmsg));
    char temp[128]; 
    memset(temp, '\0', sizeof(temp)); 
    
    E.msgtype = type; 
    strncpy(temp, stat, sizeof(temp)); 

    va_list ap;
    va_start(ap, stat);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), temp, ap);
    va_end(ap);

    E.statusmsg_time = time(NULL);
}

void drawStatusMessage()
{
    int len = strlen(E.statusmsg);
    if (len > E.cols)
    {
        len = E.cols;
    }
    time_t currentTime = time(NULL);
    if (len && (currentTime - E.statusmsg_time < 5))
    {
        if(E.msgtype != NORMAL){
            attron(COLOR_PAIR(E.msgtype));
            printw("%s", E.statusmsg); 
            attroff(COLOR_PAIR(E.msgtype)); 
        }
        else{
            addstr(E.statusmsg); 
        }
    }
}
