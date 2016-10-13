#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	int fd[2];
	char dest[50];

	if(pipe(fd) <0){
		puts("Problemas na abertura dos pipes!");
		exit(-1);
	}

	if(fork() == 0){
		// filho

		// Redireciona stdout para o pipe
		close(fd[0]);
		dup2(fd[1], 1);
		// Echo escreve na stdout
		execl("/bin/echo", "/bin/echo", "Documents", NULL);
		close(fd[1]);
	}
	else{
		// pai
		close(fd[1]);
		// Redireciona stdin para o pipe
		dup2(fd[0], 0);
		scanf("%s", dest);
		// ls lê da stdin
		execl("/bin/ls", "ls", dest, NULL);
		close(fd[0]);
	}
	return 0;
}