#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <termios.h>

#define BUFSIZE 1024
#define SINGLEWORDSIZE 50
#define true 1
#define false 0

int isLoggedIn = false;

int serverlen, sockfd;
struct sockaddr_in serveraddr;


const char * reserved = "|";
const char * existingCommands[] = {"read", "write", "info", "mkdir", "rm", "list", NULL};

char validationToken[12]; //session token
int userId = 1; //user id

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

/* Receives message from server */
void receiveMessage(char * message){
	int n;
	n = recvfrom(sockfd, message, BUFSIZE, 0, &serveraddr, &serverlen);
	if (n < 0) 
		error("ERROR in recvfrom");
}

/* Sends message to server */
void sendMessage(char * message){
	int n;
	/* send the message to the server */
    int serverlen = sizeof(serveraddr);
    n = sendto(sockfd, message, strlen(message), 0, &serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");    
}

/* djb2 hash */
unsigned long hash(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c;

        return hash;
    }

/* Handles our simple login emulation */
void userLogin(){
	char username[80];
	char password[80];
	char buf[BUFSIZE];
	char * params[10];
	int n = 0;

	printf("Usuário: ");
	scanf(" %[^\n]1024s", username);
	if(strcmp(username, "quit") == 0){
		quit();
	}
	printf("Senha: ");
	scanf(" %[^\n]1024s", password);
	sprintf(buf, "login|%s|%ld", username, hash(password));
	sendMessage(buf);
	bzero(buf, BUFSIZE);
	receiveMessage(buf);

	for (char * p = strtok(buf, "|"); p; p = strtok(NULL, "|"))
	{
	    if (p == NULL)
	    {
	        break;
	    }
	    params[n++] = p;
	}

	if(strcmp(params[0], "login") == 0){
		printf("Login bem sucedido. Bem vindo %s!\n", username);
		userId = atoi(params[1]);
		strcpy(validationToken, params[2]);
		isLoggedIn = true;
	}
	else if(strcmp(params[0], "erro") == 0){
		printf("Usuário ou senha incorretos. Tente novamente ou crie uma conta.\n", username);
	}
}

/* Gets input from user */
void getInput(char * buf){
	printf("Digite um comando: ");
    scanf(" %[^\n]1024s", buf);
   	setbuf(stdin, NULL);
}

/* Exits program */
void quit(){
	char buf[BUFSIZE];

	// Sends logout request to deactivate session token
	if(isLoggedIn == true){
		sprintf(buf, "quit|%d|%s", userId, validationToken);
		sendMessage(buf);
	}
	printf("\nAté breve!\n\n");
	printf("	  /)─―ヘ\n");
	printf("　　　＿／　　　＼\n");
	printf("　 ／　　　　●　 ●丶\n");
	printf("　｜　　　　　  ▼　|\n");
	printf("　｜　　　　　　亠ノ\n");
	printf("　 U￣U￣￣￣￣U￣\n");
	exit(1);

}

/* Prints our friendly help page */
void printHelp(){
	printf("-------------------//-----------------\n");
	printf("Olá, bem vindo ao nosso sistema de ajuda.\n");
	printf("Utilizar nosso sistema de arquivos é bem simples. Basta digitar o comando desejado e os parâmetros corretos. As opções de comando e parâmetros estão detalhadas a seguir:\n\n");

	printf("1- read => lê um numero de bytes de um determinado arquivo a partir de um offset.\n");
	printf("   Esqueleto: read path nrbytes offset\n");
	printf("   Exemplo de uso: read /user/arquivo.txt 20 7\n\n");

	printf("2- write => escreve um conteúdo em um determinado arquivo a partir de um offset. Pode ser usado para remover um arquivo ao enviar uma string (payload) vazia ou criar um arquivo caso ele nao exista.\n");
	printf("   Esqueleto: write path payload nrbytes offset\n");
	printf("   Exemplo de uso: write /user/arquivo.txt \"laranjasebananas\" 16 7\n\n");

	printf("3- info => retorna informações sobre o arquivo escolhido.\n");
	printf("   Esqueleto: info path\n");
	printf("   Exemplo de uso: info /user/arquivo.txt\n\n");

	printf("4- mkdir => cria um novo subdiretório no local indicado.\n");
	printf("   Esqueleto: mkdir path dirname\n");
	printf("   Exemplo de uso: mkdir /user/ newfolder\n\n");

	printf("5- rm => remove o subdiretório especificado.\n");
	printf("   Esqueleto: rm path dirname\n");
	printf("   Exemplo de uso: rm /user/ oldfolder\n\n");

	printf("6- list => lista todos os arquivos e subdiretórios no local indicado.\n");
	printf("   Esqueleto: list path\n");
	printf("   Exemplo de uso: list /user/\n\n");

	printf("Viu como é fácil? Agora é sua vez!\n");
	printf("-------------------//-----------------\n");
}

/* Checks if the main command is one of the allowed commands */
int checkCommand(char * command){
	int i = 0;

	while( existingCommands[i] != NULL ){
		if(strcmp(command, existingCommands[i]) == 0){
			//printf("It's a match!\n");
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

	command[i]='\0';
	if(isLoggedIn == true){
		sprintf(command, "%s|%d|%s", command, userId, validationToken);
	}
	printf("HtM:%s\n", command);
}

/* Does basic checks on the input, preparing to send to the server */
/* Base command validation is done exclusively client-side (meaning, here) 
   On the other hand, parameter validation is done exclusively server side (meaning, not here)*/ 
char * inputValidation(char * command){
	char * mainCommand, buf[BUFSIZE];

	if(strstr(command, reserved) != NULL){
		printf("Seu comando contém a substring reservada \"%s\" e nao é permitido por nosso sistema. Tente novamente.\n", reserved);
		return -3;
	}

	strcpy(buf, command);
	mainCommand = strtok(buf, " \n\t");
	
	if(checkCommand(mainCommand) == 0){
		//printf("Comando valido!\n");
		humanReadableToMachine(command);
		return 0;
	}
	else if(strcmp(mainCommand, "help") == 0){
		printHelp();
		return -1;
	}
	else if(strcmp(mainCommand, "quit") == 0){
		quit();
	}
	else{
		printf("Não foi reconhecido um comando válido. Digite \"help\" se precisar de ajuda\n");
		return -2;
	}
}

int main(int argc, char **argv) {
    int portno, n;
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
    printf("Bem vindo ao nosso servidor de arquivos! Digite seu usuário e senha ou \"quit\" para sair.\n");
    while(isLoggedIn == false){
    	userLogin(buf);
    }

    printf("Bem vindo ao nosso servidor de arquivos! Digite um comando ou digite \"help\" para obter ajuda ou \"quit\" para sair.\n");

    while(true){
     	getInput(buf);

    	//fgets(buf, BUFSIZE, stdin);
    	if(inputValidation(buf) == 0){

    		/* send the message to the server */
		    sendMessage(buf);
		    
		    /* print the server's reply */
		    receiveMessage(buf);
		    
		  	printf("Server: %s\n", buf);

		  	bzero(buf, BUFSIZE); // limpa a mensagem pra nao truncar mensagens anteriores
		}
    }
    return 0;
}