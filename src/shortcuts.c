#include "pch.h"
#include "shortcuts.h"
#include "row.h"
#include "input.h"

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
    pastTextRow *popped = (pastTextRow *)pop(E.undoStack);
    pastTextRow *toRedo = popped;

    if (popped == NULL)
    {
        return;
    }
    push(E.redoStack, (void*) toRedo);
    if (popped->action == ADD_CHAR)
    {
        pastTextRow *lookFurtherBack = (pastTextRow *)pop(E.undoStack);
        if (lookFurtherBack != NULL)
        {
            while (lookFurtherBack->action == ADD_CHAR && lookFurtherBack->rowNum == popped->rowNum)
            {
                if(popped != toRedo){ // don't free the most recent
                    free(popped);
                }
                popped = lookFurtherBack;
                lookFurtherBack = (pastTextRow *)pop(E.undoStack);
                if (lookFurtherBack == NULL)
                {
                    break;
                }
            }
            if(lookFurtherBack->action == ADD_SPACE){
                popped = (pastTextRow*) pop(E.undoStack); // removes the space
                free(lookFurtherBack);
            }
            else{
                free(popped);
                popped = lookFurtherBack; 
            }
            
        }
    }
    E.textRows[popped->rowNum].text = popped->text;
    E.textRows[popped->rowNum].len = popped->len;
    updateRow(&E.textRows[popped->rowNum]);
    if (c_y == popped->rowNum)
    {
        c_x = popped->len;
    }
    free(popped);
}

void redo() // this can only happen immediately after an undo
{
    if(E.redoStack->top == -1){
        return; 
    }

    pastTextRow *popped = (pastTextRow *)pop(E.redoStack);
    E.textRows[popped->rowNum].text = popped->text;
    E.textRows[popped->rowNum].len = popped->len;
    updateRow(&E.textRows[popped->rowNum]);
    if(c_y == popped->rowNum)
    {
        c_x = popped->len; 
    }
    free(popped);
}
