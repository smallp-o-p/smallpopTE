#include "error.h"
#include "pch.h"
#include "output.h"

void die(const char* s){
    perror(s);
    exit(1);
}
