#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>

union semun
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};
// inicializa o valor do semáforo
int setSemValue(int semId);
// remove o semáforo
void delSemValue(int semId);
// operação P
int semaforoP(int semId);
//operação V
int semaforoV(int semId);

int main (int argc, char * argv[])
{
	int i;
	int semId, buffer, *mem;

	buffer = shmget (8762, sizeof(int), IPC_EXCL | S_IRUSR | S_IWUSR);
	mem = (int *) shmat(buffer,0,0);

	if(argc > 1){
		*mem = 0;
	}

	semId = semget (8763, 1, 0666 | IPC_CREAT); 
	setSemValue(semId);
	for (i=0; i<10; i++)
	{
		// entra na regiao crítica e incrementa a memoria
		semaforoP(semId);
		*mem+= 1;

		printf("Valor: %d", * mem);fflush(stdout);
		// sai da regiao critica
		semaforoV(semId);

		sleep (rand() %2);
	}
	shmdt(mem);
	shmctl(buffer, IPC_RMID, 0);
	return 0;
}

int setSemValue(int semId)
{
	union semun semUnion;
	semUnion.val = 1;
	return semctl(semId, 0, SETVAL, semUnion);
}
void delSemValue(int semId)
{
	union semun semUnion;
	semctl(semId, 0, IPC_RMID, semUnion);
}
int semaforoP(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}
int semaforoV(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}