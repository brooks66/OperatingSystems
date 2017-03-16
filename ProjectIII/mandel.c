#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void * compute_image( void *thread_args );

typedef struct info_s {
	struct bitmap *ibm;
	double ixmin;
	double ixmax;
	double iymin;
	double iymax;
	int imax;
	int iq;
	int numThreads;
} info;

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-n <threads>  Set the number of threads (default = 1)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	int    numThreads = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:n:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'n':
				numThreads = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Display the configuration of the image.
	printf("mandel: x=%lf .y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

	// Compute the Mandelbrot image
	int q = 0;
	
		// Allocate memory required for threads
		pthread_t *array = (pthread_t *)malloc(sizeof(pthread_t)*numThreads);
	
		// Begin thread creation
		for (q = 0; q < numThreads; q++) {
				// Make sure each thread uses separate base data
		    info *thread_args = (info *)malloc(sizeof(info));
		    
		    // Populate struct with correct parameters
		    thread_args->ibm = bm;
		    thread_args->ixmin = xcenter-scale;
		    thread_args->ixmax = xcenter+scale;
		    thread_args->iymin = ycenter-scale;
		    thread_args->iymax = ycenter+scale;
		    thread_args->imax = max;
		    thread_args->iq = q;
		    thread_args->numThreads = numThreads;
		    
		    // Create thread with provided info
		    pthread_create(&array[q], NULL, compute_image, thread_args);
		}
		
		// Join all the threads together
		for (q = 0; q < numThreads; q++) {
		    pthread_join(array[q], NULL);
		}

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}

	free(array);
	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/
void * compute_image( void *args )
{
	info * thread_args = (info *) args;
	int i,j;

	int width = bitmap_width(thread_args->ibm);
	int height = bitmap_height(thread_args->ibm);
	int temp = height/(thread_args->numThreads); //	Decide how work will be split among threads
	
	// For every pixel in the image...
	for(j=temp*(thread_args->iq);j<temp*(thread_args->iq+1) && j <= height; j++) {
		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = thread_args->ixmin + i*(thread_args->ixmax-thread_args->ixmin)/width;
			double y = thread_args->iymin + j*(thread_args->iymax-thread_args->iymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,thread_args->imax);

			// Set the pixel in the bitmap.
			bitmap_set(thread_args->ibm,i,j,iters);
		}
	}
	return 0;
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}

