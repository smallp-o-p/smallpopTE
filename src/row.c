#include "pch.h"
#include "input.h"
#include "row.h"
#include "output.h"

#define TABSTOP 8
#define SPACE 32
#define BACKSPACE_ASCII 127

void addRow(int at, char *str, size_t len)
{
    if (at < 0 || at > E.numRowsofText)
    {
        return;
    }
    // make space for a new row of text
    // move everything below the row pointed to by at down 1
    E.textRows = realloc(E.textRows, sizeof(struct rowOfText) * (E.numRowsofText + 1));
    memmove(E.textRows + at + 1, E.textRows + at, sizeof(struct rowOfText) * (E.numRowsofText - at));

    E.textRows[at].len = len;
    E.textRows[at].text = malloc(sizeof(char) * len + 1);
    memcpy(E.textRows[at].text, str, len);

    E.textRows[at].text[len] = '\0';
    E.textRows[at].renderSize = 0;
    E.textRows[at].render = NULL;

    updateRow(&E.textRows[at]);
    E.numRowsofText++;
    E.dirty = true;
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

void appendRowText(struct rowOfText *row, char *str, int len)
{
    row->text = realloc(row->text, row->len + len + 1);
    memcpy(row->text + row->len, str, len);
    row->len += len;
    row->text[row->len] = '\0';
    updateRow(row);
    E.dirty = true;
}

void moveRowText(struct rowOfText *from, struct rowOfText *to)
{
    to->text = realloc(to->text, from->len);
    memcpy(to->text, from->text, from->len);
    to->len = from->len;
    memset(from->text, '\0', from->len);
}

void removeRow(int row)
{
    if (row < 0 || row >= E.numRowsofText)
        return;
    free(E.textRows[row].render);
    free(E.textRows[row].text);
    memmove(&E.textRows[row], &E.textRows[row + 1], sizeof(struct rowOfText) * (E.numRowsofText - row - 1));
    E.numRowsofText--;
    E.dirty = true;
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
        addRow(E.numRowsofText, "", 0);
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
    E.dirty = true;
}

void delChar(int col, int op)
{
    if ((E.cursor_y == E.numRowsofText && op == KEY_BACKSPACE) || (E.cursor_x == 0 && E.cursor_y == 0))
    {
        return;
    }
    struct rowOfText *tRow = &E.textRows[E.cursor_y];
    delCharInRow(op, tRow, col);
}

void delCharInRow(int op, struct rowOfText *row, int col) // this seems to work fine with tabs :D
{
    switch (op) // no need to call realloc since it's kind of a waste of time to reduce a block by one byte only
    {
    case (KEY_DC): // delete character underneath the cursor
        if (col == row->len)
        {
            appendRowText(row, (row + 1)->text, (row + 1)->len);
            removeRow(E.cursor_y + 1);
            break;
        }
        else
        {
            // copy everything 2 positions to the right of the cursor and move it one to the left, overwriting the character we want to delete
            memmove(row->text + (col), row->text + col + 1, row->len - (col));
            row->len--;
            updateRow(row); // reflect it on screen
            break;
        }

    case (KEY_BACKSPACE): // delete character to the left of cursor
        if (col == 0)
        {
            E.cursor_x = E.textRows[E.cursor_y - 1].len;
            appendRowText(row - 1, row->text, row->len);
            removeRow(E.cursor_y);
            E.cursor_y--;
            break;
        }
        else
        {
            // copy everything on the cursor position and everything to the right, and move it one to the left
            memmove(row->text + (col - 1), row->text + (col), row->len - (col));
            memset(row->text + (row->len - 1), '\0', 1); // remove trailing character that doesn't get deleted when we memmove
            row->len--;
            updateRow(row);
            moveCursor(KEY_LEFT); // follow along :D
            break;
        }
    }
    E.dirty = true;
}

foundPair *searchSubstr(char *needle, int *countToUpdate)
{
    setStatusMessage(NORMAL, "Searching for: %s...", needle);
    refreshScreen();

    int foundPairLen = 5;
    foundPair *pairs = malloc(sizeof(foundPair) * foundPairLen);
    int needleLen = strlen(needle);
    int count = 0;

    for (int i = 0; i < E.numRowsofText; i++)
    {
        char *haystack = E.textRows[i].text;
        char *temp = haystack;
        while ((temp = strstr(temp, needle)))
        {
            foundPair found = {i, (int)(temp - haystack)};
            if (count == foundPairLen)
            {
                pairs = realloc(pairs, sizeof(foundPair) * (foundPairLen *= 2));
            }
            pairs[count++] = found;
            temp += needleLen;
        }
    }
    if (count == 0)
    {
        *countToUpdate = 0;
        free(pairs);
        return NULL;
    }
    else
    {
        *countToUpdate = count;
        return pairs;
    }
}

void updateRowInternalText(uint32_t rowNum, char *text, uint32_t len) // i got tired of doing this all the time
{
    free(E.textRows[rowNum].text); 
    E.textRows[rowNum].text = text;
    E.textRows[rowNum].len = len;
}
