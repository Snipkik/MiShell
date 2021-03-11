// C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Header
#include "header/colors.h"
#include "header/pars.h"
#include "header/primitive.h"


/*** pars the message ***/
char** pars_message(char *raw_cmd, char *limit)
{
	char	*ptr = NULL;
	char	**cmd = NULL;
	size_t	idx = 0;

	// split on limit
	ptr = strtok(raw_cmd, limit);

	while (ptr) {
		cmd = (char **)realloc(cmd, ((idx + 1) * sizeof(char *)));
		cmd[idx] = strdup(ptr);
		ptr = strtok(NULL, limit);
		++idx;
	}

	//
	cmd = (char **)realloc(cmd, ((idx + 1) * sizeof(char *)));
	cmd[idx] = NULL;

	return (cmd);
}

/*** check if there is a redirection : < > 2> | ***/
int checkIfRedirection(char** pars)
{
	int i = 0;

	while (pars[i] != NULL) {
      if ((strcmp(pars[i],"|"))==0) {
      	return PIPE;
      }
      else if ((strcmp(pars[i],"<"))==0) {
      	return READ;
      }
      else if ((strcmp(pars[i],">"))==0) {
      	return WRITE;
      }
      else if ((strcmp(pars[i],"2>"))==0) {
        return ERROR;
      }
      i++;
  	}
  	return DEFAULT;
}

/* return 1 if the command must be executed in background
 * else return 0
 */
int checkBackground(char** pars)
{
	int i = 0;

	while (pars[i] != NULL) {
      if ((strcmp(pars[i],"&"))==0)
      {
      	pars[i]= NULL;
      	return 1;
      }
      i++;
  	}
  	return 0;
}

/* get the position of an element  
 * return 0 if none
 */
int getRedirectionPosition(char** pars, char* type)
{
	int i = 0;

	while (pars[i] != NULL) {
      if ((strcmp(pars[i],type))==0) {
      	return i;
      }
      i++;
  	}
  	return 0;
}