#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#include <string.h>


void createProcesses(char ** newProgramsList, int programCount){
	int i, pid;
	char cwd[1024];

	for(i = 0; i < programCount; i++){
		getcwd(cwd, 1024);
		strcat(cwd, "/bin/");
		strcat(cwd, newProgramsList[i]);
		pid = fork();
		if(pid == 0){
			execl(cwd, newProgramsList[i], NULL);
			printTime();
			printf("Processo %s criado.\n", newProgramsList[i]);
			fflush(stdout);
		}
		else{
			kill(pid, SIGSTOP);
			printTime();
			printf("Processo %s pid: %d criado e imediatamente interrompido.\n", newProgramsList[i], pid);
			fflush(stdout);
			//addProcess(processList, pid)
		}
	}
}

int roundRobinScheduler(char ** newProgramsList, int programCount){
	int i;
	printTime();
	printf("Escalonador Round-Robin:\n");
	createProcesses(newProgramsList, programCount);
	while(1){
		puts(".");
		sleep(1);
	}
}

int priorityScheduler(char ** newProgramsList, int * priorityList, int programCount){
	int i;
	printTime();
	printf("Escalonador de Prioridade:\n");
	for(i = 0; i < programCount; i++){
		printTime();
		printf("%s %d\n", newProgramsList[i], priorityList[i]);
		sleep(1);
	}
}