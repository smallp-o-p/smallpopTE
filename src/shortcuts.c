#include "pch.h"
#include "shortcuts.h"
#include "row.h"
#include "input.h"
#include "editorFeatures.h"
#include "output.h"

#define peekUndoStack() (rememberStruct *)peek(E.undoStack)
#define popUndoStack() (rememberStruct *)pop(E.undoStack)
#define pushUndoStack(k) push(E.undoStack, k)

#define peekRedoStack() (rememberStruct *)peek(E.redoStack)
#define popRedoStack() (rememberStruct *)pop(E.redoStack)
#define pushRedoStack(k) push(E.redoStack, k) 

int backspaceWord(int col, tRow *line)
{
    int toDelete = 0;
    char *pos = line->text + col;

    while (pos != line->text)
    {
        if ((*pos == ' ' || *pos == ':'))
        {
            pos--; // include the space
            break;
        }
        pos--;
        toDelete++;
    }

    char *dest = line->text + col - toDelete + 1;
    char *src = line->text + col + 1;
    int lenToMove = line->len - col - 1;

    if (lenToMove != -1)
    { // in case we're at end of line
        memmove(dest, src, lenToMove);
    }

    memset((src + lenToMove - toDelete), '\0', toDelete + 1);

    line->len -= (toDelete);
    updateRow(line);
    E.cursor_x = E.cursor_x - toDelete;
    E.dirty = true;

    return toDelete;
}

// returns the amount of characters affected
int deleteWord(int col, tRow *line)
{
    int toDelete = 0;
    char *pos = line->text + col;
    while (pos <= (line->text + line->len))
    {
        if ((*pos == ' ' || *pos == ':'))
        {
            pos++;
            toDelete++;
            break;
        }
        if(*pos == '\0')
        {
            break; 
        }
        pos++;
        toDelete++;
    }

    memmove(line->text + col, pos, line->len - col);
    line->len -= toDelete;
    updateRow(line);
    E.dirty = true;
    return toDelete;
}

void clrRightOfCursor(int col, tRow *line)
{
    char *pos = line->text + col;
    int toDelete = (int)((line->text + line->len) - pos);
    memset(line->text + col, '\0', toDelete);
    line->len = line->len - toDelete;
    updateRow(line);
    E.dirty = true;
}

void undo()
{
    rememberStruct* previousState;
    rememberStruct* currentState; 

    if(!(previousState = peekUndoStack()))
    {
        setStatusMessage(BAD, "Cannot undo further.");
        return; 
    }
    else
    {
        previousState = popUndoStack(); 
    }
    rememberRows(previousState->rowIndexes, previousState->numRows, CURRENT_STATE); 

    currentState = popUndoStack(); 
    for(uint32_t i = 0; i<previousState->numRows; i++)
    {
        if(previousState->rowIndexes[i] + 1 >= E.numRowsofText)
        {
            if(previousState->rows[i]->text)
            {
                addRow(previousState->rowIndexes[i], previousState->rows[i]->text, previousState->rows[i]->len);
                continue; 
            }
            else
            {
                removeRow(previousState->rowIndexes[i]); 
                continue;
            }
        }
        else
        {
            E.textRows[previousState->rows[i]->rowNum].text = previousState->rows[i]->text; 
            E.textRows[previousState->rows[i]->rowNum].len = previousState->rows[i]->len;
            updateRow(&E.textRows[previousState->rows[i]->rowNum]);
        }
    }
    c_y = previousState->rows[0]->rowNum; 
    c_x = previousState->rows[0]->len; 
    pushRedoStack(currentState);
}

void redo()
{
    rememberStruct* toRedo = popRedoStack();
    if(!toRedo)
    {
        setStatusMessage(BAD, "Cannot redo further.");
        return;
    }

    rememberRows(toRedo->rowIndexes, toRedo->numRows, REDO);

    for(uint32_t i = 0; i<toRedo->numRows; i++)
    {
        if(toRedo->rowIndexes[i] >= E.numRowsofText && toRedo->rows[i]->text)
        {
            if(toRedo->rows[i]->text)
            {
                addRow(toRedo->rowIndexes[i], toRedo->rows[i]->text, toRedo->rows[i]->len);
            }
            else
            {
                removeRow(toRedo->rowIndexes[i]); 
            }
        }
        else
        {
            E.textRows[toRedo->rows[i]->rowNum].text = toRedo->rows[i]->text;
            E.textRows[toRedo->rows[i]->rowNum].len = toRedo->rows[i]->len;
            updateRow(&E.textRows[toRedo->rows[i]->rowNum]);
        }
    }
    c_y = toRedo->rows[0]->rowNum; 
    c_x = toRedo->rows[0]->len; 
}
