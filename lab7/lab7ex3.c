#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int buffer[8];
int head = 0; // index do primeiro espaço ocupado
int tail = 0; // index do proximo espaço vazio
int num_elem = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 



void * produtor(){
	int i = 0;
	while(i < 64){
		sleep(1);
		if(num_elem < 8){
			// "prende" o buffer para uso
			pthread_mutex_lock(&mutex);
			buffer[tail] = rand() % 64;
			printf("Produtor: acrescentei o numero %d\n", buffer[tail]);
			tail = (tail+1) % 8;
			num_elem++;
			i++;
			// libera o buffer pra uso
			pthread_mutex_unlock(&mutex);
		}
		else{
			printf("Produtor: O buffer está cheio\n");
		}
	}
}

void * consumidor(){
	int i;
	while(i < 64){
		sleep(2);
		if(num_elem > 0){
			pthread_mutex_lock(&mutex);
			printf("Consumidor: Consumi o numero %d\n", buffer[head]);
			head = (head+1) % 8;
			num_elem--;
			i++;
			pthread_mutex_unlock(&mutex);
		}
		else{
			printf("Consumidor: O buffer está vazio\n");
		}

	}
}

int main(int argc, char const *argv[])
{
	pthread_t threads[2];

	pthread_create(&threads[0], NULL, produtor, NULL);
	pthread_create(&threads[1], NULL, consumidor, NULL);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	return 0;
}