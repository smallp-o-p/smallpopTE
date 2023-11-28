#include "pch.h"
#include "row.h"
#include "input.h"
#include "terminal.h"
#include "output.h"
#include "shortcuts.h"
#include "row.h"

/* I am honestly so proud of doing this. */
void findString()
{

    int lastCursorPos_y = E.cursor_y;
    int lastCursorPos_x = E.cursor_x;

    char *needle = makePrompt("Search for: %s");
    if (needle == NULL)
    {
        return;
    }
    int count = 0;
    foundPair *whatWasFound = searchSubstr(needle, &count);
    if (count == 0 && whatWasFound == NULL)
    {
        setStatusMessage(CONCERNING, "No matches found.");
        return;
    }
    else
    {
        int index = 0;
        E.cursor_x = whatWasFound[index].col;
        E.cursor_y = whatWasFound[index].row;
        do
        {
            setStatusMessage(GOOD, "Found %d occurences: %d/%d ([>] next, [<] back, [esc] cancel)", count, (index + 1), count, count);
            refreshScreen();
            int a = getch();
            switch (a)
            {
            case (KEY_RIGHT):
                if ((index + 1) == count)
                {
                    index = -1;
                }
                index++;
                c_x = whatWasFound[index].col;
                c_y = whatWasFound[index].row;
                break;
            case (KEY_LEFT):
                if (index == 0)
                {
                    index = count;
                }
                index--;
                c_x = whatWasFound[index].col;
                c_y = whatWasFound[index].row;
                break;
            case ('\r'):
                setStatusMessage(NORMAL, "Exited.");
                return;
            case ('\x1b'):
                setStatusMessage(NORMAL, "Exited.");
                c_x = lastCursorPos_x;
                c_y = lastCursorPos_y;
                return;
            default:
                break;
            }
        } while (1);
    }
    free(needle);
    free(whatWasFound);
}

void removeNewLines(rememberStruct* previousState)
{
    for(uint32_t i = 0; i<previousState->numRows; i++)
    {
        if(i > 0)
        {
            removeRow(previousState->rowIndexes[i]);
        }
        else
        {
            updateRowInternalText(previousState->rowIndexes[i], previousState->rows[i]->text, previousState->rows[i]->len);
            updateRow(E.textRows[previousState->rowIndexes[i]]);
        }
    }
}

void restoreNewLines(rememberStruct* previousState)
{
    for(uint32_t i = 0; i<previousState->numRows; i++)
    {
        if(i > 0)
        {
            addRow(previousState->rowIndexes[i], previousState->rows[i]->text, previousState->rows[i]->len);
        }
        else
        {
            updateRowInternalText(previousState->rowIndexes[i], previousState->rows[i]->text, previousState->rows[i]->len);
            updateRow(E.textRows[previousState->rowIndexes[i]]);
        }
        
    }
}

void undoInsertionDeletion(rememberStruct* previousState)
{
    if(previousState->numRows != 1)
    {
        return;
    }

    updateRowInternalText(previousState->rows[0]->rowNum, previousState->rows[0]->text, previousState->rows[0]->len);
    updateRow(E.textRows[previousState->rows[0]->rowNum]);
}

void rememberRows(uint32_t *rowNumbers, uint32_t numRows, actionType lastAction)
{
    if (numRows == 0 || !rowNumbers)
    {
        return;
    }

    rememberStruct *iRemember = malloc(sizeof(rememberStruct));
    iRemember->rows = malloc(sizeof(pastTextRow*) * numRows);
    iRemember->numRows = numRows;
    iRemember->rowIndexes = malloc(sizeof(uint32_t) * numRows);

    switch (lastAction)
    {
    case (INSERT):
    case (DELETE):
        iRemember->howtoUndoMe = &undoInsertionDeletion; 
        break;
    case (NEWLINE):
        iRemember->howtoUndoMe = &removeNewLines;
        break;
    case (RM_NEWLINE):
        iRemember->howtoUndoMe = &restoreNewLines; 
        break;
    }
    iRemember->action = lastAction; 

    for (uint32_t i = 0; i < numRows; i++)
    {
        pastTextRow *ptr = malloc(sizeof(pastTextRow));
        ptr->rowNum = rowNumbers[i];
        tRow* row = rowAt(rowNumbers[i]);
        if (rowNumbers[i] >= E.numRowsofText)
        {
            ptr->len = 0;
            ptr->text = NULL;
        }
        else
        {
            ptr->len = row->len;
            ptr->text = calloc(row->len, sizeof(char));
            strncpy(ptr->text, row->text, row->len);
        }
        iRemember->rowIndexes[i] = rowNumbers[i];
        iRemember->rows[i] = ptr;
    }

    iRemember->timestamp = time(NULL);
    push(E.undoStack, (void *)iRemember);
}

void highlightKeywords(char *line)
{
}
