#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int i, pid1, pid2, pid3, pid4;
	pid1 = fork();
	if(pid1 != 0){
		pid2 = fork();
		if(pid2 != 0){
			pid3 = fork();
			if(pid3 != 0){
				pid4 = fork();
				if(pid4 !=0){
					for(;;){
					}
				}else{
					for(;;){
						puts("Sinal 4 aberto!\n");
						sleep(60);
						puts("Sinal 4 atencao!\n");
						sleep(20);
						puts("Sinal 4 fechado!\n");
						sleep(60);
						fflush(stdout);
					}
				}
			}
			else{
				for(;;){
					puts("Sinal 3 aberto!\n");
					sleep(50);
					puts("Sinal 3 atencao!\n");
					sleep(15);
					puts("Sinal 3 fechado!\n");
					sleep(60);
					fflush(stdout);
				}
			}
		}
		else{
			for(;;){
				puts("Sinal 2 aberto!\n");
				sleep(40);
				puts("Sinal 2 atencao!\n");
				sleep(10);
				puts("Sinal 2 fechado!\n");
				sleep(40);
				fflush(stdout);
			}
		}
	}
	else{
		for(;;){
			puts("Sinal 1 aberto!\n");
			sleep(30);
			puts("Sinal 1 atencao!\n");
			sleep(5);
			puts("Sinal 1 fechado!\n");
			sleep(30);
			fflush(stdout);
		}
	}
	return 0;
}