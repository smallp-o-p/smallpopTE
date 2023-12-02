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

void copy(copyBuffer* cBuf, uint32_t cy_upper, uint32_t cy_lower, uint32_t cx_leftmost, uint32_t cx_rightmost) // can only copy a continuous block
{
    if(cBuf->rows){
       for(int i = 0; i< E.cvBuf.numLines; i++)
       {
        freepastTextRows(&E.cvBuf.rows[i]);
       }
    };  

    cBuf->numLines = 0;

    cBuf->byteCount = 0; 
    
    int rangeToCopy, linesToCopy; 
    rangeToCopy = cx_rightmost - cx_leftmost;
    linesToCopy = cy_upper - cy_lower + 1;

    cBuf->rows = realloc(cBuf->rows, sizeof(pastTextRow) * linesToCopy);
    
    if(linesToCopy == 1)
    {
        pastTextRow row = {rangeToCopy, c_y};
        row.text = calloc(rangeToCopy, sizeof(char));
        row.text = strncpy(row.text, rowAt(c_y).text + cx_leftmost, rangeToCopy);
        cBuf->rows[0] = row;
        cBuf->numLines = 1;  
        cBuf->byteCount += rangeToCopy; 
    }
    else
    {
        // in this case rangeToCopy becomes useless
        int rowIndex = 0;

        for(int i = linesToCopy; i >= 0; i--)
        {
            pastTextRow row;
            uint32_t rowLen;
            char* copySrc; 
            
            if(i == linesToCopy) // first line to copy
            {
                rowLen = rowAt(c_y - linesToCopy).len - cx_leftmost;                
                copySrc = rowAt(c_y - linesToCopy).text + cx_leftmost;
            }
            else if(i == 0) // last line to copy
            {
                rowLen = (rowAt(c_y).len - cx_rightmost) + 1;
                copySrc = rowAt(c_y).text; 
            }
            else // everything in between
            {
                rowLen = rowAt(c_y - i).len;
                copySrc = rowAt(c_y - i).text;
            }

            row.len = rowLen;
            row.text = calloc(rowLen, sizeof(char));
            row.text = strncpy(row.text, copySrc, rowLen);
            row.rowNum = c_y - i; 

            cBuf->numLines++; 
            cBuf->rows[rowIndex++] = row;
            cBuf->byteCount += rowLen; 
        }
    }

}

void paste()
{
    if(!E.cvBuf.rows)
    {
        return; 
    }
    for(int i = 0; i<E.cvBuf.numLines; i++) // insert first line on current cursor line and add rows
    {
        if(i == 0 && c_y < E.numRowsofText)
        {
            rowAt(c_y).text = realloc(rowAt(c_y).text, rowAt(c_y).len + 1 + E.cvBuf.rows[i].len + 1);
            memmove(rowAt(c_y).text + c_x + E.cvBuf.rows[i].len, rowAt(c_y).text + c_x, rowAt(c_y).len - c_x + 1);  
            memcpy(rowAt(c_y).text + c_x, E.cvBuf.rows[i].text, E.cvBuf.rows[i].len);
            rowAt(c_y).len += E.cvBuf.rows[i].len; 
            updateRow(rowAtAddr(c_y)); 
        }
        else
        {
            addRow(c_y + i, E.cvBuf.rows[i].text, E.cvBuf.rows[i].len); 
        }
    }
    setStatusMessage(NORMAL, "Pasted %d bytes to lines %d to %d", E.cvBuf.byteCount, c_y + 1, c_y + E.cvBuf.numLines);
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
