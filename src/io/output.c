#define _BSD_SOURCE
#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include "pch.h"
#include "output.h"
#include "terminal.h"
#include "input.h"
#include "row.h"

#define CURSOR_POSITION_FORMAT "\x1b[%d;%dH"

void refreshScreen()
{
    scrollHandler();
    struct dynamic_text_buffer dbuf = {NULL, 0};

    append2Buffer(&dbuf, "\x1b[?25l", 6);
    append2Buffer(&dbuf, "\x1b[H", 3);
    drawRows('-', &dbuf);
    drawStatusBar(&dbuf);
    drawStatusMessage(&dbuf);

    char buf[32]; // cursor position why is it (y, x) ????
    snprintf(buf, sizeof(buf), CURSOR_POSITION_FORMAT, (E.cursor_y - E.rowOffset) + 1, (E.render_x - E.colOffset) + 1);

    append2Buffer(&dbuf, buf, strlen(buf)); // write cursor position to the output buffer
    append2Buffer(&dbuf, "\x1b[?25h", 6);

    write(STDOUT_FILENO, dbuf.buf, dbuf.len);
    free((&dbuf)->buf);
}

void clearScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void drawRows(char c, struct dynamic_text_buffer *buf)
{
    for (int y = 0; y < E.rows; y++)
    {
        int filerow = y + E.rowOffset;
        if (filerow >= E.numRowsofText)
        {
            append2Buffer(buf, &c, 1);
        }
        else
        {
            int len = E.textRows[filerow].renderSize - E.colOffset;
            if (len < 0)
                len = 0;
            if (len > E.cols)
                len = E.cols;
            append2Buffer(buf, &E.textRows[filerow].render[E.colOffset], len);
        }
        append2Buffer(buf, "\x1b[K", 3);
        append2Buffer(buf, "\r\n", 2);
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

void drawStatusBar(struct dynamic_text_buffer *buf)
{
    append2Buffer(buf, "\x1b[1;7m", 6);
    char statusText[80], rstatus[80];
    int len = snprintf(statusText, sizeof(statusText),
                       "%.20s - %d lines %s",
                       E.filename ? E.filename : "[Blank File]", E.numRowsofText, E.dirty ? "(modified)" : "");
    if (len > E.cols)
    {
        len = E.cols;
    }
    append2Buffer(buf, statusText, len);

    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", E.cursor_y + 1, E.numRowsofText + 1);

    while (len < E.cols)
    {
        if (E.cols - len == rlen)
        {
            append2Buffer(buf, rstatus, rlen);
            break;
        }
        else
        {
            append2Buffer(buf, " ", 1);
            len++;
        }
    }
    append2Buffer(buf, "\x1b[m", 3);
    append2Buffer(buf, "\r\n", 2);
}

void setStatusMessage(const char *stat, ...)
{
    va_list ap;
    va_start(ap, stat);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), stat, ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}

void drawStatusMessage(struct dynamic_text_buffer *buf)
{
    append2Buffer(buf, "\x1b[k", 3);
    int len = strlen(E.statusmsg);
    if (len > E.cols)
    {
        len = E.cols;
    }
    if (len && time(NULL) - E.statusmsg_time < 5)
    {
        append2Buffer(buf, E.statusmsg, len);
    }
}
