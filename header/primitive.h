#ifndef PRIMITIVE_H
#define PRIMITIVE_H

	// Declaration of functions
	void displayPrompt();
	void readFile(char * filename);
	void printDirectory();
	void addHistory(char* buffer);
	void changeDirectory(char ** pars);
	void execCommand(char** pars, int background);
	void execCommandRedirection(char** pars, int redirection, int background);
	void execCommandPipe(char** parsPipe, int background);

#endif
