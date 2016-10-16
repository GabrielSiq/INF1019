#define true 1
#define false 0
#define TIME_SLICE 2
#define IO_TIME 5

void roundRobinScheduler(char ** newProgramsList, int programCount);
void priorityScheduler(char ** newProgramsList, int * priorityList, int programCount);