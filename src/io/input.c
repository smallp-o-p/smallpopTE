#include "pch.h"
#include "terminal.h"
#include "input.h"
#include "output.h"
#define START_X 0 
#define TOP_FRAME 0 
void processKey()
{
  int c = readKey();
  switch (c)
  {
  case (CTRL_MACRO('q')):
    clearScreen();
    exit(0);
    break;
  case DELETE:
    delChar(c, E.cursor_y, E.cursor_x);
    break;
  case (CTRL_MACRO('?')):
    delChar(BACKSPACE, E.cursor_y, E.cursor_x);
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
    while(times--)
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
    moveCursor(ARROW_RIGHT);
    break;
  }
}

void moveCursor(int direction)
{
  struct rowOfText* currentRow = (E.cursor_y >= E.numRowsofText) ? NULL : &E.textRows[E.cursor_y];
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
    if(E.cursor_y < E.numRowsofText){
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

void delChar(int op, int row, int col)
{
  if (col > E.textRows[row].len || row >= E.numRowsofText)
  {
    return;
  }
  switch (op)
  {
  case (DELETE): // delete character in front of cursor
    memmove(E.textRows[row].text + (col - 1), E.textRows[row].text + col, E.textRows[row].len - (col - 1));
    break;
  case (BACKSPACE):
    memmove(E.textRows[row].text + (col - 2), E.textRows[row].text + (col - 1), E.textRows[row].len - (col - 1));
    break;
  }
}
