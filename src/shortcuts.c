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

void copy(tRow* line, uint32_t cx_upper, uint32_t cx_lower)
{
    int rangeToCopy; 
    if(cx_upper == cx_lower)
    {
        rangeToCopy = 1; 
    }
    else
    {
        rangeToCopy = cx_upper - cx_lower; 
    }
    E.cvBuf.text = realloc(E.cvBuf.text, sizeof(char) * (rangeToCopy));
    E.cvBuf.len = rangeToCopy; 
    strncpy(E.cvBuf.text, line->text + (rangeToCopy == 1 ? c_x : cx_lower), (rangeToCopy));
}

void paste(tRow* line, uint32_t cx)
{
    line->text = realloc(line->text, sizeof(char) * (E.cvBuf.len + line->len));
    memmove(line->text + cx + E.cvBuf.len - 1, line->text + cx, line->len - cx);
    line->len = E.cvBuf.len + line->len;
    memcpy(line->text + cx, E.cvBuf.text, E.cvBuf.len);
    updateRow(line);
}

actionType getInverseAction(actionType action)
{   
    switch(action)
    {
        case(DELETE):
            return INSERT;
        case(INSERT):
            return DELETE;
        case(NEWLINE):
            return RM_NEWLINE;
        case(RM_NEWLINE):
            return NEWLINE;
        default:
            return CURRENT_STATE;
    }
}


/**
 * 
 * 
 * Cases: Add word : 1 line, delete word: 1 line, add newline : 2 lines, remove newline: 2 lines
 *  
 * 
*/
void undo()
{
    rememberStruct* previousState = popUndoStack();

    if(!previousState)
    {
        setStatusMessage(BAD, "Cannot undo further.");
        return; 
    }

    rememberRows(previousState->rowIndexes, previousState->numRows, getInverseAction(previousState->action)); 

    pushRedoStack(popUndoStack());

    previousState->howtoUndoMe(previousState); 

    c_y = previousState->rows[0]->rowNum; 
    c_x = previousState->rows[0]->len; 
}

void redo()
{
    rememberStruct* toRedo = popRedoStack();
    if(!toRedo)
    {
        setStatusMessage(BAD, "Cannot redo further.");
        return;
    }

    rememberRows(toRedo->rowIndexes, toRedo->numRows, getInverseAction(toRedo->action));
    
    toRedo->howtoUndoMe(toRedo); 
    c_y = toRedo->rows[0]->rowNum; 
    c_x = toRedo->rows[0]->len; 
}
