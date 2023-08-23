#include <stdio.h>
#include <unistd.h>

#define ROWS  9
#define COLUMNS 10

int total_time[ROWS];
int idle_time[ROWS];

void get_cpu_utilization()
{
  int array[ROWS][COLUMNS];

  FILE*file;

  file = fopen("/proc/stat", "r");

  if (file == NULL)
  {
    printf("Error opening file.\n");
  }

  int row = 0;

  while (!feof(file))
  {
    if (ferror(file))
    {
      printf("Error reading file.\n");
    }
    
    fscanf(file, "%*s");
    
    for (int i = 0; i < COLUMNS; i++)
    {
      if (fscanf(file,"%d",&array[row][i]) == EOF)
        break;
    }

    row++;

    if (row==ROWS) break; 
  }

  fclose(file);

  for (int i = 0; i< ROWS; i++)
  {
    total_time[i]=0;
    idle_time[i]=0;
  }

  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j <COLUMNS; j++)
    {
      total_time[i] += array[i][j];
    }
    idle_time[i] = array[i][3];
  }
}


int main()
{
  get_cpu_utilization();
 
 while (1)
  {
    sleep(1);
    
    int prev_total[ROWS];
    int prev_idle[ROWS];
    
    for (int i = 0; i < ROWS; i++)
    {
      prev_total[i] = total_time[i];
      prev_idle[i] = idle_time[i];
    }
    
    get_cpu_utilization();
    
    for (int i = 0; i< ROWS;i++)
    {
      int total_diff = total_time[i] - prev_total[i];
      int idle_diff = idle_time[i] - prev_idle[i];

      double fraction = 1.0 - (double)idle_diff/total_diff;

      printf("%f ",100*fraction);
    }
    
    printf("\n");
    
    fflush(stdout);
  }
    return 0;
}
