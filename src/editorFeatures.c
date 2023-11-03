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
            int a = readKey();
            switch (a)
            {
            case (ARROW_RIGHT):
                if ((index + 1) == count)
                {
                    index = -1;
                }
                index++;
                E.cursor_x = whatWasFound[index].col;
                E.cursor_y = whatWasFound[index].row;
                break;
            case (ARROW_LEFT):
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

void backspaceWord(int col, tRow* line)
{
    int toDelete = 0;
    char* pos = line->text + col;

    while (pos != line->text)
    {
        if ((*pos == ' '|| *pos == ':'))
        {
            pos--; // include the space 
            break;
        }
        pos--;
        toDelete++; 
    }

    char* dest = line->text + col - toDelete + 1; 
    char* src = line->text + col + 1; 
    int lenToMove = line->len - col - 1; 

    if(lenToMove != -1){ // in case we're at end of line 
        memmove(dest, src, lenToMove); 
    }

    memset((src + lenToMove - toDelete), '\0', toDelete+1);

    line->len -= (toDelete); 
    updateRow(line);
    E.cursor_x = E.cursor_x - toDelete; 
}

void deleteWord(int col, tRow* line)
{
    int toDelete = 0;
    char* pos = line->text + col;

    while(pos <= (line->text + line->len))
    {
        if((*pos == ' ' || *pos == ':')){
            pos++;
            break;
        }
        pos++;
        toDelete++;
    }

    memmove(line->text+col, pos, line->len - col); 
    line->len -= toDelete; 
    updateRow(line);
}

void highlightKeywords(char *line)
{
}
