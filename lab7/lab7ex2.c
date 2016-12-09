#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int contador = 0;

void * contCrescente(){
	for (int i = 0; i < 20; ++i)
	{
		sleep(1);
		contador += 1;
		printf("Thread 1 - Contagem Crescente incrementou o contador para %d\n", contador);
	}
}

void * contDescrecente(){
	for (int i = 30; i > 0; --i)
	{
		sleep(2);
		contador += 1;
		printf("Thread 2 - Contagem Decrescente incrementou o contador para  %d\n", contador);

	}
}

int main(int argc, char const *argv[])
{
	pthread_t threads[2];

	pthread_create(&threads[0], NULL, contCrescente, NULL);
	pthread_create(&threads[1], NULL, contDescrecente, NULL);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	return 0;
}