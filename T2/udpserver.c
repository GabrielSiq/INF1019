/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/xattr.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>

#define BUFSIZE 1024
#define PERALL 0777
#define ISDIR   0x4
#define ISFILE  0x8

struct stat info={0};

int numUsers = 6;
const char * existingUsers[] = {"user1", "user2", "user3", "user4", "user5", "user6"};
const int existingPasswords[] = {210723987853, 210723987854, 210723987854, 210723987855, 210723987856, 210723987854};
const char * validationTokens[6];

int currentUser;


/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

/* Validates user session token */
int validateUser(int userId, char * validationToken){
	if(strcmp(validationTokens[userId], validationToken) == 0){
		return 1;
	}
	return 0;
}

/* Generates a random string token */
void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

/* Handles user logout */
char * logoutHandler(int userId, char * validationToken){
	if(validateUser){
		free(validationTokens[userId]);
		printf("O usuário %s se deslogou.\n", existingUsers[userId]);
		return "";
	}
}

/* Handles user login */
char * loginHandler(char * username, char * password){
	char * buf = (char *) malloc(80 * sizeof(char));
	for(int i = 0; i < numUsers; i++){
		if(strcmp(username, existingUsers[i]) == 0){
			if(atoi(password) == existingPasswords[i]){
				validationTokens[i] = (char *) malloc(12 * sizeof(char));
				rand_str(validationTokens[i], 12);
				sprintf(buf, "login|%d|%s", i, validationTokens[i]);
				return buf;
			}
		}
	}
	return "erro|Login ou senha inválidos";
}

/* Extrai de um path o nome do arquivo txt */
char *getName(char *p)
{
  int i;

  for(i = strlen(p); p[i] != '/'; i--)
  {}

  return p + i + 1;

}
/* Lê uma quantidade de bytes de um arquivo a partir de um offset */
char * readFile(char * path, int nrbytes, int offset)
{
  int fd, param, tam, type;
  char payload[BUFSIZE], * str;
  struct stat path_stat;
  struct passwd * pw;
  
  printf("\n\nOperacao: leitura de %d bytes do arquivo >>%s<< a partir do offset %d.\n", nrbytes, getName(path), offset );

  stat(path, &path_stat);
  if(!S_ISREG(path_stat.st_mode)){
  	if(S_ISDIR(path_stat.st_mode)){
  		str = malloc(100);
  		pw = getpwuid(path_stat.st_uid);
  		sprintf(str, "\nO path passado corresponde a um diretório, cujo autor é %s.\n", pw->pw_name);
  		return str;
  	}
  	else{
  		return "\nO path passado não contém um arquivo.\n";
  	}
  }
  
  fd = open(path, 0, PERALL);// acusa erro caso nao exista

  if(fd == -1)
  {
    return "\nErro no comando 'read', verifique se o arquivo existe.\n";
  }

  tam = lseek(fd, 0, SEEK_END);

  if(nrbytes <= tam) // testa se nrbytes eh maior que o arquivo
    tam = nrbytes;

  lseek(fd, offset, SEEK_SET);

  param = read(fd, payload, tam);

  payload[tam] = '\0';

  if(!param)
  { 
    printf("\nLeitura vazia, se for o caso, verifique offset\n");
    *payload = '\0';
    close(fd);
    return "Você chamou a função que lê arquivos.";
  }


  close(fd);

  printf("\nString lida:%s\n", payload);

  return "Você chamou a função que lê arquivos.";
}

/* Escreve uma quantidade de bytes de um arquivo a partir de um offset */
char * writeFile(char * path, char * payload, int nrbytes, int offset, int ownerPerm, int otherPerm)
{ 
  int fd;

  printf("\n\nOperacao: escrita da string >>%s<< no arquivo >>%s<< a partir do offset %d.\n", payload, getName(path), offset);


  if(nrbytes == 0)
  {
    unlink(path);
    return "Você chamou a função que escreve em arquivos com nrbytes=0, seu arquivo foi apagado";
  }

  fd = open(path, O_CREAT|O_RDWR, PERALL);// cria se nao existir e seta a permissao libero geral

  if(fd == -1)
  {
      return "\nErro no comando 'write', parametros inválidos\n";
  }

  if(offset <= (lseek(fd, 0, SEEK_END))) // testa se offset eh maior que o arquivo
    lseek(fd, offset, SEEK_SET);

  write(fd, payload, nrbytes);


  close(fd);

  return "Você chamou a função que escreve path arquivos";
}

/* Extrai as informações de um arquivo ou pasta */
char * fileInfo(char * path)
{
  stat(path, &info);
  struct passwd *pw;
  struct group * gr;

  printf("\n\nOperacao: extracao das informacoes sobre o path passado %s.\n", path);

  pw = getpwuid(info.st_uid);
  gr = getgrgid(info.st_gid);

  if((info.st_mode & S_IFMT) == S_IFDIR)
  {
    printf("\nVoce esta consultado as informações sobre uma pasta\n");
    printf("\nOwnwer: %s e Group: %s\n", pw->pw_name, gr->gr_name);
    return "Você chamou a função que dá informações sobre arquivos";
  }

  printf("\nInformações do arquivo: %s\n", getName(path));
  printf("Owner: %s e Group: %s \n", pw->pw_name, gr->gr_name);
  printf("Tamanho: %ld bytes\n\n", (long) info.st_size);


  return "Você chamou a função que dá informações sobre arquivos";
}

/* Cria um subdiretório no path indicado */
char * makdir(char * path, char * dirname, int ownerPerm, int otherPerm)
{
  char mk[BUFSIZE];

  printf("\n\nOperação: criar um diretório de nome >>%s<< em %s.\n", dirname, path);

  strcpy(mk, path);
  strcat(mk, "/");
  strcat(mk, dirname);

  if((mkdir(mk, PERALL)) == -1)
  {
    printf("\nErro na criacao do diretorio >>%s<<, verifique.\n", dirname);
    return 0;
  }

  printf("\nDiretório criado.\n");


  return "Você chamou a função que cria diretórios";
}

/* Remove um dado diretório */
char * rm(char * path, char * dirname) 
{
  char mk[BUFSIZE];

  strcpy(mk, path);
  strcat(mk, dirname);

  printf("\n\nOperação: remover o diretório de nome >>%s<< em %s.\n", dirname, path);
  
  if( (rmdir(mk)) == -1)
  {
    return "\nErro ao tentar remover o diretório, verifique se o mesmo esta vazio\n";
  }

  return "Você chamou a função que remove diretórios";
}

/* Lista todos os arquivos e diretórios a partir de um dado path */
char * list(char * path)
{
  DIR* dir; 
  struct dirent* entrada; 
  dir = opendir( path );

  printf("\n\nOperacao: listar todos os arquivos e diretórios em >>%s<<.\n", path); 


  if( !dir )
  {   
    return "\nErro ao tentar listar no path fornecido, verifique \n."; 
  } 

  entrada = readdir( dir); 

  while( entrada)
  {   
   
    if (entrada->d_type == ISFILE)
    {
      if(entrada->d_name[0] != '.')
        printf("\nA: %s", entrada->d_name);
    }
    else
      if(entrada->d_type == ISDIR && (strcmp(entrada->d_name, ".")!=0 && strcmp(entrada->d_name, "..") != 0))
        printf("\nD: %s", entrada->d_name);

    entrada = readdir( dir);  
  }

  printf("\n");

  closedir( dir );  

  return "Você chamou a função que lista arquivos no diretório";
}

/* Routes the message to the appropriate function depending on the base command */
/* Base command validation is done exclusively client-side (meaning, not here) 
   On the other hand, parameter validation is done exclusively server side (meaning, here)*/ 

int functionRouter (char *command) 
{
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

  	if(strcmp(mainCommand, "login") == 0 && n == 3){
		strcpy(command, loginHandler(params[1], params[2]));
		return 0;
	}
  	else if(!validateUser(atoi(params[n-2]), params[n-1])){
		strcpy(command, "Erro de validação do usuário.");
		return -2;
	}

	currentUser = atoi(params[n-2]);
	
	if(strcmp(mainCommand, "read") == 0 && n == 6){
		strcpy(command, readFile(params[1], atoi(params[2]), atoi(params[3])));
	}
	else if(strcmp(mainCommand, "write") == 0 && n == 9){
		strcpy(command, writeFile(params[1], params[2], atoi(params[3]),atoi(params[4]), atoi(params[5]), atoi(params[6])));
	}
	else if(strcmp(mainCommand, "info") == 0 && n == 5){
		strcpy(command, fileInfo(params[1]));
	}
	else if(strcmp(mainCommand, "mkdir") == 0 && n == 7){
		strcpy(command, makdir(params[1], params[2], atoi(params[3]),atoi(params[4])));
	}
	else if(strcmp(mainCommand, "rm") == 0 && n == 5){
		strcpy(command, rm(params[1], params[2]));
	}
	else if(strcmp(mainCommand, "list") == 0 && n == 5){
		strcpy(command, list(params[1]));
	}
	else if(strcmp(mainCommand, "quit") == 0 && n == 5){
		strcpy(command, logoutHandler(atoi(params[1]), params[2]));
	}
	else{
		strcpy(command, "Erro nos parâmetros. Por favor, verifique a documentação dos comandos.");
		return -1;
	}
	return 0;
}


int main(int argc, char **argv) 
{
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
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));

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
  if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
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
  	n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
  	if (n < 0)
  		error("ERROR in recvfrom");

    fflush(stdout);

  	functionRouter(buf);
    printf("\n%s\n",buf);

    printf("\n");
	/* 
	 * gethostbyaddr: determine who sent the datagram
	 */
  	hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
  	if (hostp == NULL)
  		error("ERROR on gethostbyaddr");
  	hostaddrp = inet_ntoa(clientaddr.sin_addr);
  	if (hostaddrp == NULL)
  		error("ERROR on inet_ntoa\n");
  	printf("server received datagram from %s (%s)\n",hostp->h_name, hostaddrp);
  	//printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);

	/* 
	 * sendto: echo the input back to the client 
	 */
  	n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);
  	if (n < 0) 
  		error("ERROR in sendto");
  }
}
