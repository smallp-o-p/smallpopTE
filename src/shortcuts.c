#include "pch.h"
#include "shortcuts.h"
#include "row.h"
#include "input.h"
#include "editorFeatures.h"

#define peekUndoStack() (pastTextRow *)peek(E.undoStack)
#define popUndoStack() (pastTextRow *)pop(E.undoStack)

#define pushUndoStack(k) push(E.undoStack, k)
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
    pastTextRow* previousState;
    pastTextRow* currentState; 
    pastTextRow* revertTo; 
    if(!(previousState = popUndoStack()))
    {
        return; 
    }

    switch(previousState->action)
    {
        case(INITIAL_STATE):
        {
            pushUndoStack(previousState);
            revertTo = previousState;
            break;  
        }
        case(REDO):
        {
            
        }
        case(ADD_SPACE):
        {
            revertTo = previousState; 
            break;
        }
    }

    rememberTextRow(&E.textRows[c_y], CURRENT_STATE); 
    currentState = popUndoStack(); 

    if(revertTo)
    {
        E.textRows[revertTo->rowNum].text = revertTo->text;
        E.textRows[revertTo->rowNum].len = revertTo->len;
        c_x = revertTo->at; 
        updateRow(&E.textRows[revertTo->rowNum]);
    }

    pushRedoStack(currentState);
}

void redo()
{
    pastTextRow* undidState;

    if(!(undidState = pop(E.redoStack)))
    {
        return; 
    }
    rememberTextRow(&E.textRows[undidState->rowNum], REDO);

    E.textRows[undidState->rowNum].text = undidState->text;
    E.textRows[undidState->rowNum].len = undidState->len;
    if(c_y == undidState->rowNum)
    {
        c_x = undidState->len;
    }
    
    updateRow(&E.textRows[undidState->rowNum]);
    free(undidState);
}
