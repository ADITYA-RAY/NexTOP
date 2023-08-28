#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define ROWS 9
#define COLUMNS 10

int total_time[ROWS];
int idle_time[ROWS];
int memory[3];

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

  if (file == NULL) {
    printf("Error Opening file.\n");
  }

  while (!feof(file)) {
    if (ferror(file)) {
      printf("Error Opening file.\n");
    }

    for (int i = 0; i < 3; i++) {
      fscanf(file, "%*s");
      if (fscanf(file, "%d", &memory[i]) == EOF) {
        break;
      }
    }
  }
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

int main() {

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);

  int y_start = 2;
  int x_start = 2;
  int num_values = 9;
  int values[num_values];
  const char *labels[9] = {
      "Total:", "cpu 0:", "cpu 1:", "cpu 2:", "cpu 3:",
      "cpu 4:", "cpu 5:", "cpu 6:", "cpu 7:"};

  get_memory_utilization();
  get_cpu_utilization();

  while (2) {
    clear();
    sleep(1);

    int prev_total[ROWS];
    int prev_idle[ROWS];

    for (int i = 0; i < ROWS; i++) {
      prev_total[i] = total_time[i];
      prev_idle[i] = idle_time[i];
    }
    box(stdscr, 0, 0);
    get_cpu_utilization();

    for (int i = 0; i < ROWS; i++) {
      int total_diff = total_time[i] - prev_total[i];
      int idle_diff = idle_time[i] - prev_idle[i];

      double fraction = 1.0 - (double)idle_diff / total_diff;
      double value = 100 * fraction;

      if (i < 5) {
        mvprintw(y_start + i, x_start, "%s", labels[i]);
        attron(COLOR_PAIR(1));
        printw(" %.2f", value);
        printw("%% ");
        attroff(COLOR_PAIR(1));
        printw(" [");
        for (int j = 0; j < 20; j++) {
          if (j < value / 5) {
            attron(COLOR_PAIR(1));
            mvprintw(y_start + i, x_start + 15 + j, "|");
            attroff(COLOR_PAIR(1));
          } else {
            mvprintw(y_start + i, x_start + 15 + j, " ");
          }
        }
        printw("]");
      } else {
        mvprintw(y_start + i - 5, COLS / 2, "%s", labels[i]);
        attron(COLOR_PAIR(1));
        printw(" %.2f", value);
        printw("%% ");
        attroff(COLOR_PAIR(1));
        printw(" [");
        for (int j = 0; j < 20; j++) {
          if (j < value / 5) {
            attron(COLOR_PAIR(1));
            mvprintw(y_start + i - 5, COLS / 2 + 15 + j, "|");
            attroff(COLOR_PAIR(1));
          } else {
            mvprintw(y_start + i - 5, COLS / 2 + 15 + j, " ");
          }
        }
        printw("]");
      }

      refresh();
    }
    fflush(stdout);
  }
  endwin();

  return 0;
}
