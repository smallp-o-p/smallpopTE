int backspaceWord(int col, tRow* line);
int deleteWord(int col, tRow* line);
void clrRightOfCursor(int col, tRow* line);
void undo(); 
void redo(); 
void copy(tRow* line, uint32_t cx_upper, uint32_t cx_lower); 
void paste(tRow* line, uint32_t cx); 
actionType getInverseAction(actionType action);

