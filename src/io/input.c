#include "pch.h"
#include "terminal.h"
#include "input.h"
#include "output.h"
#include "file.h"
#include "row.h"
#include "editorFeatures.h"
#include "shortcuts.h"

#define START_X 0
#define TOP_FRAME 0

#define KEY_SPACE 32

void processKey()
{
  int c = getch();
  switch (c)
  {
  case 0:
    return;
  case (KEY_RESIZE):
    getmaxyx(stdscr, E.rows, E.cols);
    refreshScreen();
    break;
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
  case (10): // enter;
  {
    uint32_t temp[2] = {c_y, c_y + 1}; // can only affect two lines at a time
    rememberRows(temp, 2, NEWLINE);
    insertNewLine();
  }
  break;
  case CTRL_MACRO('f'):
    findString();
    break;
  case CTRL_MACRO('z'):
    undo();
    break;
  case (CTRL_MACRO('y')):
    redo();
    break;
  case (CTRL_MACRO('c')):
    copy(E.cy_upper, E.cy_lower, E.cx_leftmost, E.cx_rightmost);
    break;
  case (CTRL_MACRO('v')):
    paste(rowAt(c_y), c_x);
    break;
  case CTRL_BACKSPACE:
    backspaceWord(c_x, rowAt(c_y));
    break;
  case (KEY_DC):
    delChar(c_x, DELETE);
    break;
  case CTRL_DELETE:
    rememberRows(&c_y, 1, DELETE);
    deleteWord(c_x, rowAt(c_y));
    break;
  case CTRL_SHIFT_DELETE:
    clrRightOfCursor(c_x, rowAt(c_y));
    break;
  case KEY_BACKSPACE:
    delChar(c_x, KEY_BACKSPACE);
    break;
  case (KEY_PPAGE):
  case (KEY_NPAGE):
  {
    if (c == KEY_PPAGE)
    {
      c_y = E.rowOffset;
    }
    else if (c == KEY_NPAGE)
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
      moveCursor(c == KEY_PPAGE ? KEY_UP : KEY_DOWN);
    }
  }
  break;
  case CTRL_MACRO('l'): // select line
  {
    E.cx_leftmost = 0; 
    E.cx_rightmost = E.textRows[c_y]->len;
    c_x = E.textRows[c_y]->len;
    break;
  }
  case '\x1b':
    break;
  case (KEY_SLEFT):
    moveCursor(KEY_LEFT);
    E.cx_leftmost = c_x;
    break; 
  case (KEY_SRIGHT):
    if(E.cursor_x < rowAt(c_y)->len)
    {
      moveCursor(KEY_RIGHT);
    }
    if (E.cx_rightmost < E.textRows[c_y]->len - 1)
    {
      E.cx_rightmost = c_x;
    }
    break;
  case (KEY_UP):
  case (KEY_DOWN):
  case (KEY_HOME):
  case (KEY_END):
  case (KEY_LEFT):
  case (KEY_RIGHT):
    moveCursor(c);
    break;
  default:
    if (!iscntrl(c))
    {
      if (c == ' ')
      {
        rememberRows(&c_y, 1, INSERT);
      }
      insertChar(c);
      clearStack(E.redoStack, &freepastTextRows);
    }
    break;
  }
  if (c != CTRL_MACRO('l') && c != KEY_SRIGHT && c != KEY_SLEFT)
  {
    E.cx_leftmost = c_x;
    E.cx_rightmost = c_x;
  }
  if(E.cy_upper == E.cy_lower)
  {
    E.cy_upper = c_y;
    E.cy_lower = c_y; 
  }
}

void exitConfirm()
{
  setStatusMessage(NORMAL, "Save modified buffer? [y/n], cancel with [esc]");
  refreshScreen();
  int a;
  do
  {
    a = getch();
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
    setStatusMessage(NORMAL, promptFormat, userBuffer);
    refreshScreen();

    int a = getch();

    switch (a)
    {
    case 0:
      break;
    case (10):
    {
      if (userBuffLen != 0)
      {
        return userBuffer;
      }
      break;
    }
    case ('\x1b'):
      setStatusMessage(CONCERNING, "Action cancelled.");
      free(userBuffer);
      return NULL;
    case (KEY_BACKSPACE):
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
  struct rowOfText *currentRow = (c_y >= E.numRowsofText) ? NULL : E.textRows[c_y];
  switch (direction)
  {
  case KEY_LEFT:
    if (c_x != START_X)
    {
      c_x--;
    }
    else if (c_y > TOP_FRAME)
    {
      c_y--;
      c_x = E.textRows[c_y]->len;
    }
    break;
  case KEY_RIGHT:
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
  case KEY_UP:
    if (c_y != TOP_FRAME)
    {
      c_y--;
    }
    break;
  case KEY_DOWN:
    if (c_y < E.numRowsofText)
    {
      c_y++;
    }
    break;
  case KEY_HOME:
    c_x = START_X;
    break;
  case KEY_END:
    if (c_y < E.numRowsofText)
    {
      c_x = E.textRows[c_y]->len;
    }
    break;
  }
  // if above line is longer than below, snap the cursor to the last letter of the below line
  currentRow = (c_y >= E.numRowsofText) ? NULL : E.textRows[c_y];
  int rowLen = currentRow ? currentRow->len : 0;
  if (c_x > rowLen)
  {
    c_x = rowLen;
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
    struct rowOfText *row = E.textRows[c_y];
    addRow(c_y + 1, row->text + c_x, row->len - c_x);
    row = E.textRows[c_y] ; // reset the pointer in case realloc moves the block somewhere
    row->len = c_x;
    row->text[row->len] = '\0';
    updateRow(row);
  }
  c_y++;
  c_x = 0;
}
