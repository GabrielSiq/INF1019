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
int integrityCheck(char * command, char * program){
	struct stat programCheck;
	char dir[1024];
	strcpy(dir, "bin/");
	strcat(dir, program);
	if(strcmp(command, "exec") != 0){
		return -1;
	}
	if(!(stat(dir, &programCheck) == 0 && programCheck.st_mode & S_IXUSR)) {
		return -4;
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	char command[CHAR_LIMIT], program[CHAR_LIMIT], arg[10]={0};
	char * newProgramsList[PROGRAM_LIMIT], * mem, cwd[1024];
	int programCount=0, integrityValue, i, seg, j, pid, status;
	FILE *input, *output;

	if((seg=shmget(5775,PROGRAM_LIMIT * CHAR_LIMIT*sizeof(char),0600|IPC_CREAT))<0){
	    perror("shmget error");
	    exit(-1);
	}
	if((mem=(char*)shmat(seg,0,0))==(char*)-1){
	    perror("shmat error");
	    exit(-1);
	}

	// A entrada e a saída serão feitas por arquivo,
	// de modo a facilitar a avaliação e teste.
	input = fopen("input_robin.txt", "r");
	if(input == NULL){
		printTime();
		printf("Erro na abertura do arquivo de entrada.\n");
		exit(1);
	}
	if(file_output_flag == true){
		output = fopen("output_robin.txt", "w");
		if(output == NULL){
			printTime();
			printf("Erro na abertura do arquivo de saída.\n");
			exit(1);
		}

		dup2(fileno(output), STDOUT_FILENO);
	}

	printTime();
	printf("Interpretador Round-Robin:\n");

	// Aloca espaço no vetor de programas
	for(i = 0; i < PROGRAM_LIMIT; i++){
		newProgramsList[i] = (char *) malloc(CHAR_LIMIT * sizeof(char));
	}
	i = 0;
	//Leitura dos comandos
	while(fscanf(input, "%s %s", command, program)  == 2){
		integrityValue = integrityCheck(command, program);
		if(integrityValue == -1){
			printTime();
			printf("O comando %s na linha %d não é reconhecido como um comando válido. A linha será ignorada.\n", command, i);
		}
		else if(integrityValue == -4){
			printTime();
			printf("O programa %s referenciado na linha %d não foi encontrado. A linha será ignorada.\n", program, i);
		}
		else{
			printTime();
			printf("O programa %s teve sintaxe válida e entrará na lista de novos programas\n", program);
			strcpy(newProgramsList[programCount], program);
			programCount++;
		}
		i++;
	}

	for(i =0; i< programCount; i++){
		for(j=0; j < strlen(newProgramsList[i])+1;j++){
			mem[i * CHAR_LIMIT + j] = newProgramsList[i][j];
		}
	}

	fclose(input);
	if (programCount != 0){
		printTime();
		printf("Enviando programas ao escalonador round-robin..\n");
		fflush(stdout);
		sprintf(arg, "%d", programCount);
		getcwd(cwd, 1024);
		strcat(cwd, "/scheduler");
		pid = fork();
		if(pid == 0){
			execl(cwd, "scheduler", arg, NULL);
		}else{
			waitpid(pid, &status, 0);
		}
	}
	else{
		printTime();
		printf("Nenhuma entrada válida foi detecatada. O escalonador não será acionado.\n");
	}
	shmdt(mem);
	if(file_output_flag == true){
		fclose(output);
	}
	return 0;
}