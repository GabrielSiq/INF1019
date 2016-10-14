#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "Utils.h"
#include <string.h>

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
			head = add(head, element);
		}
	}
	return head;
}

int roundRobinScheduler(char ** newProgramsList, int programCount){
	int i;
	NODE * head;
	printTime();
	printf("Escalonador Round-Robin:\n");

	// Cria os processos que devem ser executados e retorna uma lista na ordem em que foram criados.
	head = createProcesses(newProgramsList, programCount);
	print_list(head);
	printf("\n");
	head = rotate(head);
	print_list(head);
	printf("\n");
	head = rotate(head);
	print_list(head);
	printf("\n");
	head = rotate(head);
	print_list(head);
	printf("\n");

	printf("%d", checkDone(head));
	print_list(head);
	printf("\n");

	while(true){
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