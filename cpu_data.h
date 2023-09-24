#include <curses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define x_start 2
#define y_start 6
#define ROWS 9
#define COLUMNS 10 

void getCpuUtilization();
void printCpuData();
void getSystemVersion();
