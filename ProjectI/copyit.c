// Include Libraries
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Functions and globals
void display_message(int s) {
	printf("copyit: still copying...\n");
	alarm(1);
}

// Main Execution
int main(int argc, char *argv[]) {
	// Set-up
	char data[256];
	int result = 1;
	int bytecount = 0;
	int errorcount = 0;

		// Check number of args
		if (argc > 3) {
			printf("copyit: Too many arguments!\n");
			printf("usage: copyit <sourcefile> <destinationfile>\n");
		}
		if (argc < 3) {
			printf("copyit: Not enough arguments!\n");
			printf("usage: copyit <sourcefile> <destinationfile>\n");
		}

		// Set up the periodic message
		signal(SIGALRM, display_message);
		alarm(1);

		// Open the source file or exit with an error
		int sourcefile = open("source.txt", O_RDONLY);
		if(sourcefile < 0){
			printf("Issue opening %s: %s!\n", argv[1], strerror(errno));
			return 1;
		}

		// Create the target file or exit with an error
		int targetfile = open("target.txt", O_WRONLY | O_CREAT | O_TRUNC, 0755);
		if (targetfile < 0){
			printf("Issue opening %s: %s!\n", argv[2], strerror(errno));
			return 1;
		}

	while (result) {
		// Read a bit of data from the source file
		result = read(sourcefile, data, 256);
		bytecount = bytecount + result;

		// If the read was interrupted, try it again
		if (result < 0) {
			while(errno==EINTR) {
				result = read(sourcefile, data, 256);
				bytecount = bytecount + result;
				errorcount = errorcount + 1;
				if (errorcount >= 100) {
					printf("Read was interrupted over 100 times in a row, terminated program,\n");
					return 1;
				}
			}
		}
		errorcount=0;

		// If there was an error reading, exit with an error
		if (result < 0) {
			printf("Unable to successfully read %s: %s!\n", argv[1], strerror(errno));
		}

		// If no data left, end the loop
		if (result == 0) {
			break;
		}

		// Write a bit of data to the target file
		write(targetfile, data, result);

		// If the write was interrupted, try it again
		if (result < 0) {
			while(errno==EINTR) {
				write(targetfile, data, result);
				errorcount = errorcount + 1;
				if (errorcount >= 100) {
					printf("Write was interrupted over 100 times in a row, terminated program,\n");
					return 1;
				}
			}
		}
		errorcount = 0;

		// If not all the data was written, exit with an error
		if (result < 0) {
 			printf("Unable to successfully write to %s: %s\n",argv[2],strerror(errno));
		}
	}

	// Close both files
	close(sourcefile);
	close(targetfile);

	// Print success message
	printf("copyit: Copied %d bytes from file %s to %s.\n", bytecount, argv[1], argv[2]);

} // End main execution
