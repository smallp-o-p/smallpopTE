#include "pch.h"
#include "terminal.h"
#include "input.h"
#include "output.h"
#include "file.h"
#include "row.h"

#define START_X 0
#define TOP_FRAME 0

void processKey()
{
  static int quit = 2;
  int c = readKey();
  switch (c)
  {
    case 0:
      return; 
  case (CTRL_MACRO('q')):
    if (E.dirty)
    {
      exitConfirm();
    }
    else
    {
      clearScreen();
      exit(0);
    }
    break;
  case (CTRL_MACRO('s')):
    writeToFile(E.filename);
    break;
  case(CTRL_MACRO('x')):
    writeToFile(NULL);
    break; 
  case ('\r'): // enter;
    insertNewLine();
    break;
  case CTRL_MACRO('h'):
    break;
  case (DELETE):
    delChar(E.cursor_x, DELETE);
    break;
  case BACKSPACE:
    delChar(E.cursor_x, BACKSPACE);
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
  case CTRL_MACRO('l'):
  case '\x1b':
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
    insertChar(c);
    break;
  }
  quit = 2;
}

void exitConfirm()
{
  setStatusMessage("Save modified buffer? [y/n], cancel with [esc]");
  refreshScreen();
  int a;
  do
  {
    a = readKey();
    switch (a)
    {
    case ('y'):
      writeToFile(E.filename);
    case ('n'):
      clearScreen();
      exit(0);
    case ('\x1b'):
      setStatusMessage("Cancelled");
      refreshScreen();
      return;
    default:
      break; 
    }
  } while (1);
}

char *makePrompt(char *prompt)
{
  int bufferSize = 128;
  char *userBuffer = malloc(bufferSize * sizeof(char));
  int userBuffLen = 0;

  memset(userBuffer, '\0', bufferSize);
  do
  {
    int a = readKey();
    setStatusMessage(prompt, userBuffer);
    refreshScreen();

    switch (a)
    {
    case 0:
      break;
    case ('\r'):
    {
      if (userBuffLen == 0)
      {
        setStatusMessage("No input provided. Retrying.");
        refreshScreen();
      }
      else
      {
        return userBuffer;
      }
      break;
    }
    case ('\x1b'):
      setStatusMessage("Action cancelled.");
      refreshScreen();
      free(userBuffer);
      return NULL;
    case (BACKSPACE):
      if (userBuffLen != 0)
      {
        userBuffer[--userBuffLen] = '\0';
      }
      break; 
    default:
    {
      if (!iscntrl(a) && a < 127)
      {
        if (userBuffLen == bufferSize)
        {
          userBuffer = realloc(userBuffer, (bufferSize*2));
        }
        userBuffer[userBuffLen++] = a;
        userBuffer[userBuffLen] = '\0';
      }
    }
    }
  } while (1);
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

void insertNewLine()
{
  if (E.cursor_x == 0)
  {
    addRow(E.cursor_y, "", 0);
  }
  else
  {
    struct rowOfText *row = E.textRows + E.cursor_y;
    addRow(E.cursor_y + 1, row->text + E.cursor_x, row->len - E.cursor_x);
    row = E.textRows + E.cursor_y; // reset the pointer in case realloc moves the block somewhere
    row->len = E.cursor_x;
    row->text[row->len] = '\0';
    updateRow(row);
  }
  E.cursor_y++;
  E.cursor_x = 0;
}

void copyText(int c)
{
  E.cvBuf.copied = c;
  E.cvBuf.len = 1;
}
