#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Utils.h"
#include <sys/shm.h>
#include <sys/wait.h>

int file_output_flag = false;

// Checa a integridade do comando de entrada
int integrityCheck(char * command, char * program, char * priority){
	char priorityIntegrity[9];
	int priorityValue;
	struct stat programCheck;
	char dir[1024];
	strcpy(dir, "bin/");
	strcat(dir, program);

	strncpy(priorityIntegrity, priority, 9);
	priorityIntegrity[9] = 0;
	if(strcmp(command, "exec") != 0){
		return -1;
	}
	if(strcmp(priorityIntegrity, "priority=") != 0){
		return -2;
	}
	priorityValue = (int) priority[9] - '0';
	if(priorityValue > 7 || priorityValue < 1){
		return -3;
	}
	if(!(stat(dir, &programCheck) == 0 && programCheck.st_mode & S_IXUSR)) {
		return -4;
	}
	return priorityValue;
}

int main(int argc, char const *argv[])
{
	char command[CHAR_LIMIT], program[CHAR_LIMIT], priority[CHAR_LIMIT], priorityIntegrity[9];
	char * newProgramsList[PROGRAM_LIMIT], cwd[1024], ascii[10]={0}, * mem;
	int programCount=0, i, j, priorityList[PROGRAM_LIMIT], priorityValue, status, seg, pid, *mem2;
	FILE *input, *output;

	if ((seg=shmget(5220,PROGRAM_LIMIT * sizeof(int),0600|IPC_CREAT))<0){
	    perror("shmget error");
	    exit(-1);
	}
	if((mem2=(int*)shmat(seg,0,0))==(int*)-1){
	    perror("shmat error");
	    exit(-1);
	}
	if ((seg=shmget(5775,PROGRAM_LIMIT * CHAR_LIMIT*sizeof(char),0600|IPC_CREAT))<0){
	    perror("shmget error");
	    exit(-1);
	}
	if((mem=(char*)shmat(seg,0,0))==(char*)-1){
	    perror("shmat error");
	    exit(-1);
	}

	// A entrada e a saída podem ser feitas por arquivo,
	// de modo a facilitar a avaliação e teste.
	input = fopen("input_priority.txt", "r");
	if(input == NULL){
		printTime();
		printf("Erro na abertura do arquivo de entrada.\n");
		exit(1);
	}
	if(file_output_flag == true){
		output = fopen("output_priority.txt", "w");
		if(output == NULL){
			printTime();
			printf("Erro na abertura do arquivo de saída.\n");
			exit(1);
		}

		dup2(fileno(output), STDOUT_FILENO);
	}

	printTime();
	printf("Interpretador de Prioridade:\n");

	//Aloca espaço no vetor de programas
	for(i = 0; i < PROGRAM_LIMIT; i++){
		newProgramsList[i] = (char *) malloc(CHAR_LIMIT * sizeof(char));
	}

	i = 0;
	//Leitura dos comandos
	while(fscanf(input, "%s %s %s", command, program, priority)  == 3){
		// separa a prioridade
		priorityValue = integrityCheck(command, program, priority);

		if(priorityValue == -1){
			printTime();
			printf("O comando %s na linha %d não é reconhecido como um comando válido. A linha será ignorada.\n", command, i);
		}
		else if(priorityValue == -2){
			printTime();
			printf("O argumento %s na linha %d não é reconhecido como um argumento de prioridade válido. A linha será ignorada.\n", priority, i);
		}
		else if(priorityValue == -3){
			printTime();
			printf("O valor recebido na linha %d não é um valor de prioridade válido (de 1 a 7).\n", i);
		}
		else if(priorityValue == -4){
			printTime();
			printf("O programa %s referenciado na linha %d não foi encontrado. A linha será ignorada.\n", program, i);
		}
		else{
			printTime();
			printf("O programa %s teve sintaxe válida e entrará na lista de novos programas com prioridade %d.\n", program, priorityValue);
			strcpy(newProgramsList[programCount], program);
			priorityList[programCount] = priorityValue;
			programCount++;
		}
		i++;
	}
	fclose(input);

	for(i =0; i< programCount; i++){
		for(j=0; j < strlen(newProgramsList[i])+1;j++){
			mem[i * CHAR_LIMIT + j] = newProgramsList[i][j];
		}
	}
	for(i =0; i< programCount; i++){
		mem2[i] = priorityList[i];
	}

	if (programCount != 0){
		printTime();
		printf("Enviando programas ao Escalonador de Prioridade..\n");
		fflush(stdout);
		sprintf(ascii, "%d", programCount);
		getcwd(cwd, 1024);
		strcat(cwd, "/scheduler");
		pid = fork();
		if(pid == 0){
			execl(cwd, "scheduler", ascii, "priority", NULL);
			exit(0);
		}else{
			waitpid(pid, &status, 0);
		}
	}
	else{
		printTime();
		printf("Nenhuma entrada válida foi detecatada. O escalonador não será acionado.\n");
	}
	shmdt(mem);
	shmdt(mem2);
	if(file_output_flag == true){
		fclose(output);
	}
	return 0;
}