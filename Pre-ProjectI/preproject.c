// Pre-Project I
// Nikolas Dean Brooks
// 1/20/2017

#include <stdio.h>
#include <signal.h>

char bKeepLooping = 1;

void charHandler(int dummy) {
	bKeepLooping = 0;
}

int main( int argc, char *argv[] ) {

		signal(SIGINT, charHandler);

		while(bKeepLooping) {
		}

    printf("\rExited successfully\n");

    return 0;
}