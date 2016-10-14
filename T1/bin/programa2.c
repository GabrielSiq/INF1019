#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	printf("Programa 2\n");
	while(1){
		sleep(1);
		printf("Executando o programa 2\n");
	}
	return 0;
}