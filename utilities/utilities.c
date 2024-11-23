#include <stdio.h>
#include <string.h>
#include"./utilities.h"
void printc(char *text){
    printf("%*s", 75 + (int)strlen(text) / 2, text);
}
