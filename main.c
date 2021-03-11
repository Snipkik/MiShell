	// C libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

	// Header
#include "header/colors.h"
#include "header/pars.h"
#include "header/primitive.h"

int main(void)
{
  char *buffer = NULL;
  char **pars = NULL;
  char **parsPipe = NULL;
  char* pipe = NULL;
  size_t	buf_size = 4096;

  int background;

  // alloc buffer which get back the command typed by the user
	buffer = (char *)calloc(sizeof(char), buf_size);
	if (buffer == NULL) {
		perror("Malloc failure");
		return (EXIT_FAILURE);
	}

  //malloc
  pipe = (char *)calloc(sizeof(char), 200);
  if (pipe == NULL) {
   perror("Malloc failure");
   exit(EXIT_FAILURE);
  }

  //display the prompt
  readFile("init.txt");
  displayPrompt();


  // wait for a command
  while (getline(&buffer, &buf_size, stdin) > 0) {

    addHistory(buffer);

    strcpy(pipe, buffer);

    //pars the buffer
    pars = pars_message(buffer, " \t\n");
    parsPipe = pars_message(pipe, "|");

    background = checkBackground(pars);

    //built-in Commands
    if( pars[0] == NULL ) {
		}
    else if((strcmp(pars[0],"exit"))==0) {
			return (EXIT_SUCCESS);
		}
    else if ((strcmp(pars[0],"cd"))==0) {
      changeDirectory(pars);
    }
    else if ((strcmp(pars[0],"history"))==0) {
      readFile("history.txt");
    }
    //commmands
    else {

      //check for redirection : > < 2> |
      int redirection = checkIfRedirection(pars);

      //
      if (redirection == READ || redirection ==  WRITE || redirection == ERROR){
        execCommandRedirection(pars, redirection, background);
      }
      else if (redirection == PIPE) {
        execCommandPipe(parsPipe, background);
      }
      else
      {
        execCommand(pars, background);
      }
    }

    //display the prompt
    displayPrompt();
  }


  //free
  free(buffer);
  int i = 0;
  while (pars[i] != NULL) {
      free(pars[i]);
      i++;
  }
  free(pars);

  return 0;
}
