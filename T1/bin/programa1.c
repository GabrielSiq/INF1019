#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int i = 0;
	while(i<15){
		sleep(1);
		i++;
	}
	return 0;
}
