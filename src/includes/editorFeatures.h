void findString(); 
void highlightKeywords(char* line); 
void rememberTextRow(tRow* row, actionType lastAction);
void rememberRows(int* rowNumbers, int numRows, actionType lastAction); 
void removeNewLines(rememberStruct* previousState);
void restoreNewLines(rememberStruct* previousState);
void undoInsertionDeletion(rememberStruct* previousState);
