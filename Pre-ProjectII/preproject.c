// Pre-Project II
// Write a small block off code that takes in an input file name
// from an argument and makees a copy of the file (call the new file
// CloneFile) using the UNIX cp command.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main (int argc, char * argv[])
{
	// Init fork
	int rc = fork();

	// Test for the success of fork
	if (rc < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	}

	// If fork worked, do the child process:
	else if (rc == 0) {
		char *myargs[4];
		myargs[0] = strdup("cp");
		myargs[1] = strdup(argv[1]);
		myargs[2] = strdup("CloneFile");
		myargs[3] = NULL;
		execvp(myargs[0], myargs);
	}

	// Wait for the child process to complete, parent:
	else {
		int cp = wait(NULL);
		char *myargs2[3];
		myargs2[0] = strdup("md5sum");
		myargs2[1] = strdup("CloneFile");
		myargs2[2] = NULL;
		execvp(myargs2[0], myargs2);
	}

	return 0;
}
