#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "Utils.h"
#include "Scheduler.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

NODE * programControl;
NODE * IOProcess;
int startIO = false;
int endIO = false;

void printStatus(){
	printTime();
	printf("Lista de Prontos\n");
	print_list(programControl, READY);
	printf("Lista em Espera\n");
	print_list(programControl, WAITING);
	printf("Lista de Terminados\n");
	print_list(programControl, FINISHED);
}

void IOFlag(int signal){
	endIO = true;
}

void RRstartIO(){
	if(startIO == false){
		return;
	}
	IOProcess->data.pstatus = WAITING;
	kill(IOProcess->data.pid, SIGSTOP);
	printf("%s interrompido para I/O\n", IOProcess->data.name);
	startIO = false;
}

void RRendIO(){
	if(endIO == false){
		return;
	}
	printf("Fim do I/O. %s será colocado no fim da lista de prontos.\n", IOProcess->data.name);
	fflush(stdout);
	IOProcess->data.pstatus = READY;
	programControl = move_to_end(IOProcess, programControl);
	endIO = false;
}

void checkIO(){
	RRendIO();
	RRstartIO();
}

void RoundRobinIOHandler(int signal){
	int pid, status;
	NODE * currentNode;
	currentNode = programControl;
	pid = fork();
	// Processo pai volta para o que estava fazendo.
	if(pid != 0){
		startIO = true;
		IOProcess = currentNode;
	}
	else{
		sleep(IO_TIME);
		kill(getppid(), SIGUSR1);
		exit(0);
	}
}

int checkDone(NODE * List){
	NODE * current = List;
	while(current != NULL){
		if(current->data.pstatus != FINISHED){
			return 0;
		}
		current = current->next;
	}
	return 1;
}

NODE* createProcesses(char ** newProgramsList, int programCount){
	int i, pid;
	char cwd[1024];

	// Cria lista de processos
	NODE* head;
    NODE* node;
    DATA element;
    init(&head);

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
			head = add(head, element);
		}
	}
	return head;
}

void roundRobinScheduler(char ** newProgramsList, int programCount){
	int i, finished, status;
	NODE * current, * temp;
	DATA * currentData;

	signal(SIGINT, RoundRobinIOHandler);
	signal(SIGUSR1, IOFlag);

	printTime();
	printf("Escalonador Round-Robin:\n");

	// Cria os processos que devem ser executados e retorna uma lista na ordem em que foram criados.
	programControl = createProcesses(newProgramsList, programCount);
	while(true){
		checkIO();
		//printStatus();
		// Checa se todos os programas já terminaram de executar
		if(checkDone(programControl)){
			printTime();
			printf("Todos os programas foram executados com sucesso.\n");
			return;
		}
		currentData = &(programControl->data);
		// Checa se o processo corrente na lista está pronto
		if(currentData->pstatus == READY){
			// Continua a execução pela fatia de tempo alotada.
			kill(currentData->pid, SIGCONT);
			fflush(stdout);
			currentData->pstatus = RUNNING;
			sleep(TIME_SLICE);

			// Checa se nesse tempo o programa já terminou
			finished = waitpid(currentData->pid, &status, WNOHANG);
			if(finished == false){
				kill(currentData->pid, SIGSTOP);
				currentData->pstatus = READY;
			}
			else{
				currentData->pstatus = FINISHED;
				printTime();
				printf("O programa %s de PID: %d terminou a execução.\n", currentData->name, currentData->pid);
			}
		}
		programControl=rotate(programControl);
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