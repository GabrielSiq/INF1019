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
#define MAX 65000 

struct stat info={0};

int numUsers = 6;
const char * existingUsers[] = {"user1", "user2", "user3", "user4", "user5", "user6"};
const int existingPasswords[] = {210723987853, 210723987854, 210723987855, 210723987856, 210723987857, 210723987858};
const char * validationTokens[6];
int currentUser;

const char * reserved = "|";
char *req;

char *readMachinetoHuman (char *buf, int tam)
{
  bzero(req, MAX);

  sprintf(req,"read%s%s%s%i",reserved,buf,reserved,tam); 
  
  return req;
}

char *writeMachinetoHuman (char *buf)
{
  bzero(req, MAX); 
  
  sprintf(req,"write%s%s",reserved,buf);  
  
  return req;
}

char *infoMachinetoHuman (char *buf1,char *buf2,char *buf3)
{
  bzero(req, MAX);

  sprintf(req,"info%s%s%s%s%s%s",reserved,buf1,reserved,buf2,reserved,buf3); 
   
  return req;
}

char *mkdirMachinetoHuman(char *buf)
{
  bzero(req, MAX); 
  
  sprintf(req,"mkdir%s%s",reserved,buf);  
  
  return req;
}

char *rmMachinetoHuman(char *buf)
{
  bzero(req, MAX); 
  
  sprintf(req,"rm%s%s",reserved,buf);  
  
  return req;
}

char *erros(char *buf)
{
  bzero(req, MAX);

  sprintf(req,"erro%s%s",reserved,buf);  

  return req;
}

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
				sprintf(buf, "login%s%d%s%s", reserved,i,reserved, validationTokens[i]);
				return buf;
			}
		}
	}
	return erros("Login ou senha inválidos");
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
  int fd, param, tam;
  char payload[BUFSIZE], * str;
  struct stat path_stat;
  struct passwd * pw;
  
  printf("\n\nOperacao: leitura de %d bytes do arquivo >>%s<< a partir do offset %d.\n", nrbytes, getName(path), offset );

  stat(path, &path_stat);

  if(!S_ISREG(path_stat.st_mode)){
  	if(S_ISDIR(path_stat.st_mode)){
  		str = malloc(100);
  		pw = getpwuid(path_stat.st_uid);
  		
      sprintf(str, "O path passado corresponde a um diretório, cujo autor é %s.", pw->pw_name);
  		return erros(str);
  	}
  	else{
  		return erros("O path passado não contém um arquivo.");
  	}
  }
  
  fd = open(path, 0, PERALL);// acusa erro caso nao exista

  if(fd == -1)
  {
    return erros("Erro no comando 'read', verifique se o arquivo existe.");
  }

  tam = lseek(fd, 0, SEEK_END);

  if(nrbytes <= tam) // testa se nrbytes eh maior que o arquivo
    tam = nrbytes;

  lseek(fd, offset, SEEK_SET);

  param = read(fd, payload, tam);

  if(!param)
  { 
    close(fd);
    return erros("Leitura vazia, se for o caso, verifique offset");
  }

  payload[tam] = '\0';

  close(fd);

  printf("\nString lida:%s\n", payload);

  return readMachinetoHuman(payload,tam);
}

void createFilePermission(char * path, int ownerPerm, int otherPerm){
	int fd;
	char * base, * basec, * dirc, * dname;
  	char hidden[80], hiddenPath[80], perms[10];

  	basec = strdup(path);
  	base = basename(basec);
  	dirc = strdup(path);
  	dname = dirname(dirc);

  	sprintf(hidden, ".%s", base);
	sprintf(hiddenPath, "%s/%s", dname, hidden);
	sprintf(perms, "%d %d %d", currentUser, ownerPerm, otherPerm);

	fd = open(hiddenPath, O_CREAT|O_RDWR, PERALL);
	write(fd, perms , sizeof(perms));
	close(fd);
}

int checkFilePermission(char * path, char * permFile){
	int fd;
	char * base, * basec, * dirc, * dname;
  char hidden[80], hiddenPath[80], buffer[10];

  basec = strdup(path);
  base = basename(basec);
  dirc = strdup(path);
  dname = dirname(dirc);

  sprintf(hidden, ".%s", base);
	sprintf(hiddenPath, "%s/%s", dname, hidden);
	fd = open(hiddenPath, O_CREAT|O_RDWR, PERALL);

	read(fd, buffer, 1);
	if(atoi(buffer) != currentUser){
		lseek(fd, 4, SEEK_SET);
	}
	else{
		lseek(fd, 2, SEEK_SET);
	}
	read(fd, buffer, 1);
	close(fd);
  strcpy(permFile, hiddenPath);
	return atoi(buffer);
}

int checkDirPermission(char * path, char * dirname, char * permFile){
	char hidden[80], hiddenPath[80], buffer[10];
	int fd;

	sprintf(hidden, ".%s", dirname);
  sprintf(hiddenPath, "%s/%s", path, hidden);
  	fd = open(hiddenPath, O_CREAT|O_RDWR, PERALL);
  	read(fd, buffer, 1);
	if(atoi(buffer) != currentUser){
		lseek(fd, 4, SEEK_SET);
	}
	else{
		lseek(fd, 2, SEEK_SET);
	}
	read(fd, buffer, 1);
	close(fd);
  strcpy(permFile, hiddenPath);
	return atoi(buffer);
}

/* Escreve uma quantidade de bytes de um arquivo a partir de um offset */
char * writeFile(char * path, char * payload, int nrbytes, int offset, int ownerPerm, int otherPerm)
{ 
  int fd;
  char permFile[80];
  
  printf("\n\nOperacao: escrita da string >>%s<< no arquivo >>%s<< a partir do offset %d.\n", payload, getName(path), offset);


  if( access( path, F_OK ) == -1 ) {
    createFilePermission(path, ownerPerm, otherPerm);
  }
  else{
    if(!checkFilePermission(path, permFile)){
      return erros("Você não tem permissão para escrever neste arquivo.");
    }
  }

  if(nrbytes == 0)
  {
    unlink(path);
    unlink(permFile);
    return writeMachinetoHuman("Você chamou a função que escreve em arquivos com nrbytes=0, seu arquivo foi apagado");
  }

  fd = open(path, O_CREAT|O_RDWR, PERALL);// cria se nao existir e seta a permissao libero geral

  if(fd == -1)
  {
      return erros("Erro no comando 'write', parametros inválidos");
  }

  if(offset <= (lseek(fd, 0, SEEK_END))) // testa se offset eh maior que o arquivo
    lseek(fd, offset, SEEK_SET);

  write(fd, payload, nrbytes);
  close(fd);

  return writeMachinetoHuman("Sua mensagem foi escrita.");
}
/* Extrai as informações de um arquivo ou pasta */
char * fileInfo(char * path)
{
  stat(path, &info);
  struct passwd *pw;
  struct group * gr;
  char buf1[BUFSIZE],buf2[BUFSIZE],buf3[BUFSIZE];

  printf("\n\nOperacao: extracao das informacoes sobre o path passado %s.\n", path);

  pw = getpwuid(info.st_uid);
  gr = getgrgid(info.st_gid);

  if((info.st_mode & S_IFMT) == S_IFDIR)
  {
    sprintf(buf1,"Voce esta consultado as informações sobre uma pasta");
    sprintf(buf2,"Ownwer: %s e Group: %s\n", pw->pw_name, gr->gr_name);
    sprintf(buf3,"O tamanho depende de seu conteudo, para mais informações use o list");
    return infoMachinetoHuman(buf1,buf2,buf3);
  }

  sprintf(buf1,"\nInformações do arquivo: %s\n", getName(path));
  sprintf(buf2,"Owner: %s e Group: %s \n", pw->pw_name, gr->gr_name);
  sprintf(buf3,"Tamanho: %ld bytes\n\n", (long) info.st_size);


  return infoMachinetoHuman(buf1,buf2,buf3);
}

/* Cria um subdiretório no path indicado */
char * makdir(char * path, char * dirname, int ownerPerm, int otherPerm)
{
  char mk[BUFSIZE];
  char hidden[80], hiddenPath[80], perms[10];
  int fd;

  printf("\n\nOperacao: criar um diretório de nome >>%s<< em %s.\n", dirname, path);

  strcpy(mk, path);
  strcat(mk, "/");
  strcat(mk, dirname);

  if((mkdir(mk, PERALL)) == -1)
  {
    sprintf(mk,"Erro na criacao do diretorio >>%s<<.", dirname);
    return erros(mk);
  }

  // create permission
  sprintf(hidden, ".%s", dirname);
  sprintf(hiddenPath, "%s/%s", path, hidden);
  sprintf(perms, "%d %d %d", currentUser, ownerPerm, otherPerm);

  fd = open(hiddenPath, O_CREAT|O_RDWR, PERALL);
  write(fd, perms, sizeof(perms));
  close(fd);

  sprintf(mk,"Diretório criado, novo path: %s.",mk);

  return mkdirMachinetoHuman(mk);
}

/* Remove um dado diretório */
char * rm(char * path, char * dirname) 
{
  char mk[BUFSIZE];
  char permFile[80];

  strcpy(mk, path);
  strcat(mk, dirname);

  printf("\n\nOperação: remover o diretório de nome >>%s<< em %s.\n", dirname, path);

  if(!checkDirPermission(path, dirname, permFile)){
  		return erros("Você não tem permissão para remover este diretório.");
  }
  
  if( (rmdir(mk)) == -1)
  {
    return erros("Erro ao tentar remover o diretório, verifique se o mesmo esta vazio");
  }

  unlink(permFile);

  sprintf(mk,"Diretório removido, novo path: %s.",path);

  return rmMachinetoHuman(mk);

  //return "Você chamou a função que remove diretórios";
}

/* Lista todos os arquivos e diretórios a partir de um dado path */
char * list(char * path)
{
  DIR* dir; 
  struct dirent* entrada; 
  dir = opendir( path );
  
  bzero(req,MAX);

  printf("\n\nOperacao: listar todos os arquivos e diretórios em >>%s<<.\n", path); 

  if( !dir )
  {   
    return erros("Erro ao tentar listar no path fornecido, verifique."); 
  }

  strcpy(req,"list");
  strcat(req,reserved); 

  entrada = readdir( dir); 

  while( entrada)
  {   
   
    if (entrada->d_type == ISFILE)
    {
      if(entrada->d_name[0] != '.')
      {
        strcat(req,"A");
        strcat(req,entrada->d_name);
        strcat(req,reserved);
      }
    }
    else
      if(entrada->d_type == ISDIR && (strcmp(entrada->d_name, ".")!=0 && strcmp(entrada->d_name, "..") != 0))
      {
        strcat(req,"D");
        strcat(req,entrada->d_name);
        strcat(req,reserved);
      }

    entrada = readdir( dir);  
  }

  printf("\n");

  closedir( dir );

  return req;

  //return "Você chamou a função que lista arquivos no diretório";
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

	for (char * p = strtok(buf, reserved); p; p = strtok(NULL, reserved))
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
		strcpy(command, erros("Erro de validação do usuário."));
		return -2;
	}

	currentUser = atoi(params[n-2]);
	
	if(strcmp(mainCommand, "read") == 0 && n == 6){
		strcpy(command, readFile(params[1], atoi(params[2]), atoi(params[3])));
	}
	else if(strcmp(mainCommand, "write") == 0 && n == 9){
		strcpy(command, writeFile(params[1], params[2], atoi(params[3]),atoi(params[4]), atoi(params[5]), atoi(params[6])));
	}
	else if(strcmp(mainCommand, "info") == 0 && n == 4){
		strcpy(command, fileInfo(params[1]));
	}
	else if(strcmp(mainCommand, "mkdir") == 0 && n == 7){
		strcpy(command, makdir(params[1], params[2], atoi(params[3]),atoi(params[4])));
	}
	else if(strcmp(mainCommand, "rm") == 0 && n == 5){
		strcpy(command, rm(params[1], params[2]));
	}
	else if(strcmp(mainCommand, "list") == 0 && n == 4){
		strcpy(command, list(params[1]));
	}
	else if(strcmp(mainCommand, "quit") == 0 && n == 5){
		strcpy(command, logoutHandler(atoi(params[1]), params[2]));
	}
	else{
		strcpy(command,erros("Erro nos parâmetros. Por favor, verifique a documentação dos comandos.")) ;
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

  req= (char*) malloc(MAX);

  if(req==NULL)
  {
      printf("\nErro no servidor, verifique.\n");
      exit(1);
  }

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
