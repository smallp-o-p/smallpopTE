int backspaceWord(int col, tRow* line);
int deleteWord(int col, tRow* line);
void clrRightOfCursor(int col, tRow* line);
void undo(); 
void redo(); 
void copy(uint32_t cy_upper, uint32_t cy_lower, uint32_t cx_leftmost, uint32_t cx_rightmost);
void paste(); 
actionType getInverseAction(actionType action);

