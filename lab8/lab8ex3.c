#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUMTHREADS 2
#define LIMIT 20


int buffer[8];
int head = 0; // index do primeiro espaço ocupado
int tail = 0; // index do proximo espaço vazio
int num_elem = 0;

int lock = 0;

void* produtor(void* threadid) {
	int i;
	while(i < LIMIT)
	{
		sleep(1);
		if(num_elem < 8){
			// "prende" o buffer para uso
			if(lock == 0){
				lock = 1;
				buffer[tail] = rand() % LIMIT;
				printf("Produtor %d: Produzi %d.\n", threadid, buffer[tail]);
				tail = (tail+1) % 8;
				num_elem++;
				i++;
				lock = 0;
			}
		}
		else{
			printf("Produtor %d: Buffer cheio, serei suspenso.\n", threadid);
		}
	}
	pthread_exit(NULL);
}

void* consumidor(void* threadid) {
	int i;
	while(i < LIMIT)
	{
		sleep(2);
		if(num_elem > 0){
			if(lock ==0){
				lock = 1;
				printf("Consumidor %d: Consumi %d.\n", threadid, buffer[head]);
				head = (head+1) % 8;
				num_elem--;
				i++;
				lock = 0;
			}
		}
		else{
			printf("Consumidor %d: Buffer vazio, serei suspenso.\n", threadid);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	pthread_t threads[NUMTHREADS];

	pthread_create(&threads[0], NULL, produtor, (void *)0);
	pthread_create(&threads[1], NULL, consumidor, (void *)1);

	for (int i = 0; i < NUMTHREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	return 0;
}