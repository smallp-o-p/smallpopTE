void removeRow(int row);
void addRow(int at, char* str, unsigned long len);
void addAndShiftRowsDown(int fromRow);
void updateRow(struct rowOfText* row);
void moveRowText(struct rowOfText* from, struct rowOfText* to);
int rowCx2Rx(struct rowOfText* row, int cx);
void insertCharInRow(int c, struct rowOfText* row, int col);
void insertChar(int c);
void delChar(int col, int op);
void delCharInRow(int op, struct rowOfText* row, int col); 
void appendRowText(struct rowOfText*, char* str, int len);
void updateRowInternalText(uint32_t rowNum, char *text, uint32_t len);
foundPair* searchSubstr(char* needle, int* countToUpdate); 
void freepastTextRows(void* past);
#define rowAt(c_y) E.textRows[c_y] 
