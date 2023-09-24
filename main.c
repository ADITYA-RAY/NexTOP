#include "header.h"

int main() {
  initializeCurses();

  while (2) {
    clear();
    sleep(sleep_duration);
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
