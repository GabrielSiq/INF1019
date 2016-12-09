#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUMTHREADS 4
#define LIMIT 20


int buffer[8];
int head = 0; // index do primeiro espaço ocupado
int tail = 0; // index do proximo espaço vazio
int num_elem = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t production_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consume_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t product = PTHREAD_COND_INITIALIZER;
pthread_cond_t consume = PTHREAD_COND_INITIALIZER;

void* produtor(void* threadid) {
	int i;
	while(i < LIMIT)
	{
		sleep(1);
		if(num_elem < 8){
			// "prende" o buffer para uso
			pthread_mutex_lock(&mutex);
			buffer[tail] = rand() % LIMIT;
			printf("Produtor %d: Produzi %d.\n", threadid, buffer[tail]);
			tail = (tail+1) % 8;
			num_elem++;
			i++;
			// libera o buffer pra uso
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&consume);
		}
		else{
			printf("Produtor %d: Buffer cheio, serei suspenso.\n", threadid);
			pthread_mutex_lock(&production_mutex);
			pthread_cond_wait(&product, &production_mutex);
			pthread_mutex_unlock(&production_mutex);
			printf("Produtor %d: Fui liberado!\n", threadid);
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
			pthread_mutex_lock(&mutex);
			printf("Consumidor %d: Consumi %d.\n", threadid, buffer[head]);
			head = (head+1) % 8;
			num_elem--;
			i++;
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&product);
		}
		else{
			printf("Consumidor %d: Buffer vazio, serei suspenso.\n", threadid);
			pthread_mutex_lock(&consume_mutex);
			pthread_cond_wait(&consume, &consume_mutex);
			pthread_mutex_unlock(&consume_mutex);
			printf("Consumidor %d: Fui liberado!\n", threadid);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	pthread_t threads[NUMTHREADS];

	pthread_create(&threads[0], NULL, produtor, (void *)0);
	pthread_create(&threads[1], NULL, produtor, (void *)1);
	pthread_create(&threads[2], NULL, consumidor, (void *)2);
	pthread_create(&threads[3], NULL, consumidor, (void *)3);

	for (int i = 0; i < NUMTHREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	return 0;
}