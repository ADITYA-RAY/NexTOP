#include "memory_data.h"

int memory[4];

void getMemoryUtilization() {
  FILE *file;
  file = fopen("/proc/meminfo", "r");
  char line[MAX_LINE_LENGTH];
  long total_memory = 0;
  long occupied_memory = 0;
  long total_swap = 0;
  long free_swap = 0;

  if (file == NULL) {
    printf("Error Opening file.\n");
  }

  while (!feof(file)) {
    while (fgets(line, sizeof(line), file)) {

      if (strstr(line, "MemTotal:") != NULL) {
        sscanf(line, "MemTotal: %ld kB", &total_memory);
      } else if (strstr(line, "MemAvailable:") != NULL) {
        sscanf(line, "MemAvailable: %ld kB", &occupied_memory);
      } else if (strstr(line, "SwapTotal:") != NULL) {
        sscanf(line, "SwapTotal: %ld kB", &total_swap);
      } else if (strstr(line, "SwapFree:") != NULL) {
        sscanf(line, "SwapFree: %ld kB", &free_swap);
      }
    }
  }
  fclose(file);

  total_memory /= 1024;
  occupied_memory /= 1024;
  total_swap /= 1024;
  free_swap /= 1024;

  memory[0] = total_memory;
  memory[1] = occupied_memory;
  memory[2] = total_swap;
  memory[3] = free_swap;
}

void printMemoryData() {
  getMemoryUtilization();
  
  mvprintw(y_start, COLS / 2, "%d /%d (%d%%)", memory[1], memory[0],
           (int)(((float)memory[1] / memory[0]) * 100));
  mvprintw(y_start + 1, COLS / 2, "%d /%d (%d%%)", memory[2] - memory[3],
           memory[2],
           (int)(((memory[3] - memory[2]) / (float)memory[2]) * 100));
}