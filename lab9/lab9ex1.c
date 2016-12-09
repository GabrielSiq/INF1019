#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>

#define MAXFILA 8
#define MSGMAX 50
#define ITEMS 20 /* Altere aqui para mudar o número de itens a serem produzidos/consumidos */

// Estrutura de buffer para mensagens
// Será usada para ambos os exercícios e contém um id de tipo que ajudará
// a discernir entre os processos futuramente
// Sua implementação foi sugerida aqui: https://linux.die.net/man/2/msgsnd

typedef struct msgbuf {
long    mtype;
char    mtext[MSGMAX];
} message_buf;

int message_count = 0, msqid;

void * produtor(){
	int i;
	message_buf sbuf;
	size_t buf_size;
	for(i = 0; i < ITEMS; i++){
		if(message_count < MAXFILA){
			
			sbuf.mtype = 1;

			sprintf(sbuf.mtext, "Mensagem %d", i);

			buf_size = strlen(sbuf.mtext) + 1 ;

			if (msgsnd(msqid, &sbuf, buf_size, IPC_NOWAIT) < 0) {
				perror("msgsnd");
				exit(1);
			}
			
			printf("O produtor produziu: %s\n", sbuf.mtext);
			message_count++;
		}
		else{
			printf("Fila cheia!\n");
		}
		sleep(1);
	}
}

void * consumidor(void * id){
	int i;
	message_buf rbuf;
	while(i < ITEMS/2){
		if(message_count > 0){
			if (msgrcv(msqid, &rbuf, MSGMAX, 1, 0) < 0) {
				printf("Erro no consumidor %d\n", id);
				exit(1);
			}
			printf("O consumidor %d consumiu: %s\n",id, rbuf.mtext);
			message_count--;
			i++;
		}
		else{
			printf("Fila vazia\n");
		}
		sleep(2);
	}
}

int main()
{
	int i;
    key_t key = 2018;

    pthread_t threads[3];

    // Cria uma fila de mensagens com o ID que especificamos
	if ((msqid = msgget(key, IPC_CREAT | 0666 )) < 0) {
		perror("msgget");
		exit(1);
	}

	// Cria um produtor e dois consumidores numerados
	pthread_create(&threads[0], NULL, produtor, NULL);
	pthread_create(&threads[1], NULL, consumidor, (void *) 1);
	pthread_create(&threads[2], NULL, consumidor, (void *) 2);

	for (i = 0; i < 3; ++i)
	{
		pthread_join(threads[i], NULL);
	}

	pthread_exit(NULL);

	return 0;

}
