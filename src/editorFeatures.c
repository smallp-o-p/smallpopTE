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
                E.cursor_x = whatWasFound[index].col;
                E.cursor_y = whatWasFound[index].row;
                break;
            case (KEY_LEFT):
                if (index == 0)
                {
                    index = count;
                }
                index--;
                E.cursor_x = whatWasFound[index].col;
                E.cursor_y = whatWasFound[index].row;
                break;
            case ('\r'):
                setStatusMessage(NORMAL, "Exited.");
                return;
            case ('\x1b'):
                setStatusMessage(NORMAL, "Exited.");
                E.cursor_x = lastCursorPos_x;
                E.cursor_y = lastCursorPos_y;
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

    if(row == NULL){
        remember->text = NULL;
        remember->len = 0; 
    }
    else{
        remember->text = malloc(sizeof(char) * row->len);
        strncpy(remember->text, row->text, row->len);
        remember->len = row->len;
    } 

    if(lastAction == ADD_CHAR || lastAction == ADD_SPACE){
        remember->at = c_x; 
    }

    remember->rowNum = c_y;
    remember->timestamp = time(NULL); 
    remember->action = lastAction; 
    push(E.undoStack, (void*) remember); 
}

void highlightKeywords(char *line)
{
}
