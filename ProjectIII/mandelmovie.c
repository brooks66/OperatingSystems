// mandelmovie.c
// Author: Nikolas Dean Brooks
// Date: 2/24/2017
//
//
#include "bitmap.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	// Initialize required variables
		int currentProcesses = 0;
		int maxProcesses = 1;
		if (argv[1] != NULL) {
		maxProcesses = strtol(argv[1], NULL, 10);
		}
		int processesStarted = 0;
		int FirstTimeThrough = 0;
		float s = 2;

	// Initialize image production
	while (processesStarted < 50) {
		if (currentProcesses <= maxProcesses) { // If more processes are allowed, start forking
			// Start fork
			int rc = fork();
			processesStarted++;

			/* Initialize stuff for this forked process */
			currentProcesses++;												//
			if (FirstTimeThrough > 0) {								//
			  s = s*(exp(log(.000001/2)/50));					//
			}																					//
			else {																		//
			  FirstTimeThrough++;											//
			}																					//
			char sstr[20];														//
			sprintf(sstr, "%f", s);										//
			/* Finish initializing stuff for this process */

			// If there was an error in forking, say so:
			if (rc < 0) {
				fprintf(stderr, "Forking error occurred!\n");
				exit(1);
			}

			// If all was good with the fork, let's do it:
			else if (rc == 0) {
				// Initialize req'd vars to execute mandelbrot creation
				char *myargs[50];

				// Prepare for mandel execution:
				myargs[0] = "./mandel";
				myargs[1] = "-x";
				myargs[2] = "-1.249";
				myargs[3] = "-y";
				myargs[4] = "-.03438";
				myargs[5] = "-s";
				myargs[6] = sstr;
				myargs[7] = "-m";
				myargs[8] = "4000";
				myargs[9] = "-W";
				myargs[10] = "750";
				myargs[11] = "-H";
				myargs[12] = "750";
				myargs[13] = "-n";
				myargs[14] = "1";
				myargs[15] = "-o";
				char outfile[15];
				sprintf(outfile, "mandel%d.bmp", processesStarted);
				myargs[16] = outfile;
				myargs[17] = NULL;
				// Execute mandel
				execvp(myargs[0], myargs);
			}
		}

		// If there are too many processes running to start a new one, wait:
		else {
			wait(NULL);
			currentProcesses--;
		}

	} // FINISHED CREATING IMAGES

	return 0;
}
