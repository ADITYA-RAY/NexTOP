#include <curses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define x_start 2
#define y_start 4

void populateProcessData(const char *filename, int i);
void initializeProcesses();
void printProcessData() ;
int isNumber(const char *str);


