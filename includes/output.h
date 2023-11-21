struct dynamic_text_buffer; 
void refreshScreen();
void drawRows(char c); 
void clearScreen(); 
void scrollHandler();
void drawStatusBar();
void setStatusMessage(msgType type, const char* stat, ...);
void drawStatusMessage();
