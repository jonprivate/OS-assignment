#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

// delimiters
char delim[] = " \t";
#define PROMPT_MAX 200
#define COMMAND_MAX 8192
#define ARG_NUM_MAX 32
inline void CHKERR(int ret, char* msg) {
	if(ret < 0) {
		fprintf(stderr, "ERROR: %s, %s\n", msg, strerror(errno));
		exit(-1);
	}
}
// supported command list
enum command_t {NONE, EXIT, SYSTEM, TEST};
// set the prompt
void setPrompt(char* prompt);
// parse the input command
command_t parseCommand(char* cmdLine, char** args);
// execute the command
void executeCommand(command_t cmd, char** args);
// check status
void pr_exit(const char* name, int status);

// prompt
char prompt[PROMPT_MAX];
// command line
char cmdLine[COMMAND_MAX];
// command arguements
char* args[ARG_NUM_MAX];
// background or not
bool bg = false;

/******************************
 *      Main function
 ******************************/
int main(int argc, char** argv)
{
	command_t cmd = NONE;
	while(cmd != EXIT) {
		setPrompt(prompt);
		cin.getline(cmdLine,COMMAND_MAX);
		cmd = parseCommand(cmdLine, args);
		executeCommand(cmd, args);
	}
	return 0;
}

/****************************************************/
void setPrompt(char* prompt) {
	sprintf(prompt, "shell-> ");
	cout << prompt;
}

command_t parseCommand(char* cmdLine, char** args) {
	bg = false;
	if(strcmp(cmdLine, "exit") == 0)
		return EXIT;
	else if(strcmp(cmdLine, "test") == 0)
		return TEST;
	else {
		char* res = cmdLine;
		size_t cnt = 0;
		while((res = strtok(res, delim)) != NULL) {
			//cout << res << endl;
			args[cnt++] = strdup(res);
			res = NULL;
		}
		args[cnt] = NULL;
		if(strcmp(args[cnt-1],"&") == 0) {
			bg = true;
			args[cnt-1] = NULL;
		}
		return SYSTEM;
	}

	return NONE;
}

void executeCommand(command_t cmd, char** args) {
	switch(cmd) {
		case SYSTEM:
			int r, cpid, status;
			cpid = fork();
			CHKERR(cpid,"fork");
			if(cpid == 0) {
				r = execvp(args[0], args);
				CHKERR(r,args[0]);
			} else {
				if(!bg) {
					waitpid(cpid, &status, 0);
					pr_exit(args[0],status);
				}
			}
			break;
		case EXIT:
			cout << "About to leave^_^" << endl;
			break;
		case TEST:
			cout << "Just to test the program" << endl;
			break;
		case NONE:
			cout << "This is not a legal command" << endl;
	}
}

void pr_exit(const char* name, int status) {
	if(WIFEXITED(status))
		return;
	else if(WIFSIGNALED(status))
		fprintf(stderr, "%s exit abnormally, signal %d caught%s\n", name, WTERMSIG(status),
			#ifdef WCOREDUMP
				WCOREDUMP(status) ? "core dumped" : "");
			#else
				"");
			#endif
	else if(WIFSTOPPED(status))
		fprintf(stderr, "child stopped, signal %d caught", WSTOPSIG(status));
}