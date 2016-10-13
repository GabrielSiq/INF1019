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

int main(int argc, char const *argv[])
{
	char command[CHAR_LIMIT], program[CHAR_LIMIT];
	char * newProgramsList[PROGRAM_LIMIT];
	int programCount=0, i;
	FILE *input, *output;
	printf("Interpretador Round-Robin:\n");
	// A entrada e a saída serão feitas por arquivo,
	// de modo a facilitar a avaliação e teste.
	input = fopen("input_robin.txt", "r");
	if(input == NULL){
		printf("Erro na abertura do arquivo de entrada.\n");
		exit(1);
	}
	output = fopen("output_robin.txt", "w");
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

	//Leitura dos comandos
	while(fscanf(input, "%s %s", command, program)  == 2){
		if(strcmp(command, "exec") != 0){
			printf("O comando %s na linha %d não é reconhecido como um comando válido. A linha será ignorada.\n", command, programCount);
		}
		else{
			printf("O programa %s teve sintaxe válida e entrará na lista de novos programas\n", program);
			strcpy(newProgramsList[programCount], program);
			programCount++;
		}
	}

	for(i=0;i<programCount;i++){
		printf("%s\n", newProgramsList[i]);
	}
	fclose(input);
	printf("Enviando programas ao escalonador round-robin..\n");
	//roundRobinScheduler(newProgramsList, programCount);
	fclose(output); 
	return 0;
}