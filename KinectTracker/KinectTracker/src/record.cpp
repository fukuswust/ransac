#include "record.h"
#include <fstream>

void recordSlices(float slices[], int numSlices, int fileOn) {
	char bufferFileName[128];
	sprintf(bufferFileName, "sliceData/%.6u.csv", fileOn);
	FILE * sliceFile = fopen(bufferFileName,"w");
	for (int i=0, iIm=0; i < numSlices*2; ) {
		fprintf(sliceFile,"%f,",slices[i++]);
		fprintf(sliceFile,"%f\n",slices[i++]);
	}
	fclose(sliceFile);
}

void recordAccel(float xAccel, float yAccel, float zAccel, int fileOn) {
	char bufferFileName[128];
	sprintf(bufferFileName, "accelData/%.6u.csv", fileOn);
	FILE * accelFile = fopen(bufferFileName,"w");
	fprintf(accelFile,"%f,",xAccel);
	fprintf(accelFile,"%f,",yAccel);
	fprintf(accelFile,"%f",zAccel);
	fclose(accelFile);
}

void recordDepth(float depthBuffer[], int outFileOn) {
	char bufferFileName[128];
	sprintf(bufferFileName, "depthData/%.6u.csv", outFileOn);
	FILE * depthFile = fopen(bufferFileName,"w");
	int i = 0;
	while (i < 640*480) {
		for (int x = 0; x < 640-1; x++) {
			fprintf(depthFile,"%u,",depthBuffer[i++]);
		}
		fprintf(depthFile,"%u\n",depthBuffer[i++]);
	}
	fclose(depthFile);
}

void recordColor(float colorBuffer[], int fileOn) {
	char bufferFileName[128];
	sprintf(bufferFileName, "colorData/%.6u_R.csv", fileOn);
	FILE * redFile = fopen(bufferFileName,"w");
	int i = 0;
	while (i < 640*480) {
		for (int x = 0; x < 640-1; x++) {
			fprintf(redFile,"%u,",colorBuffer[((i++)*3)+0]);
		}
		fprintf(redFile,"%u\n",colorBuffer[((i++)*3)+0]);
	}
	fclose(redFile);
			
	sprintf(bufferFileName, "colorData/%.6u_G.csv", fileOn);
	FILE * greenFile = fopen(bufferFileName,"w");
	i = 0;
	while (i < 640*480) {
		for (int x = 0; x < 640-1; x++) {
			fprintf(greenFile,"%u,",colorBuffer[((i++)*3)+1]);
		}
		fprintf(greenFile,"%u\n",colorBuffer[((i++)*3)+1]);
	}
	fclose(greenFile);

	sprintf(bufferFileName, "colorData/%.6u_B.csv", fileOn);
	FILE * blueFile = fopen(bufferFileName,"w");
	i = 0;
	while (i < 640*480) {
		for (int x = 0; x < 640-1; x++) {
			fprintf(blueFile,"%u,",colorBuffer[((i++)*3)+1]);
		}
		fprintf(blueFile,"%u\n",colorBuffer[((i++)*3)+1]);
	}
	fclose(blueFile);
}