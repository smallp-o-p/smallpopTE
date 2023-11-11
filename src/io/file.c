#define _POSIX_C_SOURCE >= 200809L
#define _GNU_SOURCE
#include "pch.h"
#include "input.h"
#include "output.h"
#include "error.h"
#include "file.h"
#include "row.h"
/**
 * Handle file IO
 *
 */
int openFile(char *filename)
{
    free(E.filename);
    E.filename = strdup(filename);
    if (filename != NULL)
    {
        FILE *fp = fopen(filename, "r");
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
            addRow(E.numRowsofText, line, lineLen);
        }
        free(line);
        fclose(fp);
        E.dirty = 0;
        return 0;
    }
}

int writeToFile(char *fileName)
{
    FILE* fp = NULL; 
    if(fileName == NULL){
        fileName = makePrompt("Save as ([esc] to cancel): %s");
        if(fileName == NULL){
            return 0;
        } 
        fp = fopen(fileName, "w+");
    }
    else{
        fp = fopen(fileName, "r+");
    }
    if(!fp){
        setStatusMessage(BAD, "Failed to open file %s, error returned: %s", fileName, strerror(errno));
        return -1; 
    }

    int length; 
    char* toWrite = rowsToCharBuffer(&length, E.textRows, E.numRowsofText);

    ftruncate(fileno(fp), length);
    fwrite(toWrite, sizeof(char), length, fp);
    fclose(fp);
    setStatusMessage(GOOD, "Wrote %d %s to: %s", E.numRowsofText, "lines", fileName);
    E.dirty = 0;
    E.filename = fileName; 
    free(toWrite);
    return 0;
}


char* rowsToCharBuffer(int* len, tRow* rows, int numRows){

    int totalLen = 0;
    for(int i = 0; i<numRows; i++){
        totalLen += (rows+i)->len+1;
    }

    *len = totalLen;
    char* buffer = malloc(totalLen); 
    char* ptr = buffer; 

    memset(buffer, '\0', sizeof(totalLen));

    for(int j = 0; j< numRows; j++){
        memcpy(ptr, (rows+j)->text, (rows+j)->len);
        ptr+= (rows+j)->len;
        (*ptr++) = '\n';
    }
    return buffer; 
}
