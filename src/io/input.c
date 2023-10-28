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
  case (CTRL_MACRO('x')):
    writeToFile(NULL);
    break;
  case ('\r'): // enter;
    insertNewLine();
    break;
  case CTRL_MACRO('h'):
    break;
  case (DELETE):
    delChar(c_x, DELETE);
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
    insertChar(c);
    break;
  }
  quit = 2;
}

char *makePrompt(char *promptFormat)
{
  size_t bufferSize = 64;
  size_t bufferStrLen = 0;
  char *buffer = malloc(bufferSize * sizeof(char));

  memset(buffer, '\0', bufferSize);
  do
  {
    setStatusMessage(promptFormat, buffer);
    refreshScreen();

    int a = readKey();

    switch (a)
    {
    case 0:
      break;
    case '\r':
      if (bufferSize == 0)
      {
        setStatusMessage("Empty input. Trying again.");
        refreshScreen();
        break;
      }
      else
      {
        return buffer;
      }
    case ('\x1b'):
      setStatusMessage("Action cancelled.");
      free(buffer);
      return NULL;
      break;
    case (BACKSPACE):
      if (bufferStrLen != 0)
      {
        buffer[--bufferStrLen] = '\0';
      }
      break;
    default:
      if (iscntrl(a) && a < 128)
      {
        if (bufferSize == bufferStrLen)
        {
          buffer = realloc(buffer, (bufferSize *= 2));
        }
        buffer[bufferStrLen++] = a;
        buffer[bufferStrLen] = '\0';
        break;
      }
    }
  } while (1);
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

char *saveConfirm()
{
  char *fileName;
  size_t max_fileNameLen = 64;
  int fIndex = 0;
  fileName = malloc(max_fileNameLen * sizeof(char));
  memset(fileName, '\0', max_fileNameLen);
  do
  {
    setStatusMessage("Save as: %s", fileName);
    refreshScreen();

    int a = readKey();

    switch (a)
    {
    case 0:
      break;
    case ('\r'):
    {
      if (fIndex == 0)
      {
        setStatusMessage("No file name provided.");
        refreshScreen();
      }
      else
      {
        return fileName;
      }
      break;
    }
    case ('\x1b'):
      setStatusMessage("File save cancelled.");
      refreshScreen();
      free(fileName);
      return NULL;
    case (BACKSPACE):
      if (fIndex != 0)
      {
        fileName[--fIndex] = '\0';
      }
    default:
    {
      if (!iscntrl(a) && a < 127)
      {
        if (fIndex == max_fileNameLen)
        {
          setStatusMessage("File name can be at most 64 characters.");
          refreshScreen;
          break;
        }
        else
        {
          fileName[fIndex++] = a;
          fileName[fIndex] = '\0';
        }
      }
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
