// myshell.c
// Nikolas Dean Brooks
// February 10th, 2017
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

void ctrlcHandler(int sig) {
		char  c;
		signal(sig, SIG_IGN);
		printf("\nmyshell: I see you pressed ctrl+c... See you later, hater!\n");
		exit(0);
}

int main (int argc, char * argv[])
{
	// Initialize Variables
	int quit = 0;
	char input[4096];
	signal(SIGINT, ctrlcHandler);

	// Start shell execution
	while (quit == 0) {
		// Start new myshell line
		printf("myshell> ");
		fflush(stdout);

		// Initialize req'd variables
		char *token;
		char *words[101];
		int nwords = 0;
		int i = 0;

		// Take in input
			// If fgets returns null, set exit = 1
			if (!fgets(input,4096,stdin)) {
				quit = 1;
			}

			// After acquiring input, break it into separate words:
				// Acquire and test the start of the input
				token = strtok(input, " \t\n");
				if (token == NULL) {
					continue;
				}

				// If input is acceptable, place the first word in words[]:
				words[i] = token;
				nwords++;

				// If there are any more words, place the rest of the words in words[]:
				while(token != NULL) {
					i++;
					token = strtok(0, " \t\n");
					words[i] = token;
					nwords++;
				}

				// If the number of words exceeded the decided limit, take in a new input:
				if (nwords > 101) {
					printf("Usage: Input exceeds input limit of 100 words\n");
					continue;
				}

				// Cap off words[]
				words[nwords] = 0;

		// Check words[0] for the provided command to myshell
			// If "start" was the first word:
			if (strcmp(words[0],"start") == 0) {
				// Start the fork for the provided command
				int rc = fork();
				if (rc < 0) {
					fprintf(stderr, "Forking error occurred!\n");
					continue;
				}

				// If forking worked, carry out the provided command (child process)
				else if (rc == 0) {
					// Req'd variables, transfer of words[] to myargs[]
					char *myargs[100];
					int j = 0;
					int valid = 0;
					for (j = 0; j < nwords; j++){
						myargs[j] = words[j+1];
					}

					// Attempt execution of the desired process:
					valid = execvp(myargs[0], myargs);

					// If the provided command wasn't valid:
					if (valid == -1) {
						printf("myshell: error: invalid UNIX command provided to 'start'\n");
						exit(1);
					}
				}
				else {
					printf("myshell: process %d started\n", rc);
				}
			}

			// If "wait" was the first word:
			else if (strcmp(words[0],"wait") == 0) {
				// Initialize required variables:
				int status;
				int pid;

				// Begin wait function:
				pid = waitpid(-1, &status, WUNTRACED);

				// Check that there are processes to wait for:
				if (pid == -1){
					printf("myshell: no processes left\n");
				}

				// If there is a process to wait for, check on it:
				else if (WIFEXITED(status)) {
					printf("myshell: process %d exited normally with status %d\n", pid, WEXITSTATUS(status)); /*Getting null????*/
				}
				else if (WIFSIGNALED(status)){
					printf("myshell: process %d exited abnormally with signal %d: %s\n", pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
				}
				else if (WIFSTOPPED(status)) {
					printf("myshell: process %d is stopped with status %s\n", pid, WIFSTOPPED(status));
				}
				else if (WIFCONTINUED(status)) {
					printf("myshell: process %d is continued with status %s\n", pid, WIFCONTINUED(status));
				}
				else {
					printf("Your wait command goofed m8, unprepared for this outcome\n");
				}
			}
			// If "run" was the first word:
			else if (strcmp(words[0],"run") == 0) {
				// Start the fork for the provided command
				int rc = fork();
				int status;
				int pid;
				if (rc < 0) {
					fprintf(stderr, "Forking error occurred!\n");
					continue;
				}

				// If forking worked, carry out the provided command (child process)
				else if (rc == 0) {
					// Req'd variables, transfer of words[] to myargs[]
					char *myargs[100];
					int j = 0;
					int valid = 0;
					for (j = 0; j < nwords; j++){
						myargs[j] = words[j+1];
					}

					// Attempt execution of the desired process:
					valid = execvp(myargs[0], myargs);

					// If the provided command wasn't valid:
					if (valid == -1) {
						printf("myshell: error: invalid UNIX command provided to 'run'\n");
						exit(1);
					}
				}
				else {
					printf("myshell: process %d started\n", rc);
				}

				// Begin wait:
				pid = waitpid(rc, &status, 0);

				// Check that there are processes to wait for:
				if (pid == -1){
					printf("myshell: no processes left\n");
				}

				// If there is a process to wait for, check on it:
				else if (WIFEXITED(status)) {
					printf("myshell: process %d exited normally with status %d\n", pid, WEXITSTATUS(status)); /*Getting null????*/
				}
				else if (WIFSIGNALED(status)){
					printf("myshell: process %d exited abnormally with signal %d: %s\n", pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
				}
				else if (WIFSTOPPED(status)) {
					printf("myshell: process %d is stopped with status %s\n", pid, WIFSTOPPED(status));
				}
				else if (WIFCONTINUED(status)) {
					printf("myshell: process %d is contiued with status %s\n", pid, WIFCONTINUED(status));
				}
				else {
					printf("Your wait command goofed m8, unprepared for this outcome\n");
				}
			}
			// If "kill" was the first word:
			else if (strcmp(words[0],"kill") == 0) {

				// Check if a process was provided:
				if (words[i] == NULL) {
					printf("myshell: No process selected to kill\n");
					continue;
				}

				// If a process WAS provided, try to kill it and then check if it worked:
				else if (atoi(words[1]) != 0) {
					int fd = kill(atoi(words[1]), 9);
					if (fd < 0) {
						printf("Unable to kill %s: %s\n", words[1], strerror(errno));
					}
					else {
						printf("myshell: process %s killed\n", words[1]);
					}
				}
				else if (atoi(words[1]) == 0) {
					printf("Unable to kill %s: %s is NOT a valid PID\n", words[1], words[1]);
				}
			}

			// If "stop" was the first word:
			else if (strcmp(words[0],"stop") == 0) {

				// Check if a process was provided:
				if (words[i] == NULL) {
					printf("myshell: No process selected to stop\n");
					continue;
				}

				// If a process WAS provided, try to stop it and then check if it worked:
				else if (atoi(words[1]) != 0) {
					int fd = kill(atoi(words[1]), 23);
					if (fd < 0) {
						printf("Unable to stop %s: %s\n", words[1], strerror(errno));
					}
					else {
						printf("myshell: process %s stopped\n",words[1]);
					}
				}
				else if (atoi(words[1]) == 0) {
					printf("Unable to stop %s: %s is NOT a valid PID\n", words[1], words[1]);
				}

			}

			// If "continue" was the first word:
			else if (strcmp(words[0],"continue") == 0) {

				// Check if a process was provided:
				if (words[1] == NULL) {
					printf("myshell: No process selected to continue\n");
					continue;
				}

				// If a process WAS provided, try to continue it and then check if it worked:
				else if (atoi(words[1]) != 0) {
					int fd = kill(atoi(words[1]), 25);
					if (fd < 0) {
						printf("Unable to continue %s: %s\n", words[1], strerror(errno));
					}
					else {
						printf("myshell: process %s continued\n",words[1]);
					}
				}
				else if (atoi(words[1]) == 0) {
					printf("Unable to continue %s: %s is NOT a valid PID\n", words[1], words[1]);
				}
			}

			// "Quit" or "exit":
			else if (strcmp(words[0],"quit") == 0) {
				quit = 1;
			}
			else if (strcmp(words[0],"exit") == 0) {
				quit = 1;
			}

			// Command not listed:
			else {
				printf("myshell: unknown command: %s\n", words[0]);
			}
	}

	// Once the shell has been executed
	return 0;
}
