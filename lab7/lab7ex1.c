#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void * contCrescente(){
	for (int i = 0; i < 20; ++i)
	{
		printf("Thread 1 - Contagem Crescente: %d\n", i);
		sleep(1);
	}
}

void * contDescrecente(){
	for (int i = 30; i > 0; --i)
	{
		printf("Thread 2 - Contagem Decrescente: %d\n", i);
		sleep(2);
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