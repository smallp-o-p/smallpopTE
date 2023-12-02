#pragma once 
#include <stdint.h>
#include <time.h>

typedef struct rowOfText tRow; 

typedef struct softDeletedRowOfText pastTextRow; 

typedef enum actionType actionType; 

typedef struct rowOfText{
    int len; // length of string, length of array
    char* text; // raw text 
    int renderSize; // length of render string
    char* render; // text converted to work with our text editor
    char* highLighting; 
} tRow;  

typedef struct copyBuffer{
    pastTextRow* rows; 
    uint32_t numLines; 
    uint32_t byteCount; 
} copyBuffer; 

typedef enum actionType{
    INITIAL_STATE, 
    INSERT,
    REMOVE,
    NEWLINE,
    RM_NEWLINE, 
    CURRENT_STATE, 
} actionType;

typedef struct softDeletedRowOfText{
    int len;
    int rowNum; 
    int at; 
    char* text;
} pastTextRow; 

typedef struct pastTextRows{
    pastTextRow** rows; 
    uint32_t* rowIndexes; 
    uint32_t numRows;   
    time_t timestamp;
    actionType action; 
    void (*howtoUndoMe)(struct pastTextRows*); 
} rememberStruct; 
