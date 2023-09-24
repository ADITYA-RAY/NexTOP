#include "header.h"
#include <curses.h>

int main() {
  initializeCurses();

  while (2) {
    clear();
    sleep(sleep_duration);
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int title_x =
        (max_x - strlen("NxtTop - a system monitor TUI for linux")) / 2;
    int title_y = 2;
    attron(COLOR_PAIR(1));
    attron(A_UNDERLINE);
    attron(A_BOLD);
    mvprintw(title_y, title_x, "%s",
             "NxtTop - a system monitoring TUI for linux");
    attroff(A_BOLD);
    attroff(A_UNDERLINE);
    attroff(COLOR_PAIR(1));

    initializeProcesses();
    structSort();
    printCpuData();
    printMemoryData();
    printProcessData();
    refresh();
    fflush(stdout);
  }

  endwin();

  return 0;
}
