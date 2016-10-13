#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>

int buffer, semId;
char *msg;

int setSemValue(int semId);

void delSemValue(int semId);

int semaforoP(int semId, int semOp);

union semun
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int main (int argc, char * argv[]){
    int i;
    char c;

    if (argc > 1) { 
        /* Processo Leitor */

        // memória compartilhada
        buffer = shmget (8731, 16 * sizeof (char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
        // attach a memória compartilhada
        msg = (char *) shmat(buffer,0,0);
        // Cria o semáforo para controle
        semId = semget (8732, 1, 0666 | IPC_CREAT);
        setSemValue(semId);

        // loop infinito
        while(1) {
            semaforoP(semId,0);
            i=0;
            while(i < 16){
                // captura os chars e coloca no buffer
                if((c = getchar()) != -1)
                    msg[i++] = c;
            }
            // ao atingir 16, modifica o valor do semáforo
            semaforoP(semId,1);
            
        }
    }
    else
    {
        /* Processo Impressor */

        // Espera ser criado o semáforo. Imprime "." enquanto isso.
        while ((semId = semget (8732, 1, 0666)) < 0)
        {
            putchar ('.'); fflush(stdout);
            sleep (1);
        }
        // memória compartilhada 
        buffer = shmget (8761, 16 * sizeof (char), IPC_EXCL | S_IRUSR | S_IWUSR);
        // attach a memória compartilhada
        msg = (char *) shmat(buffer,0,0);

        //loop infinito
        while (1) {
            printf("Processo Impressor: ");  fflush(stdout);
            for(i=0;i<16;i++){
                // imprime os 16 chars do buffer
                putchar(msg[i]); fflush(stdout);
            }
            semaforoP(semId,-1);
        }
    }
    delSemValue(semId);
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
int semaforoP(int semId, int semOp)
{
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = semOp;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
    
}
