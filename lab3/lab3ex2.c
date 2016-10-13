#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


time_t start, end;


int cost(int duration){
	if(duration <= 60){
		return duration*2;
	}else{
		// 60*2 + duration - 60 = duration + 60
		return duration + 60;
	}
}

void callStartHandler(int signal){
	// Saves start time
	start = time(NULL);
}

void callEndHandler(int signal){
	// Saves end time
	end = time(NULL);

	// Calculates call duration
	int duration = (int) end-start;

	printf("This call lasted %d minutes and %d seconds.\n", duration/60, duration%60);
	printf("It's gonna cost %d cents. What a bargain!\n", cost(duration));
}

void main(){
	int pid;

	signal(SIGUSR1, callStartHandler);
	signal(SIGUSR2, callEndHandler);

	for(;;);
}



