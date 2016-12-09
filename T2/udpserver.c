/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

char * readFile(char * path, int nrbytes, int offset){
	printf("Devo ler %d bytes do arquivo %s a partir do offset %d.\n", nrbytes, path, offset );
	return "Você chamou a função que lê arquivos";
}

char * writeFile(char * path, char * payload, int offset){
	printf("Devo escrever a string %s no arquivo %s a partir do offset %d.\n", payload, path, offset);
	return "Você chamou a função que escreve arquivos";
}

char * fileInfo(char * path){
	printf("Devo dar informações sobre o arquivo %s.\n", path);
	return "Você chamou a função que dá informações sobre arquivos";
}

char * mkdir(char * path, char * dirname){
	printf("Devo criar um diretório de nome %s em %s.\n", dirname, path);
	return "Você chamou a função que cria diretórios";
}

char * rm(char * path, char * dirname){
	printf("Devo remover o diretório de nome %s em %s.\n", dirname, path);
	return "Você chamou a função que remove diretórios";
}

char * list(char * path){
	printf("Devo listar todos os arquivos e diretórios em %s.\n", path);
	return "Você chamou a função que lista arquivos no diretório";
}

//empty string

/* Routes the message to the appropriate function depending on the base command */
/* Base command validation is done exclusively client-side (meaning, not here) 
   On the other hand, parameter validation is done exclusively server side (meaning, here)*/ 
int functionRouter (char *command) {
	char * mainCommand, buf[BUFSIZE];
	char * params[10];
	int n = 0;

	strcpy(buf, command);

	for (char * p = strtok(buf, "|"); p; p = strtok(NULL, "|"))
	{
	    if (p == NULL)
	    {
	        break;
	    }
	    params[n++] = p;
	    printf("%s ", params[n-1]);
	}

	mainCommand = params[0];
	
	if(strcmp(mainCommand, "read") == 0 && n == 4){
		strcpy(command, readFile(params[1], atoi(params[2]), atoi(params[3])));
	}
	else if(strcmp(mainCommand, "write") == 0 && n == 4){
		strcpy(command, writeFile(params[1], atoi(params[2]), atoi(params[3])));
	}
	else if(strcmp(mainCommand, "info") == 0 && n == 2){
		strcpy(command, fileInfo(params[1]));
	}
	else if(strcmp(mainCommand, "mkdir") == 0 && n == 3){
		strcpy(command, mkdir(params[1], params[2]));
	}
	else if(strcmp(mainCommand, "rm") == 0 && n == 3){
		strcpy(command, rm(params[1], params[2]));
	}
	else if(strcmp(mainCommand, "list") == 0 && n == 2){
		strcpy(command, list(params[1]));
	}
	else{
		strcpy(command, "Erro nos parâmetros. Por favor, verifique a documentação dos comandos.");
		return -1;
	}
	return 0;
}
int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
	
  char name[BUFSIZE];   // name of the file received from client
  int cmd;              // cmd received from client

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
  	fprintf(stderr, "usage: %s <port>\n", argv[0]);
  	exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
  	error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
  	(const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
  	sizeof(serveraddr)) < 0) 
  	error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {

	/*
	 * recvfrom: receive a UDP datagram from a client
	 */
  	bzero(buf, BUFSIZE);
  	n = recvfrom(sockfd, buf, BUFSIZE, 0,
  		(struct sockaddr *) &clientaddr, &clientlen);
  	if (n < 0)
  		error("ERROR in recvfrom");

  	functionRouter(buf);

	/* 
	 * gethostbyaddr: determine who sent the datagram
	 */
  	hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
  		sizeof(clientaddr.sin_addr.s_addr), AF_INET);
  	if (hostp == NULL)
  		error("ERROR on gethostbyaddr");
  	hostaddrp = inet_ntoa(clientaddr.sin_addr);
  	if (hostaddrp == NULL)
  		error("ERROR on inet_ntoa\n");
  	printf("server received datagram from %s (%s)\n", 
  		hostp->h_name, hostaddrp);
  	//printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);

	/* 
	 * sendto: echo the input back to the client 
	 */
  	n = sendto(sockfd, buf, strlen(buf), 0, 
  		(struct sockaddr *) &clientaddr, clientlen);
  	if (n < 0) 
  		error("ERROR in sendto");
  }
}
