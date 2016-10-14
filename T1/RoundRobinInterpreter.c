#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "scheduler.h"
#include "utils.h"


#define PROGRAM_LIMIT 50// Suporta uma entrada de até 50 programas com sintaxe VÁLIDA
#define CHAR_LIMIT 50

//TODO: SINCRONIZAR ESCALONADOR E INTERPRETADOR POR SEMÁFORO OU SIGNAL

//TODO: PASSAR AS FUNÇÕES AUXILIARES PARA UM UTIL.H

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
}

int main(int argc, char const *argv[])
{
	char command[CHAR_LIMIT], program[CHAR_LIMIT];
	char * newProgramsList[PROGRAM_LIMIT];
	int programCount=0, integrityValue, i;
	FILE *input, *output;

	printTime();
	printf("Interpretador Round-Robin:\n");
	// A entrada e a saída serão feitas por arquivo,
	// de modo a facilitar a avaliação e teste.
	input = fopen("input_robin.txt", "r");
	if(input == NULL){
		printTime();
		printf("Erro na abertura do arquivo de entrada.\n");
		exit(1);
	}
	output = fopen("output_robin.txt", "w");
	if(output == NULL){
		printTime();
		printf("Erro na abertura do arquivo de saída.\n");
		exit(1);
	}
	// Desviando o stdout. Caso prefira STDOUT, comente esta linha.
	//dup2(fileno(output), STDOUT_FILENO);

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

	for(i=0;i<programCount;i++){
		printf("%s\n", newProgramsList[i]);
	}
	fclose(input);
	if (programCount != 0){
		printTime();
		printf("Enviando programas ao escalonador round-robin..\n");
		roundRobinScheduler(newProgramsList, programCount);
	}
	else{
		printTime();
		printf("Nenhuma entrada válida foi detecatada. O escalonador não será acionado.\n");
	}
	fclose(output); 
	return 0;
}