#include "pch.h"
#include "shortcuts.h"
#include "row.h"
#include "input.h"
#include "editorFeatures.h"
#include "output.h"

#define peekUndoStack() (pastTextRow *)peek(E.undoStack)
#define popUndoStack() (pastTextRow *)pop(E.undoStack)
#define pushUndoStack(k) push(E.undoStack, k)

#define peekRedoStack() (pastTextRow *)peek(E.redoStack)
#define popRedoStack() (pastTextRow *)pop(E.redoStack)
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
    pastTextRow* previousState;
    pastTextRow* currentState; 

    if(!(previousState = popUndoStack()))
    {
        setStatusMessage(BAD, "Cannot undo further.");
        return; 
    }

    if(previousState->action == INITIAL_STATE)
    {
        pushUndoStack(previousState);
    }
    else if(previousState->action == NEWLINE)
    {
        rememberTextRow(&E.textRows[previousState->rowNum], CURRENT_STATE);
        currentState = popUndoStack(); 
        currentState->rowNum = previousState->rowNum; 

        rememberTextRow(&E.textRows[previousState->rowNum + 1], NEWLINE);
        pastTextRow* tempRow = popUndoStack(); 
        tempRow->rowNum = previousState->rowNum+1; 

        pushRedoStack(tempRow);

        removeRow((previousState->rowNum)+1);
    }
    else if(previousState->action == REDO_FROM_NEWLINE)
    {
        setStatusMessage(BAD, "Unsupported action: Undoing a redo that made a new line.");
        return; 
    }
    else
    {
        rememberTextRow(&E.textRows[previousState->rowNum], CURRENT_STATE); 
        currentState = popUndoStack(); 
    }

    if(previousState)
    {
        E.textRows[previousState->rowNum].text = previousState->text;
        E.textRows[previousState->rowNum].len = previousState->len;
        if(previousState->rowNum == c_y)
        {
            c_x = previousState->at; 
        }
        else
        {
            c_y = previousState->rowNum;
            c_x = previousState->at; 
        }
        updateRow(&E.textRows[previousState->rowNum]);
    }
    
    pushRedoStack(currentState);
}

void redo()
{
    pastTextRow* undidState;
    pastTextRow* nextLine; 
    if(!(undidState = pop(E.redoStack)))
    {
        setStatusMessage(BAD, "Cannot redo further.");
        return; 
    }
    if((peekRedoStack())){
        if((peekRedoStack())->action == NEWLINE)
        {
            nextLine = popRedoStack(); 
            addRow(nextLine->rowNum, nextLine->text, nextLine->len);
        }
    }

    rememberTextRow(&E.textRows[undidState->rowNum], REDO);

    if(nextLine && nextLine->action == NEWLINE)
    {
        nextLine->action = REDO_FROM_NEWLINE; 
        pushUndoStack(nextLine);
    }

    E.textRows[undidState->rowNum].text = undidState->text;
    E.textRows[undidState->rowNum].len = undidState->len;
    if(c_y == undidState->rowNum)
    {
        c_x = undidState->len;
    }
    
    updateRow(&E.textRows[undidState->rowNum]);
    if(nextLine && nextLine->action != NEWLINE)
    {
        free(nextLine);
    }
}
