#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/xattr.h>



#define P 0777
#define TRUE 1
#define FALSE 0

struct stat sb = {0};
int altera;

void seekFile();
void criaFiles();
void printPermissions();
void printFile(FILE* entry);
void alterFile(FILE* entry);
void changePermission();
void ListarArquivos(char caminho[]);

int main (void)
{
    altera = FALSE;

   
    criaFiles();
    //changePermission();
	 ListarArquivos("./Root");
    //seekFile();
    return 0;
}

void criaFiles()
{
    char a[10][30];

    strcpy(a[0],"./Root");
    strcpy(a[1],"/a");
    strcpy(a[2],"/b");
    strcpy(a[3],"/c");   

    sprintf(a[1],"%s%s",a[0],"/a");   
    sprintf(a[2],"%s%s",a[0],"/b");   
    sprintf(a[3],"%s%s",a[0],"/c");

    if (stat(a[0], &sb) == -1)
    {
         mkdir(a[0],P);
    }
    else
    {
        int i;
        for (i=1;i<=3;i++)
        {       
            mkdir(a[i],P);
            if(stat(a[i],&sb) == 0)
            {
                char b[9];
                switch (i)
                {
                    case 1:
                        strcpy(b,"asd");
                        break;
                    case 2:
                        strcpy(b,"zxc");
                        break;
                    case 3:
                        strcpy(b,"qwe");
                        break;
                    default:
                        strcpy(b,"bligs");
                }
                strcat(a[i],"/lucas.txt");
                FILE *p = fopen(a[i],"wt");
                if(p==NULL)
                    exit(1);
                fprintf(p,"Hello World %s\n",b);               
                fclose(p);

            }
        }
    }
   
}

void printPermissions()
{
    printf("File type: ");
    switch (sb.st_mode & S_IFMT)
    {
        case S_IFBLK: 
            printf("block device\n");           
            break;
        case S_IFCHR: 
            printf("character device\n");       
            break;
        case S_IFDIR: 
            printf("directory\n");    
            break;
        case S_IFIFO: 
            printf("FIFO/pipe\n");              
            break;
        case S_IFLNK: 
            printf("symlink\n");                
            break;
        case S_IFREG: 
            printf("regular file\n");           
            break;
        case S_IFSOCK:
            printf("socket\n");                 
            break;
        default:      
            printf("unknown?\n");               
            break;
    }

    printf("I-node number:\t%ld\n", (long) sb.st_ino);
    printf("Mode:\t%lo (octal)\n",(unsigned long) sb.st_mode);
    printf("Ownership:\tUID=%ld  GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);
    printf("\n");
}

void seekFile()
{
    DIR* FD;
    char busca[255];
    struct dirent* input;
    FD =     opendir("./Root");
    FILE *entry;

    printf("DIgite o nome do arquivo a ser procurado: ");
    scanf("%s",busca);


    while((input=readdir(FD))!=NULL)
    {
        char string[30];

        if( !strcmp(input->d_name, ".") || !strcmp(input->d_name, "..") )
        {
        }
        else
        {
            sprintf(string,"./Root/%s",input->d_name);
            printf("Path = %s\n",string);
            if (opendir(string)==NULL)
            {
                printf("not a folder\n");
                continue;
            }
 	    if(strstr(busca,".txt")==NULL)
                sprintf(string,"%s/%s.txt",string,busca);
            else
                sprintf(string,"%s/%s",string,busca);
            entry=fopen(string,"rt");
            if (entry!=NULL)
            {
                char y = 'n';
                printf("Arquivo encontrado\n");
                printf("Conteudo do arquivo: ");
                printFile(entry);
                printf("Voce gostaria de alterar o arquivo?(y/n) ");
                scanf(" %c",&y);
                if (y=='y')
                {
                    printf("if");
                    fclose(entry);
                    entry = fopen(string,"wt");
                    alterFile(entry);
                    fclose(entry);
                    entry = fopen(string,"rt");
                    printFile(entry);
                }
                y = 'n';
                printf("Mudar Permissao? (y/n)");
                scanf(" %c",&y);
                fclose(entry);       
                if (y=='y')
                {    
                    changePermission();
                }
            }
            else
            {
                printf("No such file in directory\n");
            }
            printf("\n");
        }
    }

}

void changePermission()
{
    DIR* FD;
    struct dirent* input;
    FD =     opendir("./Root");
    FILE *entry;


    while((input=readdir(FD))!=NULL) //le todos os conteudos de FD
    {
        char string[30];
        struct dirent* files;
        DIR* newFolder;

        if( !strcmp(input->d_name, ".") || !strcmp(input->d_name, "..") )
        {
        }
        else
        {
            printf("FolderName = %s\n",input->d_name);
            sprintf(string,"./Root/%s",input->d_name);
            if ((newFolder = opendir(string))==NULL)            //entra em A/B/C
            {
                printf("not a folder\n");
                continue;
            }
            while((files=readdir(newFolder))!=NULL)                //le os txt's
            {
                if( !strcmp(files->d_name, ".") || !strcmp(files->d_name, "..") )
                {
                }
                else
                {
                    char path[500];			
                    sprintf(path,"./Root/%s/%s",input->d_name, files->d_name);
                    printf("\tFileName = %s\n",files->d_name);
                    if (stat(path,&sb) == -1)
                    {
                        printf("deu ruim");
                    }
                    else
                    {
			printPermissions();
			chmod(path,0777);
			stat(path,&sb);
			printPermissions();

                    }
                }
            }       
           
        }
    }
}

void printFile(FILE* entry)
{
    char a;
    while(fscanf(entry,"%c",&a) != EOF)
    {
        printf("%c",a);
        fflush(stdout);
    }
}

void alterFile(FILE* entry)
{
    fprintf(entry,"oi");
}

void ListarArquivos(char caminho[])
{
    DIR* FD;
    struct dirent* input;
    FD =     opendir(caminho);
    FILE *entry;
    char nomes[200][3000];
    int i=0,cont=0;

	printf("oi");
    while((input=readdir(FD))!=NULL) //le todos os conteudos de FD
    {
        char string[30];
        struct dirent* files;
        DIR* newFolder;

        if( !strcmp(input->d_name, ".") || !strcmp(input->d_name, "..") )
        {
        }
        else
        {
            printf("FolderName = %s\n",input->d_name);
            sprintf(string,"%s/%s",caminho,input->d_name);
            if ((newFolder = opendir(string))==NULL)            //entra em A/B/C
            {
                printf("not a folder\n");
                continue;
            }
            while((files=readdir(newFolder))!=NULL)                //le os txt's
            {
                if( !strcmp(files->d_name, ".") || !strcmp(files->d_name, "..") )
                {
                }
                else
                {
			if (files->d_type != DT_DIR)
			{
               			strcpy(nomes[i],files->d_name);
				i++;
			}
			else
			{
				char pasta[500];
				getwd(pasta);
				sprintf("%s/%s",pasta,files->d_name);
				printf("pasta = %s\n\n",pasta);
				ListarArquivos(pasta);
				printf("file %s not a file\n",files->d_name);
			}
                }
            }       
           
        }
    }
		printf("Arquivos encontrados\n");
	for(cont=0;cont<i;cont++)
	{
		printf("%s\t",nomes[cont]);
		if(strstr(nomes[cont],".txt")==NULL)
			printf("eh uma pasta \n");
		if(cont%2==0)
		{	
			printf("\n");
		}
	}

	printf("\n");

}





