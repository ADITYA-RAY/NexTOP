#include <ctype.h>
#include <curses.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "processes.c"
#include "cpu_data.c"
#include "memory_data.c"
#include "curses.c"

#define sleep_duration 1

int compareStruct(const void *a, const void *b);
void structSort();
void initializeCurses();

