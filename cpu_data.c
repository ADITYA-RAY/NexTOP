#include "cpu_data.h"

int total_time[ROWS];
int idle_time[ROWS];

void getCpuUtilization() {

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
  getCpuUtilization();

  getSystemVersion();

  for (int i = 0; i < ROWS; i++) {
    int total_diff = total_time[i] - prev_total[i];
    int idle_diff = idle_time[i] - prev_idle[i];

    double fraction = 1.0 - (double)idle_diff / total_diff;
    double value = 100 * fraction;

    if (i < ROWS / 2 + 1) {
      if (i != 0) {
        mvprintw(y_start + i - 1, x_start, "%s", labels[i]);
        attron(COLOR_PAIR(1));
        printw(" %.2f", value);
        printw("%% ");
        attroff(COLOR_PAIR(1));
      }

    } else {
      mvprintw(y_start + i - 5, COLS / 4, "%s", labels[i]);
      attron(COLOR_PAIR(1));
      printw(" %.2f", value);
      printw("%% ");
      attroff(COLOR_PAIR(1));
    }
  }
}

void getSystemVersion() {

  FILE *file;
  file = fopen("/proc/version", "r");
  char c;
  int count = 0;
  while (!feof(file)) {
    fscanf(file, "%c", &c);
    if (c == '#')
      break;
    attron(A_BOLD);
    mvprintw(y_start - 2, x_start + count, "%c", c);
    attroff(A_BOLD);
    count++;
  }

  fclose(file);
}
