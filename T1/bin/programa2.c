#include <stdio.h>

int main(void) {
	float var;
	int i = 0;
	while(1) {
		var = 5+4+3+2+1;
		var *= 0.2;
		if(i > 200000000){
			return 0;
		}
		i++;
	}
}
