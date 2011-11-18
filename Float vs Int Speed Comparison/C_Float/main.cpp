#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#define NUMARRAYS 100
#define WIDTH 640
#define HEIGHT 480

int main( int argc, const char* argv[] )
{
	//initialize test arrays
	float* testArrays[NUMARRAYS];
	for (int k = 0; k < NUMARRAYS; k++) {
		testArrays[k] = (float*)calloc(WIDTH*HEIGHT,sizeof(float));
	}
	for (int k = 0; k < NUMARRAYS; k++) {
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				testArrays[k][(j*WIDTH)+i] = 512+(rand()%1024);
			}
		}
	}

	//Start
	std::clock_t start = std::clock();

	//Code
	for (int k = 0; k < NUMARRAYS; k++) {
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				testArrays[k][(j*WIDTH)+i] = (100/((-0.00307*testArrays[k][(j*WIDTH)+i])+3.3));
			}
		}
	}

	//End
	printf("Time: %f\n",( ( std::clock() - start ) / (double)CLOCKS_PER_SEC ));

	printf("Value: %f\n", testArrays[rand()%NUMARRAYS][((rand()%HEIGHT)*WIDTH)+(rand()%WIDTH)]);

	return 0;
}
