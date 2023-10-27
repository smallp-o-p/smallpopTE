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
    FILE* fp; 
    if(fileName == NULL){
        fileName = makePrompt("Save as: %s");
        if(fileName == NULL){
            return 0; 
        } 
        fp = fopen(fileName, "w+");
    }
    else{
        fp = fopen(fileName, "r+");
    }
    if(!fp){
        setStatusMessage("Failed to open file %s, error returned: %s", fileName, strerror(errno));
        return -1; 
    }

    char* toWrite = rowsToCharBuffer(E.textRows, E.numRowsofText);
    fwrite(toWrite, sizeof(char), strlen(toWrite), fp);

    fclose(fp); 
    
    E.dirty = 0;
    E.filename = fileName;
    setStatusMessage("Saved to file %s", fileName);
    free(toWrite);

    return 0;
}

char* rowsToCharBuffer(struct rowOfText* rows, int numRows){
    int lenTracker = 0; 
    for(int i =0; i<numRows; i++){
        lenTracker += (rows+i)->len + 1; 
    }
    char* buffer = malloc(lenTracker * sizeof(char));
    char* ptr = buffer; 

    for(int i = 0; i<numRows; i++){
        memcpy(ptr, (rows+i)->text, (rows+i)->len);
        ptr += (rows+i)->len;
        *(ptr) = '\n';
        ptr++;
    }
    return buffer; 
}
