#include "pch.h"
#include "shortcuts.h"
#include "row.h"
#include "input.h"

#define peekUndoStack() (pastTextRow *)peek(E.undoStack)
#define popUndoStack() (pastTextRow *)pop(E.undoStack)

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
    if (peek(E.undoStack) == NULL)
    {
        return;
    }

    pastTextRow *mostRecentState = popUndoStack();
    pastTextRow *revertTo; 
    pastTextRow *rightptr;
    char *text;
    int len;
    int rowNum;
    switch (mostRecentState->action)
    {
    case (ADD_CHAR):
    {
        pastTextRow *rightptr = mostRecentState; 
        pastTextRow *leftptr = popUndoStack();  
        while(leftptr) // we're using the fact that the letters in words of a germanic language go from left to write to our advantage
        {
            /*
                Undo "batches" of successive inputted characters, stop in front of spaces or when the popped row is in a different row from the most recent state
                To detect a batch we basically check if the position of the character inserted at the rightptr, which should be the more recent state is 
                to the immediate right of leftptr, which should be the state immediately before. 
                Consider moving this into book-keeping so we don't have to store so many row states  
            */

            if((rightptr->at - leftptr->at) != 1 || leftptr->action == ADD_SPACE || leftptr->rowNum != mostRecentState->rowNum)  
            {
                if(leftptr->action == ADD_SPACE)
                {
                    push(E.undoStack, leftptr); 
                } 
                if(leftptr->rowNum != mostRecentState->rowNum)
                {
                    leftptr = rightptr; 
                }
                break; 
            }
            if(rightptr != mostRecentState)
            {
                free(rightptr); 
            }
            rightptr = leftptr;
            leftptr = popUndoStack(); 
        }
        revertTo = leftptr; 
        break;
    } 
    case (ADD_SPACE): // we cannot guarantee that the previous action was on the same line 
    {
        revertTo = malloc(sizeof(pastTextRow));

        revertTo->rowNum = mostRecentState->rowNum; 

        char* temp = malloc(sizeof(mostRecentState->text) * mostRecentState->len);

        revertTo->text = strncpy(temp, mostRecentState->text, mostRecentState->len);

        memmove(revertTo->text + mostRecentState->at - 1, revertTo->text + mostRecentState->at, mostRecentState->len - (mostRecentState->at - 1));
        
        revertTo->len = mostRecentState->len-1; 

        revertTo->at = revertTo->len; 
        break; 
    }
    case(REDO):
    {

    }
    }

    if(revertTo != NULL)
    {
        E.textRows[revertTo->rowNum].text = revertTo->text;
        E.textRows[revertTo->rowNum].len = revertTo->len; 
        updateRow(&E.textRows[revertTo->rowNum]);
        if(c_y == revertTo->rowNum)
        {
            c_x = revertTo->at;
        }
    }
    push(E.redoStack, mostRecentState); 
}

void redo() // this can only happen immediately after an undo
{
    if (peek(E.redoStack) == NULL)
    {
        return;
    }

    pastTextRow *popped = (pastTextRow *)pop(E.redoStack);
    E.textRows[popped->rowNum].text = popped->text;
    E.textRows[popped->rowNum].len = popped->len;
    updateRow(&E.textRows[popped->rowNum]);
    if (c_y == popped->rowNum)
    {
        c_x = popped->len;
    }
    popped->action = REDO;
    push(E.undoStack, popped);
}
