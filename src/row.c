#include "pch.h"
#include "input.h"
#include "row.h"

#define TABSTOP 8
#define SPACE 32
#define BACKSPACE_ASCII 127

void addRow(char *str, size_t len)
{
    E.textRows = realloc(E.textRows, sizeof(struct rowOfText) * (E.numRowsofText + 1));

    int at = E.numRowsofText;
    E.textRows[at].len = len;
    E.textRows[at].text = malloc(sizeof(char) * len + 1);
    memcpy(E.textRows[at].text, str, len);

    E.textRows[at].text[len] = '\0';
    E.textRows[at].renderSize = 0;
    E.textRows[at].render = NULL;

    updateRow(&E.textRows[at]);

    E.numRowsofText++;
    E.dirty++;
}
// copy chars from row->text into render
void updateRow(struct rowOfText *row)
{
    int tabs = 0;
    int idx = 0;
    for (int i = 0; i < row->len; i++)
    {
        if (row->text[i] == '\t')
        {
            tabs++;
        }
    }
    free(row->render);
    row->render = (char *)malloc(row->len + tabs * (TABSTOP - 1) + 1);
    for (int i = 0; i < row->len; i++)
    {
        if (row->text[i] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % TABSTOP != 0)
            {
                row->render[idx++] = ' ';
            }
        }
        else
        {
            row->render[idx++] = row->text[i];
        }
    }
    row->render[idx] = '\0';
    row->renderSize = idx;
}

void appendRowText(struct rowOfText* row, char* str, int len){
    row->text = realloc(row->text, row->len + len + 1);
    memcpy(row->text + row->len, str, len);
    row->len += len; 
    row->text[row->len] = '\0';
    updateRow(row);
    E.dirty++; 
}

void moveRowText(struct rowOfText *from, struct rowOfText *to)
{
    to->text = realloc(to->text, from->len);
    memcpy(to->text, from->text, from->len);
    to->len = from->len;
    memset(from->text, '\0', from->len);
}

void addAndShiftRowsDown(int fromRow)
{
    addRow("", E.textRows[E.numRowsofText - 1].len); // add a new row at the bottom
    // from the bottom up, move each row down one
    for (int i = E.numRowsofText - 1; i > fromRow; i--)
    { // this should leave the line after fromRow that can be overwritten
        moveRowText(&E.textRows[i - 1], &E.textRows[i]);
        updateRow(&E.textRows[i]);
    }
}

void addRowAt(int row, char *whatToCopy, int len)
{
    addAndShiftRowsDown(row);

    struct rowOfText *newRow = &E.textRows[row];
    struct rowOfText *previousRow = &E.textRows[row - 1];

    newRow->text = realloc(newRow->text, len);
    newRow->len = len;

    memcpy(newRow->text, whatToCopy, len);

    int newBlockSize = (previousRow->len - len) + 1;
    char *new = realloc(previousRow->text, newBlockSize);

    previousRow->text = new;
    previousRow->text[newBlockSize - 1] = '\0';
    previousRow->len = newBlockSize - 1;

    updateRow(newRow);
    updateRow(previousRow);
}

void removeRow(int row)
{ 
    if (row < 0 || row >= E.numRowsofText) return;
    free(E.textRows[row].render);
    free(E.textRows[row].text);
    memmove(&E.textRows[row], &E.textRows[row+1], sizeof(struct rowOfText)*(E.numRowsofText - row - 1));
    E.numRowsofText--;
    E.dirty++;
}

int rowCx2Rx(struct rowOfText *row, int cx)
{
    int rx = 0;
    for (int i = 0; i < cx; i++)
    {
        if (row->text[i] == '\t')
        {
            rx += (TABSTOP - 1) - (rx % TABSTOP);
        }
        rx++;
    }
    return rx;
}

void insertChar(int c)
{
    if (E.cursor_y == E.numRowsofText)
    {
        addRow("", 0);
    }
    insertCharInRow(c, &E.textRows[E.cursor_y], E.cursor_x);
    E.cursor_x++;
}

void insertCharInRow(int c, struct rowOfText *row, int col)
{
    if (col < 0 || col > row->len)
    {
        col = row->len;
    }
    row->text = realloc(row->text, row->len + 2);
    // make room for the new character, works even if we're at the end
    memmove(row->text + col + 1, row->text + col, row->len - col + 1);
    row->len++;
    row->text[col] = c;
    updateRow(row);
    E.dirty++;
}

void delChar(int col, int op)
{
    if(E.cursor_y == E.numRowsofText || (E.cursor_x == 0 && E.cursor_y == 0) || E.cursor_x >= E.textRows[E.cursor_y].len){
        return; 
    }
    struct rowOfText *tRow = &E.textRows[E.cursor_y];
    delCharInRow(op, tRow, col);
}

void delCharInRow(int op, struct rowOfText *row, int col) // this seems to work fine with tabs :D
{
    switch (op) // no need to call realloc since it's kind of a waste of time to reduce a block by one byte only
    {
    case (DELETE): // delete character to the right of cursor
        // copy everything 2 positions to the right of the cursor and move it one to the left, overwriting the character we want to delete
        memmove(row->text + (col), row->text + col + 1, row->len - (col));
        row->len--;
        updateRow(row); // reflect it on screen
        break;
    case (BACKSPACE): // delete character to the left of cursor
        if (col == 0)
        {
            E.cursor_x = E.textRows[E.cursor_y-1].len;
            appendRowText(row-1, row->text, row->len);
            removeRow(E.cursor_y);
            E.cursor_y--;
        }
        else{
            // copy everything on the cursor position and everything to the right, and move it one to the left
            memmove(row->text + (col - 1), row->text + (col), row->len - (col));
            memset(row->text + (row->len - 1), '\0', 1); // remove trailing character that doesn't get deleted when we memmove
            row->len--;
            updateRow(row);
            moveCursor(ARROW_LEFT); // follow along :D
            break;
        }

    }
    E.dirty++;
}
