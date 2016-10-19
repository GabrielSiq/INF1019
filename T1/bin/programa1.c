#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
	int i = 0;
	while(i<15){
		sleep(1);
		printf("Executando o programa 1\n");
		i++;
	}
	return 0;
}
