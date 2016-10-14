#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	FILE * fp = fopen("teste.txt", "w");
	fprintf(fp, "Programa 1\n");
	fclose(fp);
	return 0 ;
	while(1){
		sleep(1);
		fprintf(fp, "Executando o programa 1\n");
	}
	return 0;
}
