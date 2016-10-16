#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
	int i = 0;
	while(i<15){
		sleep(1);
		printf("Executando o programa 4\n");
		if(i == 2){
			kill(getppid(), SIGINT);
		}
		i++;
	}
	return 0;
}
