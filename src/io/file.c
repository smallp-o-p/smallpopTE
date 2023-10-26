#define _POSIX_C_SOURCE >= 200809L
#define _GNU_SOURCE
#include "pch.h"
#include "input.h"
#include "output.h"
#include "error.h"
#include "file.h"

/**
 * Handle file IO and text rows 
 * 
*/

#define TABSTOP 8 
#define SPACE 32
int openFile(char *filename)
{
    free(E.filename);
    E.filename = strdup(filename);
    if (filename != NULL)
    {
        FILE *fp = fopen(filename, "r+w");
        if (!fp)
        {
            die("fopen");
        }
        size_t lineCap = 0;
        size_t lineLen;
        char *line = NULL;
        while ((lineLen = getline(&line, &lineCap, fp)) != -1)
        {
            while (lineLen > 0 && (line[lineLen - 1] == '\n' || line[lineLen - 1] == '\r'))
            {
                lineLen--;
            }
            addRow(line, lineLen);
        }
        free(line); 
        E.fp = fp; 
        return 0;
    }
}
int writeToFile(char* fileName){
    rewind(E.fp);
    for(int i = 0; i<E.numRowsofText; i++){
        strcat(E.textRows[i].text, "\n");
        fwrite(E.textRows[i].text, sizeof(char), E.textRows[i].len+1, E.fp);
    }
    int statusMessageSize = strlen(fileName) + 32; 
    char buf[statusMessageSize];
    snprintf(buf, statusMessageSize, "Saved file to: %s", fileName); 
    setStatusMessage(buf);
    return 0; 
}

void addRow(char *str, size_t len)
{
    E.textRows = realloc(E.textRows, sizeof(struct rowOfText) * (E.numRowsofText + 1));
    
    int at = E.numRowsofText;
    E.textRows[at].len = len;
    E.textRows[at].text = malloc(sizeof(char) * len + 1);
    memcpy(E.textRows[at].text, str, len);

    E.textRows[at].text[len] = '\0';
    E.textRows[at].renderSize = 0;
    E.textRows[at].render = NULL; 

    updateRow(&E.textRows[at]);

    E.numRowsofText++;
}
// copy chars from row->text into render
void updateRow(struct rowOfText* row){
    int tabs = 0;
    int idx = 0;
    for(int i = 0; i<row->len; i++){
        if(row->text[i] == '\t') {
            tabs++;
        } 
    }
    free(row->render);
    row->render = (char*) malloc(row->len + tabs*(TABSTOP-1) + 1);
    for(int i = 0; i<row->len; i++){
        if(row->text[i] == '\t'){
            row->render[idx++] = ' '; 
            while(idx % TABSTOP != 0){
                row->render[idx++] = ' '; 
            }
        }
        else if(row->text[i] == '\n'){ // don't draw newlines
            ;
        }
        else{
            row->render[idx++] = row->text[i]; 
        }
    }
    row->render[idx] = '\0';
    row->renderSize = idx; 
}

void moveRowText(struct rowOfText* from, struct rowOfText* to){
    to->text = realloc(to->text, from->len);
    memcpy(to->text, from->text, from->len);
    to->len = from->len; 
    memset(from->text, '\0', from->len);
}

void addAndShiftRowsDown(int fromRow){
  addRow("\0", E.textRows[E.numRowsofText].len); // add a new row at the bottom
  for(int i = E.numRowsofText-1; i>fromRow; i--){ // this should leave the line after fromRow that can be overwritten
    moveRowText(&E.textRows[i-1], &E.textRows[i]);
    updateRow(&E.textRows[i]);
  }  
}

void addRowAt(int row, char* whatToCopy, int len){
    addAndShiftRowsDown(row);

    struct rowOfText* newRow = &E.textRows[row];
    struct rowOfText* previousRow = &E.textRows[row-1];

    newRow->text = realloc(newRow->text, len);
    newRow->len = len; 

    memcpy(newRow->text, whatToCopy, len);

    int newBlockSize = (previousRow->len - len)+1;
    char* new = realloc(previousRow->text, newBlockSize);

    previousRow->text = new;
    previousRow->text[newBlockSize-1] = '\0';
    previousRow->len = newBlockSize;

    updateRow(newRow);
    updateRow(previousRow);
}

int rowCx2Rx(struct rowOfText* row, int cx){
    int rx = 0;
    for(int i = 0; i< cx; i++){
        if(row->text[i] == '\t'){
            rx+= (TABSTOP - 1) - (rx %TABSTOP);
        }
        rx++; 
    }
    return rx; 
}
