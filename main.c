#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ROWS 9
#define COLUMNS 10
#define x_start 2
#define y_start 2
#define sleep_duration 1
#define MAX_LINE_LENGTH 256

int total_time[ROWS];
int idle_time[ROWS];
int memory[4];

void get_running_process() {
  FILE *file;
  file = fopen("/proc/*/stat", "r");

  if (file == NULL) {
    printf("Error Opening file.\n");
  }

  while (!feof(file)) {
    if (ferror(file)) {
      printf("Error Opening file.\n");
    }
  }
}

void get_memory_utilization() {
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
      // Search for lines containing "MemTotal" and "MemAvailable" to get total
      // and available memory
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
  total_swap /=1024;
  free_swap /=1024;

  memory[0] = total_memory;
  memory[1] = occupied_memory;
  memory[2] = total_swap;
  memory[3] = free_swap;
}

void get_cpu_utilization() {

  int array[ROWS][COLUMNS];

  FILE *file;

  file = fopen("/proc/stat", "r");

  if (file == NULL) {
    printf("Error opening file.\n");
  }

  int row = 0;

  while (!feof(file)) {
    if (ferror(file)) {
      printf("Error reading file.\n");
    }

    fscanf(file, "%*s");

    for (int i = 0; i < COLUMNS; i++) {
      if (fscanf(file, "%d", &array[row][i]) == EOF)
        break;
    }

    row++;

    if (row == ROWS)
      break;
  }

  fclose(file);

  for (int i = 0; i < ROWS; i++) {
    total_time[i] = 0;
    idle_time[i] = 0;
  }

  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      total_time[i] += array[i][j];
    }
    idle_time[i] = array[i][3];
  }
}

void initializeCurses() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  start_color();
}

void printCpuData() {
  int values[ROWS];
  const char *labels[9] = {"Total:", "cpu 0:", "cpu 1:", "cpu 2:", "cpu 3:",
                           "cpu 4:", "cpu 5:", "cpu 6:", "cpu 7:"};
  int prev_total[ROWS], prev_idle[ROWS];

  for (int i = 0; i < ROWS; i++) {
    prev_total[i] = total_time[i];
    prev_idle[i] = idle_time[i];
  }
  box(stdscr, 0, 0);
  get_cpu_utilization();
  get_memory_utilization();

  for (int i = 0; i < ROWS; i++) {
    int total_diff = total_time[i] - prev_total[i];
    int idle_diff = idle_time[i] - prev_idle[i];

    double fraction = 1.0 - (double)idle_diff / total_diff;
    double value = 100 * fraction;

    if (i < ROWS / 2 + 1) {
      mvprintw(y_start + i, x_start, "%s", labels[i]);
      attron(COLOR_PAIR(1));
      printw(" %.2f", value);
      printw("%% ");
      attroff(COLOR_PAIR(1));
    } else {
      mvprintw(y_start + i - 5, COLS / 4, "%s", labels[i]);
      attron(COLOR_PAIR(1));
      printw(" %.2f", value);
      printw("%% ");
      attroff(COLOR_PAIR(1));
    }
  }
  mvprintw(y_start, COLS / 2, "%d /%d (%d%%)", memory[1], memory[0],
           (int)(((float)memory[1] / memory[0]) * 100));
  mvprintw(y_start+1, COLS / 2, "%d /%d (%d%%)", memory[2]-memory[3], memory[2],
           (int)(((memory[3]-memory[2]) / (float)memory[2]) * 100));
}

void printMemoryData() {}

void printProcessData() {}

int main() {
  initializeCurses();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);

  while (2) {
    clear();
    sleep(sleep_duration);
    printCpuData();
    printMemoryData();
    printProcessData();
    refresh();
    fflush(stdout);
  }
  endwin();

  return 0;
}
