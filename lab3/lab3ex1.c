#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int i, pid1, pid2;

	pid1 = fork();
	if(pid1 == 0) {
		while(1) {
			printf("Filho 1\n");
			sleep(1);
		}
	}
	else {
		pid2 = fork();
		if(pid2 == 0) {
			while(1) {
				printf("Filho 2\n");
				sleep(1);
			}				
		}
		else {
			kill(pid2, SIGSTOP);
			for(i=0;i<10;i++) {
				if(i%2 == 0){
					kill(pid2, SIGSTOP);
					kill(pid1, SIGCONT);
					sleep(1);
				}
				else{
					kill(pid1, SIGSTOP);
					kill(pid2, SIGCONT);
					sleep(1);
				}
			}
			kill(pid1, SIGKILL);
			kill(pid2, SIGKILL);	
			exit(1);
			}
		}
	return 0;
}
