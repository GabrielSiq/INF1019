#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "Utils.h"
#include "Scheduler.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

Queue * ready;
Queue * waiting;
Queue * terminated;
NODE * running;

int move_wait = false;
int resume = false;
int alarm_flag = false;

void printStatus(){
	printTime();
	printf("Fila de Prontos: ");
	printQueue(ready);
	printTime();
	printf("Fila de Espera: ");
	printQueue(waiting);
	printTime();
	printf("Lista de Terminados: ");
	printQueue(terminated);
}

void endIO(int signal){
	resume = true;
	printTime();
	printf("Fim do I/O do processo %s.\n", waiting->head->data.name);
	fflush(stdout);

}

void RoundRobinIOHandler(int signal){
	int pid;
	pid = fork();
	if(pid != 0){
		move_wait = true;
		kill(running->data.pid, SIGSTOP);
		printTime();
		printf("O processo %s iniciou o I/O.\n", running->data.name);
	}
	else{
		sleep(IO_TIME);
		kill(getppid(), SIGUSR1);
		exit(0);
	}
}

Queue * createProcesses(char ** newProgramsList, int programCount){
	int i, pid;
	char cwd[1024];

	NODE* head;
    NODE* node;
    DATA element;

    ready = ConstructQueue(50);

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
			element.pid = pid;
			element.pstatus = READY;
			strcpy(element.name, newProgramsList[i]);
			node = createNode(element);
			Enqueue(ready, node);

		}
	}
}

void timeSlice(int signal){
	if(running != NULL){
		kill(running->data.pid, SIGSTOP);
		fflush(stdout);
		Enqueue(ready, running);
		running = NULL;
		alarm_flag = false;
	}
}

void roundRobinScheduler(char ** newProgramsList, int programCount){
	int finished, status, i;
	NODE * temp;

	signal(SIGINT, RoundRobinIOHandler);
	signal(SIGUSR1, endIO);
	signal(SIGALRM, timeSlice);

	printTime();
	printf("Escalonador Round-Robin:\n");

	// Cria os processos que devem ser executados e retorna uma lista na ordem em que foram criados.
	createProcesses(newProgramsList, programCount);

	waiting = ConstructQueue(50);
	terminated = ConstructQueue(50);


	while(true){
		//printStatus();
		// Checa se todos os programas já terminaram de executar
		if(isEmpty(ready) && isEmpty(waiting) && running == NULL){
			printTime();
			printf("Todos os programas foram executados com sucesso.\n");
			return;
		}
		if(running == NULL && !isEmpty(ready))
		{
			running = Dequeue(ready);
		}
		// Continua a execução pela fatia de tempo alotada.
		if(running != NULL){
			kill(running->data.pid, SIGCONT);
			fflush(stdout);
			if(alarm_flag == false){
				alarm(TIME_SLICE);
				alarm_flag = true;
			}
		}

		if(resume == true){
			temp = Dequeue(waiting);
			Enqueue(ready, temp);
			resume = false;
		}

		// Checa se nesse tempo o programa já terminou
		if(running != NULL){
			finished = waitpid(running->data.pid, &status, WNOHANG);
			if(finished){
				Enqueue(terminated, running);
				printTime();
				printf("O programa %s de PID: %d terminou a execução.\n", running->data.name, running->data.pid);
				running = NULL;
			}else{
				if(move_wait == true){
					kill(running->data.pid, SIGSTOP);
					fflush(stdout);
					Enqueue(waiting, running);
					running = NULL;
					move_wait = false;
				}
			}
		}
	}
}

void priorityScheduler(char ** newProgramsList, int * priorityList, int programCount){
	int i;
	printTime();
	printf("Escalonador de Prioridade:\n");
	for(i = 0; i < programCount; i++){
		printTime();
		printf("%s %d\n", newProgramsList[i], priorityList[i]);
		sleep(1);
	}
}