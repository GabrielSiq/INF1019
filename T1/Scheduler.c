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

int print_status_flag = true;

int move_wait = false;
int resume = false;
int alarm_flag = false;
int priority = false;

/* Imprime o status de todas as filas */
void printStatus(){
	if(print_status_flag == true){
		printf("\n");
		printf("---------------- STATUS ATUAL -------------\n");
		printf("\n");
		printTime();
		if(running != NULL){
			printf("Em execução: %s\n", running->data.name);
		}
		else{
			printf("Em execução: Nenhum\n");
		}
		printTime();
		printf("Fila de Prontos: ");
		printQueue(ready);
		printTime();
		printf("Fila de Espera: ");
		printQueue(waiting);
		printTime();
		printf("Lista de Terminados: ");
		printQueue(terminated);
		printf("\n");
		printf("------------------------------------------\n");
		printf("\n");
	}
}

/* Handler para o sinal de término de I/O */
void endIO(int signal){
	resume = true;
}

/* Handler para o sinal de início de I/O */
void IOHandler(int signal){
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

/* Cria os novos processos e os coloca na fila de prontos */
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

/* Handler para o alarme de fatia de tempo esgotada. Serve tanto para RoundRobin
quanto para ajuste dinâmico de prioridade */
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

/* Escalonador RoundRobin */
void roundRobinScheduler(char ** newProgramsList, int programCount){
	int finished, status, i;
	NODE * temp;

	signal(SIGILL, IOHandler);
	signal(SIGUSR1, endIO);
	signal(SIGALRM, timeSlice);

	printTime();
	printf("Escalonador Round-Robin:\n");

	// Cria os processos que devem ser executados.
	createProcesses(newProgramsList, programCount, NULL);

	waiting = ConstructQueue(50);
	terminated = ConstructQueue(50);


	while(true){
		// Checa se todos os programas já terminaram de executar
		if(isEmpty(ready) && isEmpty(waiting) && running == NULL){
			printTime();
			printf("Todos os programas foram executados com sucesso.\n");
			printStatus();
			DestructQueue(ready);
			DestructQueue(terminated);
			DestructQueue(waiting);
			return;
		}

		// Tira processo da fila de espera e coloca em prontos.
		if(resume == true){
			printTime();
			printf("Fim do I/O do processo %s. Ele irá para a fila de prontos.\n", waiting->head->data.name);
			temp = Dequeue(waiting);
			Enqueue(ready, temp);
			resume = false;
			printStatus();
		}

		// Escalona processo e dispara contagem de tempo
		else if(running == NULL && !isEmpty(ready))
		{
			running = Dequeue(ready);
			kill(running->data.pid, SIGCONT);
			printTime();
			printf("O processo %s foi escalonado e está rodando.\n", running->data.name);
			printStatus();
			if(alarm_flag == false){
				alarm(TIME_SLICE);
				alarm_flag = true;
			}
		}
		
		if(running != NULL){
			// Checa se nesse tempo o programa já terminou
			finished = waitpid(running->data.pid, &status, WNOHANG);
			if(finished){
				Enqueue(terminated, running);
				printTime();
				printf("O programa %s terminou a execução.\n", running->data.name);
				running = NULL;
			}else{
				// Move processo em I/O para espera
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

/* Escalonador de Prioridade */
void priorityScheduler(char ** newProgramsList, int * priorityList, int programCount){
	int finished, status, i;
	NODE * temp;

	signal(SIGILL, IOHandler);
	signal(SIGUSR1, endIO);
	signal(SIGALRM, timeSlice);

	priority = true;

	printTime();
	printf("Escalonador de Prioridade:\n");

	// Cria os processos que devem ser executados.
	createProcesses(newProgramsList, programCount, priorityList);

	waiting = ConstructQueue(50);
	terminated = ConstructQueue(50);


	while(true){
		// Checa se todos os programas já terminaram de executar
		if(isEmpty(ready) && isEmpty(waiting) && running == NULL){
			printTime();
			printf("Todos os programas foram executados com sucesso.\n");
			printStatus();
			DestructQueue(ready);
			DestructQueue(terminated);
			DestructQueue(waiting);
			return;
		}
		// Escalona processo.
		else if(running == NULL && !isEmpty(ready)){
			running = Dequeue(ready);
			kill(running->data.pid, SIGCONT);
			printTime();
			printf("O processo %s de prioridade %d foi escalonado e está rodando.\n", running->data.name, running->data.priority);
			printStatus();
		}
		// Interrompe processo por prioridade.
		else if(running != NULL && !isEmpty(ready)){
			if(ready->head->data.priority < running->data.priority){
				kill(running->data.pid, SIGSTOP);
				OrderEnqueue(ready, running);
				printTime();
				printf("O processo %s de prioridade %d foi interrompido por prioridade.\n", running->data.name, running->data.priority);
				running = NULL;
			}
			// Conta tempo para proxima queda de prioridade
			else if(alarm_flag == false){
				alarm(TIME_SLICE);
				alarm_flag = true;
			}
		}
		// Devolve à fila de prontos quando acaba I/O
		if(resume == true){
			printTime();
			printf("Fim do I/O do processo %s. Ele irá para a fila de prontos.\n", waiting->head->data.name);
			temp = Dequeue(waiting);
			OrderEnqueue(ready, temp);
			resume = false;
			printStatus();
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
				// Move para espera quando começa o I/O.
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
		shmdt(mem2);
	}
	else{
		roundRobinScheduler(newProgramsList, max);
		shmdt(mem);
	}
	return 0;
}