#include "pch.h"
#include "terminal.h"
#include "input.h"
#include "output.h"
#include "file.h"
#include "row.h"
#include "editorFeatures.h"

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
  case (CTRL_MACRO('x')):
    writeToFile(NULL);
    break;
  case ('\r'): // enter;
    insertNewLine();
    break;
  case CTRL_MACRO('h'):
    break;
  case CTRL_MACRO('w'): // this works in the vscode terminal but not on the mint terminal since ctrl+backspace is ^W over there 
    backspaceWord(c_x, &E.textRows[c_y]);
    break;
  case CTRL_MACRO('f'):
    findString();
    break; 
  case (DELETE):
    delChar(c_x, DELETE);
    break;
  case CTRL_DELETE:
    deleteWord(c_x, &E.textRows[c_y]);
    break; 
  case BACKSPACE:
    delChar(c_x, BACKSPACE);
    break;
  case (PAGE_UP):
  case (PAGE_DOWN):
  {
    if (c == PAGE_UP)
    {
      c_y = E.rowOffset;
    }
    else if (c == PAGE_DOWN)
    {
      c_y = E.rowOffset + E.rows - 1;
      if (c_y > E.numRowsofText)
      {
        c_y = E.numRowsofText;
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
    if(!iscntrl(c)){
      insertChar(c);
    } 
    break;
  }
  quit = 2;
}

void exitConfirm()
{
  setStatusMessage(NORMAL, "Save modified buffer? [y/n], cancel with [esc]");
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
      setStatusMessage(NORMAL, "Cancelled");
      refreshScreen();
      return;
    default:
      break;
    }
  } while (1);
}

char *makePrompt(char *promptFormat)
{
  int bufferSize = 128;
  char *userBuffer = malloc(bufferSize * sizeof(char));
  int userBuffLen = 0;

  memset(userBuffer, '\0', bufferSize);
  do
  {
    int a = readKey();
    setStatusMessage(NORMAL, promptFormat, userBuffer);
    refreshScreen();

    switch (a)
    {
    case 0:
      break;
    case ('\r'):
    {
      if (userBuffLen != 0)
      {
        return userBuffer;
      }
      break;
    }
    case ('\x1b'):
      setStatusMessage(CONCERNING, "Action cancelled.");
      refreshScreen();
      free(userBuffer);
      return NULL;
    case (BACKSPACE):
      if (userBuffLen != 0)
      {
        userBuffer[--userBuffLen] = '\0';
      }
    default:
      if (!iscntrl(a) && a < 127)
      {
        if (userBuffLen == bufferSize)
        {
          userBuffer = realloc(userBuffer, (bufferSize *= 2));
        }
        userBuffer[userBuffLen++] = a;
        userBuffer[userBuffLen] = '\0';
      }
    }
  } while (1);
}

void moveCursor(int direction)
{
  struct rowOfText *currentRow = (c_y >= E.numRowsofText) ? NULL : &E.textRows[c_y];
  switch (direction)
  {
  case ARROW_LEFT:
    if (c_x != START_X)
    {
      c_x--;
    }
    else if (c_y > TOP_FRAME)
    {
      c_y--;
      c_x = E.textRows[c_y].len;
    }
    break;
  case ARROW_RIGHT:
    if (currentRow && c_x < currentRow->len)
    {
      c_x++;
    }
    else if (currentRow && c_x == currentRow->len)
    { //
      c_y++;
      c_x = START_X;
    }
    break;
  case ARROW_UP:
    if (c_y != TOP_FRAME)
    {
      c_y--;
    }
    break;
  case ARROW_DOWN:
    if (c_y < E.numRowsofText)
    {
      c_y++;
    }
    break;
  case HOME:
    c_x = START_X;
    break;
  case END:
    if (c_y < E.numRowsofText)
    {
      c_x = E.textRows[c_y].len;
    }
    break;
  }
  // if above line is longer than below, snap the cursor to the last letter of the below line
  currentRow = (c_y >= E.numRowsofText) ? NULL : &E.textRows[c_y];
  int rowLen = currentRow ? currentRow->len : 0;
  if (c_x > rowLen)
  {
    c_x = rowLen + 1;
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
  if (c_x == 0)
  {
    addRow(c_y, "", 0);
  }
  else
  {
    struct rowOfText *row = E.textRows + c_y;
    addRow(c_y + 1, row->text + c_x, row->len - c_x);
    row = E.textRows + c_y; // reset the pointer in case realloc moves the block somewhere
    row->len = c_x;
    row->text[row->len] = '\0';
    updateRow(row);
  }
  c_y++;
  c_x = 0;
}

void copyText(int c)
{
  E.cvBuf.copied = c;
  E.cvBuf.len = 1;
}
