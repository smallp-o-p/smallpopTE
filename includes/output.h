struct dynamic_text_buffer; 
void refreshScreen();
void drawRows(char c, struct dynamic_text_buffer* buf); 
void clearScreen(); 
void scrollHandler();
void drawStatusBar(struct dynamic_text_buffer* buf);
void setStatusMessage(const char* stat, ...);
void drawStatusMessage(struct dynamic_text_buffer* buf);
