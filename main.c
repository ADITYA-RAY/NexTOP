#include <ctype.h>
#include <curses.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ROWS 9
#define COLUMNS 10
#define x_start 2
#define y_start 4
#define sleep_duration 1
#define MAX_LINE_LENGTH 256

int total_time[ROWS];
int idle_time[ROWS];
int memory[4];

struct processInfo {
  int pid;
  char user[50];
  char state[50];
  int utime;
  int stime;
  int num_threads;
  int process_time;
};

struct processInfo process[1024];

int compareStruct(const void *a, const void *b) {
  return   ((struct processInfo *)b)->utime-((struct processInfo *)a)->utime;
}

void structSort() {
  size_t cpu = sizeof(process) / sizeof(process[0]);
  qsort(process, cpu, sizeof(struct processInfo), compareStruct);
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
    mvprintw(y_start - 2, x_start + count, "%c", c);

    count++;
  }

  fclose(file);
}

void populateProcessData(const char *filename, int i) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Failed to open file");
    return;
  }

  char line[1024];

  while (fgets(line, sizeof(line), file) != NULL) {
    char *token = strtok(line, " \t\n");

    int counter = 1;
    while (token != NULL) {
      if (counter == 1)
        process[i].pid = atoi(token);
      else if (counter == 2)
        strcpy(process[i].user, token);
      else if (counter == 3)
        strcpy(process[i].state, token);
      else if (counter == 14)
        process[i].utime = atoi(token);
      else if (counter == 15)
        process[i].stime = atoi(token);
      else if (counter == 20)
        process[i].num_threads = atoi(token);
      else if (counter == 22) {
        process[i].process_time = atoi(token);
        break;
      }
      counter += 1;
      token = strtok(NULL, " \t\n");
    }
  }

  fclose(file);
}

int isNumber(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i])) {
      return 0;
    }
  }
  return 1;
}

void initializeProcesses() {
  for (int i = 1; i <= 1024; i++) {
    process[i].pid = 0;
  }
  DIR *procDir = opendir("/proc");
  if (procDir == NULL) {
    perror("Failed to open /proc directory");
  }

  struct dirent *entry;
  int count = 0;
  while ((entry = readdir(procDir))) {
    if (isNumber(entry->d_name)) {
      char proc[40] = "/proc/";
      char stat[30] = "/stat";
      strcat(proc, entry->d_name);
      strcat(proc, stat);
      populateProcessData(proc, count);
      count += 1;
    }
  }

  closedir(procDir);
};

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
  total_swap /= 1024;
  free_swap /= 1024;

  memory[0] = total_memory;
  memory[1] = occupied_memory;
  memory[2] = total_swap;
  memory[3] = free_swap;
}

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
  getCpuUtilization();
  getMemoryUtilization();
  getSystemVersion();

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
  mvprintw(y_start + 1, COLS / 2, "%d /%d (%d%%)", memory[2] - memory[3],
           memory[2],
           (int)(((memory[3] - memory[2]) / (float)memory[2]) * 100));
}

void printMemoryData() {}

void printProcessData() {
  char *headers[] = {"pid",   "user",    "state",       "utime",
                     "stime", "threads", "process time"};
  int colSpaces[] = {0, 10, 50, 60, 80, 100, 110};
  for (int i = 0; i < 7; i++) {
    mvprintw(12, x_start + colSpaces[i], "%s", headers[i]);
  }

  for (int i = 0; i < 200; i++) {
    mvprintw(14 + i, x_start, "%d", process[i].pid);
    mvprintw(14 + i, x_start + colSpaces[1], "%s", process[i].user);
    mvprintw(14 + i, x_start + colSpaces[2], "%s", process[i].state);
    mvprintw(14 + i, x_start + colSpaces[3], "%d", process[i].utime);
    mvprintw(14 + i, x_start + colSpaces[4], "%d", process[i].stime);
    mvprintw(14 + i, x_start + colSpaces[5], "%d", process[i].num_threads);
    mvprintw(14 + i, x_start + colSpaces[6], "%d", process[i].process_time);
  }
}

int main() {
  initializeCurses();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);

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
