//TODO: Remover os printfs desnecessários
/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024
#define SINGLEWORDSIZE 50
#define true 1
#define false 0

const char * reserved = "|";
const char * existingCommands[] = {"read", "write", "info", "mkdir", "rm", "list", NULL};

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

/* Prints our friendly help page */
void printHelp(){
	printf("-------------------//-----------------\n");
	printf("Ola, bem vindo ao nosso sistema de ajuda.\n");
	printf("Utilizar nosso sistema de arquivos eh bem simples. Basta digitar o comando desejado e os parâmetros corretos. As opções de comando e parâmetros estão detalhadas a seguir:\n\n");

	printf("1- read => le um numero de bytes de um determinado arquivo a partir de um offset. Também pode ser usada para criar um arquivo caso ele não exista.\n");
	printf("   Esqueleto: read path nrbytes offset\n");
	printf("   Exemplo de uso: read /user/arquivo.txt 20 7\n\n");

	printf("2- write => escreve um conteudo em um determinado arquivo a partir de um offset. Pode ser usado para remover um arquivo ao enviar uma string (payload) vazia.\n");
	printf("   Esqueleto: write path payload offset\n");
	printf("   Exemplo de uso: write /user/arquivo.txt \"laranjas e bananas\" 7\n\n");

	printf("3- info => retorna informacoes sobre o arquivo escolhido.\n");
	printf("   Esqueleto: info path\n");
	printf("   Exemplo de uso: info /user/arquivo.txt\n\n");

	printf("4- mkdir => cria um novo subdiretório no local indicado.\n");
	printf("   Esqueleto: mkdir path dirname\n");
	printf("   Exemplo de uso: mkdir /user/ newfolder\n\n");

	printf("5- rm => remove o subdiretorio especificado.\n");
	printf("   Esqueleto: rm path dirname\n");
	printf("   Exemplo de uso: rm /user/ oldfolder\n\n");

	printf("6- list => lista todos os arquivos e subdiretorios no local indicado.\n");
	printf("   Esqueleto: list path\n");
	printf("   Exemplo de uso: list /user/\n\n");

	printf("Viu como eh facil? Agora eh sua vez!\n");
	printf("-------------------//-----------------\n");
}

/* Checks if the main command is one of the allowed commands */
int checkCommand(char * command){
	int i = 0;

	while( existingCommands[i] != NULL ){
		if(strcmp(command, existingCommands[i]) == 0){
			printf("It's a match!\n");
			return 0;
		}
		i++;
	}
	return -1;
}

/* Translates the human readable input into strcutured code for the server */
void humanReadableToMachine(char * command){
	int i = 0;

	while(command[i] != '\n'){
		if(command[i] == ' ')
			command[i] = reserved[0];
		i++;
	}
	printf("HtM:%s\n", command);
}

/* Does basic checks on the input, preparing to send to the server */
char * inputValidation(char * command){
	char * mainCommand, buf[BUFSIZE];

	if(strstr(command, reserved) != NULL){
		printf("Seu comando contem a substring reservada \"%s\" e nao eh permitido por nosso sistema. Tente novamente.\n", reserved);
		return -3;
	}

	strcpy(buf, command);
	mainCommand = strtok(buf, " \n\t");
	
	if(checkCommand(mainCommand) == 0){
		printf("Comando valido!\n");
		humanReadableToMachine(command);
		return 0;
	}
	else if(strcmp(mainCommand, "help") == 0){
		printHelp();
		return -1;
	}
	else if(strcmp(mainCommand, "quit") == 0){
		printf("Ate breve!\n");
		exit(0);
	}
	else{
		printf("Nao foi reconhecido um comando valido. Digite \"help\" se precisar de ajuda\n");
		return -2;
	}
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    printf("Bem vindo ao nosso servidor de arquivos! Digite um comando ou digite \"help\" para obter ajuda ou \"quit\" para sair.\n");

    do {
    	printf("Digite um comando: ");
    	fgets(buf, BUFSIZE, stdin);
    }
    while(inputValidation(buf) != 0);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply */
    n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
    if (n < 0) 
      error("ERROR in recvfrom");
    printf("Echo from server: %s", buf);
    return 0;
}