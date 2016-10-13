#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int setSemValue(int semId);

void delSemValue(int semId);

int semaforoOp(int semId, int semOp);

int main(int argc, char const *argv[])
{
	char * msg;
	int semId, buffer, aux;


	if(argc > 1){
		/* Programa 1 */

		// Nossas mensagens terão limite de 100 chars
		buffer = shmget(6666, 100 * sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
		msg = (char *) shmat(buffer, 0, 0);
		// Criamos o semáforo para sincronização
		semId = semget(6667, 1, 0666 | IPC_CREAT);
		setSemValue(semId);
		// Entra na região crítica, salva a mensagem e sai.
		semaforoOp(semId, 0);
		printf("Mensagem do processo 1 para o processo 2:\n");
		fflush(stdout);
		scanf(" %[^\n]", msg);
		semaforoOp(semId, 2);
		while(1){
			// Suspende execução até o valor do semáforo ser zero novamente.
			semaforoOp(semId, 0);
			printf("O processo 1 leu a mensagem \"%s\" do processo 2.\n", msg);
			fflush(stdout);
			// E então escreve uma nova mensagem, salva e sai.
			printf("Nova mensagem do processo 1 para o processo 2:\n");
			fflush(stdout);
			scanf(" %[^\n]", msg);
			semaforoOp(semId, 2);
		}
	}
	else{
		/* Processo 2 */

		// Aguarda criação do semáforo
		while((semId = semget(6667, 1, 0666)) < 0){
			putchar('.');
			fflush(stdout);
			sleep(1);
		}
		// Mesmo procedimento, assumindo que a memória foi alocada pelo processo anterior
		buffer = shmget(6666, 100*sizeof(char), IPC_EXCL | S_IRUSR | S_IWUSR);
		msg = (char *) shmat(buffer, 0, 0);
		while(1){
			// Suspende execução até o valor do semáforo ser >=1 e então subtrai 1
			if(semaforoOp(semId, -1) == -1){
				exit(0);
			}
			printf("O processo 2 leu a mensagem \"%s\" do processo 1.\n", msg);
			fflush(stdout);
			printf("Nova mensagem do processo 2 para o processo 1:\n");
			fflush(stdout);
			sleep(2);
			strcpy(msg, "Mensagem Padrão.");
			// Retorna o valor a zero.
			semaforoOp(semId, -1);
		}
	}
	return 0;
}

int setSemValue(int semId)
{
    union semun semUnion;
    semUnion.val = 0;
    return semctl(semId, 0, SETVAL, semUnion);
}
void delSemValue(int semId)
{
    union semun semUnion;
    semctl(semId, 0, IPC_RMID, semUnion);
}
int semaforoOp(int semId, int semOp)
{
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = semOp;
    semB.sem_flg = SEM_UNDO;
    return semop(semId, &semB, 1);
}