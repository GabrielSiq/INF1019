#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "Utils.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>

Queue * ready;
Queue * waiting;
Queue * terminated;
NODE * running;

int move_wait = false;
int resume = false;
int alarm_flag = false;
int priority = false;

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
	printf("Fim do I/O do processo %s. Ele irá para a fila de prontos.\n", waiting->head->data.name);
	fflush(stdout);
}

void RoundRobinIOHandler(int signal){
	int pid;
	pid = fork();
	if(pid != 0){
		move_wait = true;
		kill(running->data.pid, SIGSTOP);
		printTime();
		printf("O processo %s iniciou o I/O e será movido para a fila de espera.\n", running->data.name);
	}
	else{
		sleep(IO_TIME);
		kill(getppid(), SIGUSR1);
		exit(0);
	}
}

void createProcesses(char ** newProgramsList, int programCount, int * priorityList){
	int i, pid;
	char cwd[1024];
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
			printf("Processo %s entrou na fila de prontos.\n", newProgramsList[i]);
			fflush(stdout);
			element.pid = pid;
			strcpy(element.name, newProgramsList[i]);
			node = createNode(element);
			if(priorityList != NULL){
				node->data.priority = priorityList[i];
				OrderEnqueue(ready, node);
			}else{
				Enqueue(ready, node);
			}
		}
	}
}

void timeSlice(int signal){
	if(running != NULL){
		kill(running->data.pid, SIGSTOP);
		fflush(stdout);
		if(priority == false){
			printTime();
			printf("Processo %s interrompido por tempo. Irá para a fila de prontos.\n", running->data.name);
			Enqueue(ready, running);
		}else{
			if(running->data.priority < 9){
				running->data.priority++;
				printTime();
				printf("O processo %s teve sua prioridade reduzida para %d.\n", running->data.name, running->data.priority);
			} 
			OrderEnqueue(ready, running);
		}
		running = NULL;
		alarm_flag = false;
	}
}

void roundRobinScheduler(char ** newProgramsList, int programCount){
	int finished, status, i;
	NODE * temp;

	signal(SIGILL, RoundRobinIOHandler);
	signal(SIGUSR1, endIO);
	signal(SIGALRM, timeSlice);

	printTime();
	printf("Escalonador Round-Robin:\n");

	// Cria os processos que devem ser executados e retorna uma lista na ordem em que foram criados.
	createProcesses(newProgramsList, programCount, NULL);

	waiting = ConstructQueue(50);
	terminated = ConstructQueue(50);


	while(true){
		// Checa se todos os programas já terminaram de executar
		if(isEmpty(ready) && isEmpty(waiting) && running == NULL){
			printTime();
			printf("Todos os programas foram executados com sucesso.\n");
			printStatus();
			return;
		}
		else if(running == NULL && !isEmpty(ready))
		{
			running = Dequeue(ready);
			kill(running->data.pid, SIGCONT);
			printTime();
			printf("O processo %s foi escalonado e está rodando.\n", running->data.name);
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
				printf("O programa %s terminou a execução.\n", running->data.name);
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
	int finished, status, i;
	NODE * temp;

	signal(SIGILL, RoundRobinIOHandler);
	signal(SIGUSR1, endIO);
	signal(SIGALRM, timeSlice);

	priority = true;

	printTime();
	printf("Escalonador de Prioridade:\n");

	// Cria os processos que devem ser executados e retorna uma lista na ordem em que foram criados.
	createProcesses(newProgramsList, programCount, priorityList);

	waiting = ConstructQueue(50);
	terminated = ConstructQueue(50);


	while(true){
		// Checa se todos os programas já terminaram de executar
		if(isEmpty(ready) && isEmpty(waiting) && running == NULL){
			printTime();
			printf("Todos os programas foram executados com sucesso.\n");
			printStatus();
			return;
		}
		else if(running == NULL && !isEmpty(ready)){
			running = Dequeue(ready);
			kill(running->data.pid, SIGCONT);
			printTime();
			printf("O processo %s de prioridade %d foi escalonado e está rodando.\n", running->data.name, running->data.priority);
		}
		// Continua a execução pela fatia de tempo alotada.
		else if(running != NULL && !isEmpty(ready)){
			if(ready->head->data.priority < running->data.priority){
				kill(running->data.pid, SIGSTOP);
				OrderEnqueue(ready, running);
				printTime();
				printf("O processo %s de prioridade %d foi interrompido por prioridade.\n", running->data.name, running->data.priority);
				running = NULL;
			}
			else if(alarm_flag == false){
				alarm(TIME_SLICE);
				alarm_flag = true;
			}
		}

		if(resume == true){
			temp = Dequeue(waiting);
			OrderEnqueue(ready, temp);
			resume = false;
		}

		// Checa se nesse tempo o programa já terminou
		if(running != NULL){
			finished = waitpid(running->data.pid, &status, WNOHANG);
			if(finished){
				Enqueue(terminated, running);
				printTime();
				printf("O programa %s terminou a execução.\n", running->data.name);
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

int main(int argc, char const *argv[])
{
	int seg, i, max, priorityList[PROGRAM_LIMIT], *mem2;
	char * mem;
	char * newProgramsList[PROGRAM_LIMIT];

	max = atoi(argv[1]);

	for(i = 0; i < max; i++){
		newProgramsList[i] = (char *) malloc(CHAR_LIMIT * sizeof(char));
	}

	if ((seg=shmget(5775,PROGRAM_LIMIT * CHAR_LIMIT*sizeof(char),0600|IPC_CREAT))<0){
	    perror("shmget error");
	    exit(-1);
	}
	if((mem=(char*)shmat(seg,0,0))==(char*)-1){
	    perror("shmat error");
	    exit(-1);
	}

	for(i=0;i<max;i++){
		strcpy(newProgramsList[i], (char *)(mem + i*50));
	}

	if( argc > 2){

		if ((seg=shmget(5220,PROGRAM_LIMIT * sizeof(int),0600|IPC_CREAT))<0){
		    perror("shmget error");
		    exit(-1);
		}
		if((mem2=(int*)shmat(seg,0,0))==(int *)-1){
		    perror("shmat error");
		    exit(-1);
		}

		for(i=0;i<max;i++){
			priorityList[i] = mem2[i];
		}
		priorityScheduler(newProgramsList, priorityList, max);
	}
	else{
		roundRobinScheduler(newProgramsList, max);
	}
	return 0;
}