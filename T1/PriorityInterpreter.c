#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_LIMIT 50// Suporta uma entrada de até 50 programas com sintaxe VÁLIDA
#define CHAR_LIMIT 50


//TODO: FAZER CHECAGEM DE INTEGRIDADE EM FUNÇÕES SEPARADAS
//TODO: FAZER CHECAGEM DE EXISTÊNCIA DOS PROGRAMAS PARA TESTE
//TODO: PASSAR AS COISAS POR MEMÓRIA COMPARTILHADA
//TODO: SINCRONIZAR ESCALONADOR E INTERPRETADOR POR SEMÁFORO OU SIGNAL

//TODO:

int main(int argc, char const *argv[])
{
	char command[CHAR_LIMIT], program[CHAR_LIMIT], priority[CHAR_LIMIT], priorityIntegrity[9];
	char * newProgramsList[PROGRAM_LIMIT];
	int programCount=0, i, priorityList[PROGRAM_LIMIT], priorityValue;
	FILE *input, *output;

	printf("Interpretador de Prioridade:\n");
	// A entrada e a saída serão feitas por arquivo,
	// de modo a facilitar a avaliação e teste.
	input = fopen("input_priority.txt", "r");
	if(input == NULL){
		printf("Erro na abertura do arquivo de entrada.\n");
		exit(1);
	}
	output = fopen("output_priority.txt", "w");
	if(output == NULL){
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
		strncpy(priorityIntegrity, priority, 9);
		priorityIntegrity[9] = 0;

		if(strcmp(command, "exec") != 0){
			printf("O comando %s na linha %d não é reconhecido como um comando válido. A linha será ignorada.\n", command, i);
		}
		else if(strcmp(priorityIntegrity, "priority=") != 0){
			printf("O argumento %s na linha %d não é reconhecido como um argumento de prioridade válido. A linha será ignorada.\n", priority, i);
		}
		else{
			priorityValue = (int) priority[9] - '0';
			if(priorityValue > 7 || priorityValue < 0){
				printf("O valor %d não é um valor de prioridade válido (de 0 a 7).\n", priorityValue);
			}
			else{
				printf("O programa %s teve sintaxe válida e entrará na lista de novos programas com prioridade %d.\n", program, priorityValue);
				strcpy(newProgramsList[programCount], program);
				priorityList[programCount] = priorityValue;
				programCount++;
			}
		}
		i++;
	}
	fclose(input);
	for(i=0;i<programCount;i++){
		printf("%s %d\n", newProgramsList[i], priorityList[i]);
	}
	printf("Enviando programas ao escalonador de prioridade...\n");
	//priorityScheduler(newProgramsList, programCount);
	fclose(output); 
	return 0;
}