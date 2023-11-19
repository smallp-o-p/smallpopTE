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

void rememberTextRow(tRow* row, actionType lastAction){
    
    pastTextRow* remember = malloc(sizeof(pastTextRow)); 

    if(!row){ // null means initial state
        if(!(&E.textRows[c_y])) // empty file
        {
            remember->text = NULL;
            remember->len = 0; 
        }
        else{ // non-empty file
            remember->text = malloc(sizeof(char) * E.textRows[c_y].len);
            strncpy(remember->text, E.textRows[c_y].text, E.textRows[c_y].len);
            remember->len = E.textRows[c_y].len;
        } 
    }
    else{
        remember->text = malloc(sizeof(char) * row->len);
        strncpy(remember->text, row->text, row->len);
        remember->len = row->len;
    } 

    remember->at = c_x; 
    remember->rowNum = c_y;
    remember->timestamp = time(NULL); 
    remember->action = lastAction; 

    push(E.undoStack, (void*) remember); 
}

void highlightKeywords(char *line)
{
}
