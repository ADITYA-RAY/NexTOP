#include <curses.h>
#include <string.h>
#include <unistd.h>

void initializeCurses() {
  initscr();
  cbreak();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  

  
}