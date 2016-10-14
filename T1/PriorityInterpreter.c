#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Scheduler.h"
#include "Utils.h"

#define PROGRAM_LIMIT 50// Suporta uma entrada de até 50 programas com sintaxe VÁLIDA
#define CHAR_LIMIT 50

//TODO: SINCRONIZAR ESCALONADOR E INTERPRETADOR POR SEMÁFORO OU SIGNAL

//TODO: PASSAR AS FUNÇÕES AUXILIARES PARA UM UTIL.H

// Checa a integridade do comando de entrada
int integrityCheck(char * command, char * program, char * priority){
	char priorityIntegrity[9];
	int priorityValue;
	struct stat programCheck;

	strncpy(priorityIntegrity, priority, 9);
	priorityIntegrity[9] = 0;
	if(strcmp(command, "exec") != 0){
		return -1;
	}
	if(strcmp(priorityIntegrity, "priority=") != 0){
		return -2;
	}
	priorityValue = (int) priority[9] - '0';
	if(priorityValue > 7 || priorityValue < 0){
		return -3;
	}
	if(!(stat(program, &programCheck) == 0 && programCheck.st_mode & S_IXUSR)) {
		return -4;
	}
	return priorityValue;
}

int main(int argc, char const *argv[])
{
	char command[CHAR_LIMIT], program[CHAR_LIMIT], priority[CHAR_LIMIT], priorityIntegrity[9];
	char * newProgramsList[PROGRAM_LIMIT];
	int programCount=0, i, priorityList[PROGRAM_LIMIT], priorityValue;
	FILE *input, *output;

	printTime();
	printf("Interpretador de Prioridade:\n");
	// A entrada e a saída serão feitas por arquivo,
	// de modo a facilitar a avaliação e teste.
	input = fopen("input_priority.txt", "r");
	if(input == NULL){
		printTime();
		printf("Erro na abertura do arquivo de entrada.\n");
		exit(1);
	}
	output = fopen("output_priority.txt", "w");
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
			printf("O valor recebido na linha %d não é um valor de prioridade válido (de 0 a 7).\n", i);
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
	for(i=0;i<programCount;i++){
		printf("%s %d\n", newProgramsList[i], priorityList[i]);
	}
	if (programCount != 0){
		printTime();
		printf("Enviando programas ao escalonador round-robin..\n");
		priorityScheduler(newProgramsList, priorityList, programCount);
	}
	else{
		printTime();
		printf("Nenhuma entrada válida foi detecatada. O escalonador não será acionado.\n");
	}
	fclose(output); 
	return 0;
}