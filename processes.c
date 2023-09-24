#include "processes.h"

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

int isNumber(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i])) {
      return 0;
    }
  }
  return 1;
}

int compareStruct(const void *a, const void *b) {
  return ((struct processInfo *)b)->utime - ((struct processInfo *)a)->utime;
}

void structSort() {
  size_t cpu = sizeof(process) / sizeof(process[0]);
  qsort(process, cpu, sizeof(struct processInfo), compareStruct);
}