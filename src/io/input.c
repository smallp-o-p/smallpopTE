#include "pch.h"
#include "terminal.h"
#include "input.h"
#include "output.h"
#include "file.h"
#define START_X 0
#define TOP_FRAME 0
#define BACKSPACE_ASCII 127
void processKey()
{
  int c = readKey();
  switch (c)
  {
  case (CTRL_MACRO('q')):
    clearScreen();
    exit(0);
    break;
  case (CTRL_MACRO('c')):
    copyText(E.textRows[E.cursor_y].text[E.cursor_x]);
    break;
  case (CTRL_MACRO('v')):
    insertChar(E.cvBuf.copied, &E.textRows[E.cursor_y], E.cursor_x);
    break;
  case (CTRL_MACRO('s')):
    writeToFile(E.filename);
    break;
  case (CTRL_MACRO('m')): // enter;
    addRowAt(E.cursor_y + 1, (E.textRows[E.cursor_y]).text + E.cursor_x + 1, E.textRows[E.cursor_y].len - (E.cursor_x + 1));
    break;
  case (DELETE):
    delChar(c, &E.textRows[E.cursor_y], E.cursor_x);
    break;
  case (BACKSPACE_ASCII):
    delChar(BACKSPACE_ASCII, &E.textRows[E.cursor_y], E.cursor_x);
    break;
  case (PAGE_UP):
  case (PAGE_DOWN):
  {
    if (c == PAGE_UP)
    {
      E.cursor_y = E.rowOffset;
    }
    else if (c == PAGE_DOWN)
    {
      E.cursor_y = E.rowOffset + E.rows - 1;
      if (E.cursor_y > E.numRowsofText)
      {
        E.cursor_y = E.numRowsofText;
      }
    }
    int times = E.rows;
    while (times--)
    {
      moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
  }
  break;
  case (HOME):
  case (END):
  case (ARROW_UP):
  case (ARROW_DOWN):
  case (ARROW_LEFT):
  case (ARROW_RIGHT):
    moveCursor(c);
    break;
  default:
  {
    if (E.cursor_y == E.numRowsofText)
    {                        // we're at the bottom
      addRow((char *)&c, 1); // create a line
      E.cursor_x--;
    }
    else
    {
      insertChar(c, &E.textRows[E.cursor_y], E.cursor_x);
    }
    moveCursor(ARROW_RIGHT);
    break;
  }
  }
}

void moveCursor(int direction)
{
  struct rowOfText *currentRow = (E.cursor_y >= E.numRowsofText) ? NULL : &E.textRows[E.cursor_y];
  switch (direction)
  {
  case ARROW_LEFT:
    if (E.cursor_x != START_X)
    {
      E.cursor_x--;
    }
    else if (E.cursor_y > TOP_FRAME)
    {
      E.cursor_y--;
      E.cursor_x = E.textRows[E.cursor_y].len;
    }
    break;
  case ARROW_RIGHT:
    if (currentRow && E.cursor_x < currentRow->len)
    {
      E.cursor_x++;
    }
    else if (currentRow && E.cursor_x == currentRow->len)
    { //
      E.cursor_y++;
      E.cursor_x = START_X;
    }
    break;
  case ARROW_UP:
    if (E.cursor_y != TOP_FRAME)
    {
      E.cursor_y--;
    }
    break;
  case ARROW_DOWN:
    if (E.cursor_y < E.numRowsofText)
    {
      E.cursor_y++;
    }
    break;
  case HOME:
    E.cursor_x = START_X;
    break;
  case END:
    if (E.cursor_y < E.numRowsofText)
    {
      E.cursor_x = E.textRows[E.cursor_y].len;
    }
    break;
  }
  // if above line is longer than below, snap the cursor to the last letter of the below line
  currentRow = (E.cursor_y >= E.numRowsofText) ? NULL : &E.textRows[E.cursor_y];
  int rowLen = currentRow ? currentRow->len : 0;
  if (E.cursor_x > rowLen)
  {
    E.cursor_x = rowLen + 1;
  }
}

void append2Buffer(struct dynamic_text_buffer *buf, char *str, int addedLen)
{
  char *new = realloc(buf->buf, buf->len + addedLen);
  if (new == NULL)
  {
    return;
  }
  else
  {
    memcpy(&new[buf->len], str, addedLen);
    buf->buf = new;
    buf->len += addedLen;
  }
}

void delChar(int op, struct rowOfText *row, int col) // this seems to work fine with tabs :D
{
  if (col > row->len)
  {
    return;
  }
  switch (op) // no need to call realloc since it's kind of a waste of time to reduce a block by one byte only
  {
  case (DELETE): // delete character to the right of cursor
    // copy everything 2 positions to the right of the cursor and move it one to the left, overwriting the character we want to delete
    memmove(row->text + (col), row->text + col + 1, row->len - (col));
    updateRow(row); // reflect it on screen
    break;
  case (BACKSPACE_ASCII): // delete character to the left of cursor
  {
    if (col == 0)
    { // do nothing if we're at the start of a line
      break;
    }
    // copy everything on the cursor position and everything to the right, and move it one to the left
    memmove(row->text + (col - 1), row->text + (col), row->len - (col));
    memset(row->text + (row->len - 1), '\0', 1); // remove trailing character that doesn't get deleted when we memmove
    updateRow(row);
    moveCursor(ARROW_LEFT); // follow along :D
    break;
  }
  }
}
void insertChar(int c, struct rowOfText *row, int col)
{
  if (c > 126 || c < 32)
  {
    return;
  }
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
}
void copyText(int c)
{
  E.cvBuf.copied = c;
  E.cvBuf.len = 1;
}
