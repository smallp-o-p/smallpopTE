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
        fileName = saveConfirm();
        if(fileName == NULL){
            return -1; 
        } 
        fp = fopen(fileName, "w+");
    }
    else{
        fp = fopen(fileName, "r+");
    }
    if(!fp){
        return -1; 
    }
    for (int i = 0; i < E.numRowsofText; i++)
    {
        struct rowOfText row = E.textRows[i];
        if(i == E.numRowsofText-1){
            fwrite(row.text, sizeof(char), row.len, fp);
            break;
        }
        fwrite(strcat(row.text, "\n"), sizeof(char), row.len+1, fp);
    }
    fclose(fp);
    E.dirty = 0;
    int statusMessageSize = strlen(fileName) + 32;
    char buf[statusMessageSize];
    snprintf(buf, statusMessageSize, "Saved file to: %s", fileName);
    setStatusMessage(buf);
    return 0;
}

