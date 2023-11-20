#include "pch.h"
#include "row.h"
#include "input.h"
#include "terminal.h"
#include "output.h"

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
    foundPair* whatWasFound = searchSubstr(needle, &count);
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

void rememberRows(uint32_t* rowNumbers, uint32_t numRows, actionType lastAction)
{   
    if(numRows == 0 || !rowNumbers)
    {
        return; 
    }

    rememberStruct* iRemember = malloc(sizeof(rememberStruct)); 
    iRemember->rows = malloc(sizeof(pastTextRow) * numRows);
    iRemember->numRows = numRows; 
    iRemember->rowIndexes = malloc(sizeof(uint32_t) * numRows); 

    for(uint32_t i = 0; i<numRows; i++)
    {
        pastTextRow* ptr = malloc(sizeof(pastTextRow));
        ptr->rowNum = rowNumbers[i];
        if(rowNumbers[i] >= E.numRowsofText)
        {
            ptr->len = 0; 
            ptr->text = NULL;
        }
        else
        {
            ptr->len = E.textRows[rowNumbers[i]].len; 
            ptr->text = calloc(E.textRows[rowNumbers[i]].len, sizeof(char));
            strncpy(ptr->text, E.textRows[rowNumbers[i]].text, E.textRows[rowNumbers[i]].len); 
        }
        iRemember->rowIndexes[i] = rowNumbers[i];
        iRemember->rows[i] = ptr; 
    }

    iRemember->timestamp = time(NULL);
    iRemember->action = lastAction; 
    push(E.undoStack, (void*) iRemember);
}

void highlightKeywords(char *line)
{
}
