#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
	int i = 0;
	while(i<15){
		sleep(1);
		i++;
		if(i == 3){
			kill(getppid(), SIGILL);
		}
	}
	return 0;
}
