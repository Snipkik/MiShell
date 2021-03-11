// C libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

// Header
#include "header/colors.h"
#include "header/pars.h"
#include "header/primitive.h"

int resetfile = 0;


/*** ***/
void displayPrompt()
{
	printf(YELLOW);
	printDirectory();
	printf(CYAN " %s " WHITE, "%");
}

/*** read and display a file ***/
void readFile(char* filename)
{
    FILE *file = NULL;
    char read_string[100];

    if((file = fopen(filename,"r")) == NULL)
    {
        fprintf(stderr,"error opening %s\n",filename);
        exit(1);
    }

    if ((strcmp(filename, "init.txt")) == 0) printf(CYAN);
    
    while(fgets(read_string,sizeof(read_string),file) != NULL)
        printf("%s",read_string);

    fclose(file);
}

/*** ***/
void printDirectory()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s", cwd);
}

/*** ***/
void changeDirectory(char ** pars)
{
	if ((pars[1]==NULL) || (!(strcmp(pars[1], "~") && strcmp(pars[1], "~/"))))
		chdir(getenv("HOME"));
	else chdir(pars[1]);
}

/*** write the last command in a file***/
void addHistory(char * buffer)
{
	char *filename = "history.txt";
    FILE *file = NULL;

    //reset history on each execution
    if (!resetfile)
    {
    	char* argv[] = { "rm", "history.txt", NULL};
    	execCommand(argv, 0);
		resetfile=1;
    }

    if((file = fopen(filename,"a+")) == NULL)
    {
        fprintf(stderr,"error opening %s\n",filename);
        exit(1);
    }

    fprintf(file,"%s",buffer);

    fclose(file);
}


/*** execute a simply command ***/
void execCommand(char **pars, int background)
{
  	pid_t cpid;
  	int status;

	switch (cpid = fork()){
		case -1 :
			perror ("fork");
			break;
		case 0 :
			
			if (execvp(pars[0], pars) < 0) {
	  			printf(RED "%s" WHITE " : Commande introuvable\n", pars[0]);
				exit(EXIT_FAILURE);
			}
			
			//another Syntax

			//exec the command
			// execvp (pars[0], pars);
			// perror (pars[0]);
			// exit (1);
			break;
			
		default :
			//father is waiting or not for his child
			if (background)
				waitpid(cpid, & status, WNOHANG);
			else
				waitpid(cpid, & status, WUNTRACED);
			break;
	}
}

/*** execute a command with redirection : < > 2> ***/
void execCommandRedirection(char** pars, int redirection, int background)
{
	pid_t cpid;
  	int status;

  	int position;

  	char filename[40];
  	int fileDescriptor;

  	//int bg = checkBackground(pars);

	switch (cpid = fork()){
		case -1 :
			perror ("fork");
			break;

		case 0 :

			switch(redirection){
				case READ :
					position = getRedirectionPosition(pars, "<");
					pars[position]=NULL;
					strcpy(filename,pars[position + 1]);

			        if ((fileDescriptor = open(filename, O_RDONLY, 0)) < 0) {
			            perror("Impossible d'ouvrir le fichier");
			            exit(0);
			        }

			        dup2(fileDescriptor, STDIN_FILENO);
			        close(fileDescriptor);
					break;

				case WRITE :
					position = getRedirectionPosition(pars, ">");
					pars[position]=NULL;
					strcpy(filename,pars[position + 1]);

					// 0644	-- User: read & write | Group: read | Other: read
			        if ((fileDescriptor = open(filename, O_CREAT | O_RDWR , 0644)) < 0) {		
			            perror("Impossible de créer le fichier");
			            exit(0);
			        }

		        	dup2(fileDescriptor, STDOUT_FILENO);
		        	close(fileDescriptor);
					break;

				case ERROR :
					position = getRedirectionPosition(pars, "2>");
					pars[position]=NULL;
					strcpy(filename,pars[position + 1]);

					// 0644	-- User: read & write | Group: read | Other: read
			        if ((fileDescriptor = open(filename, O_CREAT | O_RDWR , 0644)) < 0) {		
			            perror("Impossible de créer le fichier");
			            exit(0);
			        }

		        	dup2(fileDescriptor, 2);
		        	close(fileDescriptor);
					break;

				default:
					break;
			}

        	//exec the command
			if (execvp(pars[0], pars) < 0){
	  			printf(RED "%s : Commande introuvable\n", pars[0]);
				exit(EXIT_FAILURE);
			}
			break;

		default :
			//father is waiting or not for his child
			if (background)
				waitpid(cpid, & status, WNOHANG);
			else
				waitpid(cpid, & status, WUNTRACED);
			break;
	}
}


/*** execute a command with multiple pipe ***/
void execCommandPipe(char** parsPipe, int background)
{
	char ** pars = NULL ;
	char filename[40];

	int fileDescriptor;
	int output;
	pid_t cpid;
	int fd[2];
	int fd_in = 0;
	int status;

	int i = 0;
	while(parsPipe[i] != NULL)
    {
		pars = pars_message(parsPipe[i], " \n\t")   ;
     	output = -1;
	    int k = 0;
	    while(pars[k] != NULL)
	    {
			if ((strcmp(pars[k],"<")==0) || (strcmp(pars[k],">")==0) || (strcmp(pars[k],"2>")==0) )
			{
				strcpy(filename,pars[k + 1]);
				pars[k]=NULL;
				pars[k + 1]=NULL;

				if (strcmp(pars[k],"<")==0)
				{
					output = READ ;
					if ((fileDescriptor = open(filename, O_RDONLY, 0)) < 0) {
			            perror("Impossible d'ouvrir le fichier");
			            exit(0);
			        }
				}
				else if (strcmp(pars[k],">")==0)
				{
					output = WRITE ;
					// 0644	-- User: read & write | Group: read | Other: read
					if ((fileDescriptor = open(filename, O_CREAT | O_RDWR , 0644)) < 0) {
			            perror("Impossible de créer le fichier");
			            exit(0);
			        }
				}
				else if (strcmp(pars[k],"2>")==0)
				{
					output = ERROR;
					if ((fileDescriptor = open(filename, O_CREAT | O_RDWR , 0644)) < 0) {
			            perror("Impossible de créer le fichier");
			            exit(0);
			        }
				}
			}
			k++;
	    }

	    //open the pipe
	    if (pipe(fd) == -1) {
	        perror("pipe failed");
	        exit(EXIT_FAILURE);
    	}

    	switch(cpid = fork())
    	{
	    	case -1 :
		        perror("fork failed");
		        exit(EXIT_FAILURE);
		        break;

		    case 0 :
				dup2(fd_in,0);
				
				//
				if (parsPipe[i+1]!= NULL)
				{
				 	dup2(fd[1],1);
				}
				close(fd[0]);

				// if there is a redirection
				if (output != -1)
				{
					if (dup2(fileDescriptor, output) < 0)
					{
						perror ("dup2() failed") ;
					}
					if (close(fileDescriptor))
					{
						perror("close() failed");
					}
				}

				//exec the command
				if (execvp(pars[0], pars) < 0){
			  		printf(RED "%s" WHITE " : Commande introuvable\n", pars[0]);
					exit(EXIT_FAILURE);
				}
				break;

			default:

				//father is waiting or not for his child
				if (background)
					waitpid(cpid, & status, WNOHANG);
				else
					waitpid(cpid, & status, WUNTRACED);
				close(fd[1]);
      			fd_in = fd[0];
		    	break;
		}
      	i++;
    }
}
