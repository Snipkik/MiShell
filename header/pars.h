#ifndef PARS_H
#define PARS_H

	// Declaration of functions
	char** pars_message(char *raw_cmd, char *limit);
	void pars_messageB(char *raw_cmd, char *limit);
	int checkIfRedirection(char** pars);
	int getRedirectionPosition(char** pars, char* type);
	int checkBackground(char** pars);

	#define READ 0
	#define WRITE 1
	#define ERROR 2
	#define PIPE 3	
	#define DEFAULT 4

#endif
