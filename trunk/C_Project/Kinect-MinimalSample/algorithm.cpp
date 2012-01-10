#include "algorithm.h"
#include "globals.h"
#include <math.h>

void runAlgorithm() {
	float xAccelAvg = xAccel;
	float yAccelAvg = yAccel;
	float zAccelAvg = zAccel;

	rollValue = atan2(xAccelAvg, yAccelAvg);
	pitchValue = atan2(zAccelAvg, yAccelAvg);
	
	// GET GRAVITY ROTATION MATRIX (Simlified from MATLAB code for UP vector = [0 1 0])
	//Get unit vector and magnitude of gravity
	float gravMag = sqrt((xAccelAvg*xAccelAvg)+(yAccelAvg*yAccelAvg)+(zAccelAvg*zAccelAvg)); //Quality = diff from 819/512
	float uGravX = xAccelAvg/gravMag;
	float uGravY = yAccelAvg/gravMag;
	float uGravZ = zAccelAvg/gravMag;
	//Set temporary variables to reduce calculations
	float s = sqrt(1-(uGravY*uGravY));
	float t = 1-uGravY;
	//Perform the cross product of the UP vector and gravity to produce the following rotational axis
	float RotAxisX = -uGravZ;
	float RotAxisZ = uGravX;
	//Determine the unit rotational axis
	float magRotAxis = sqrt((RotAxisX*RotAxisX)+(RotAxisZ*RotAxisZ));
	float uX = RotAxisX/magRotAxis;
	float uZ = RotAxisZ/magRotAxis;
	//Solve multiplications in advance that occur more than once
	float xz = uX*uZ;
	float sx = s*uX;
	float sz = s*uZ;
	//Calculate Individual Matrix Elements
	float R11 = uGravY + (t*uX*uX);
	float R12 = -sz;
	float R13 = t*xz;
	float R21 = sz;
	float R22 = uGravY;
	float R23 = -sx;
	float R31 = t*xz;
	float R32 = sx;
	float R33 = uGravY + (t*uZ*uZ);

	// DECLARE POINT CLOUD DATA as Local Array of Floats (40x30x3 on Stack)
	// DECLARE COLOR DATA as Local Array of Floats (40*30 on Stack)
	float depthPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))*3]; // Stored in the order of [z,y,x]
	float colorPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))];
	int ijPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))*2]; // Stored in the order of [i,j]
	int offset = 0;
	int imOffset = 0;
	int ijOffset = 0;
	float currentMinHeight = 999999.0f;
	float currentMinDir = 999999.0f;
	float currentMaxDir = -999999.0f;

	// Convert depth data to polar point cloud data aligned to initial vector
	// Detect mins and maxes
	// Assign color data to each point
	for (int j = 0; j < 480; j+=DEPTH_SCALE_FACTOR) {
		for (int i = 0; i < 640; i+=DEPTH_SCALE_FACTOR) {
			//Acquire Raw Depth Value
			float z;
			z = (float)(K->mDepthBuffer[(j*640)+i]);
				
			//Check Sensor Data for Error
			if (z == 2047.0f) {
				depthPointCloud[offset] = 999999.0f;
				offset += 3; // -> next
				imOffset++;
				ijOffset += 2;
			} else {
				// Depth to Z
				depthPointCloud[offset++] = z = -100.0f/((-0.00307f * z) + 3.33f); //z -> y
					
				// Z to Point Cloud
				depthPointCloud[offset++] = (float)(j - 240) * (z - 10.0f) * 0.0021f ; //y -> x
				depthPointCloud[offset] = (float)(i - 320) * (z - 10.0f) * -0.0021f ;  //x
					 
				// Set up initial temporary variables
				float tmpX = depthPointCloud[offset--]; // x -> y
				float tmpY = depthPointCloud[offset--]; // y -> z
				float tmpZ = depthPointCloud[offset];   // z

				// Determine fi,fj for color data
				float fi = ((( tmpX - 1.8f) / 0.0023f)/ (-tmpZ - 10)) + 320.0f - 1.0f;
				float fj = (((-tmpY - 2.4f) / 0.0023f)/ (-tmpZ - 10)) + 240.0f - 1.0f;

				// Set Color Value
				int imI = floor(fi);
				int imJ = floor(fj);
				ijPointCloud[ijOffset++] = imI;
				ijPointCloud[ijOffset++] = imJ;
				if ( (imI >= 0) && (imI < 640-2) && (imJ >= 0) && (imJ < 480-2) ) {
					int imSum = 0;
					int imOffset3 = (((imJ*640)+imI)*3);
					for (int count3 = 0; count3 < 3; count3++) { 
						for (int count33 = 0; count33 < 3; count33++) {
							imSum += ((int)K->mColorBuffer[imOffset3++] + 
								(int)K->mColorBuffer[imOffset3++] + (int)K->mColorBuffer[imOffset3++]);
						}
						imOffset3 += -9 + 640;
					}
					colorPointCloud[imOffset++] = imSum / (9.0 * 3.0 * 255.0);
				} else {
					colorPointCloud[imOffset++] = 999999.0f;
				}

				// Reorient Y-Axis to Gravity
				depthPointCloud[offset++] = (tmpX*R13)+(tmpY*R23)+(tmpZ*R33);  // z -> y
				depthPointCloud[offset++] = (tmpX*R12)+(tmpY*R22)+(tmpZ*R32);  // y -> x					
				depthPointCloud[offset--] = tmpX = (tmpX*R11)+(tmpY*R21)+(tmpZ*R31); // x -> y
				tmpY = depthPointCloud[offset--]; // y -> z
				tmpZ = depthPointCloud[offset]; // z

				// Convert to polar coordinates
				depthPointCloud[offset++] = tmpY; // height -> dir
				depthPointCloud[offset++] = atan2(tmpZ,tmpX); // dir -> dis
				depthPointCloud[offset--] = sqrt((tmpZ*tmpZ)+(tmpX*tmpX)); // dis -> dir

				// Check for min and max
				// Dir
				if (depthPointCloud[offset] < currentMinDir) { // dir
					currentMinDir = depthPointCloud[offset]; // dir
					tmpX++;
				} else {
					if (depthPointCloud[offset] > currentMaxDir) { // dir
						currentMaxDir = depthPointCloud[offset]; // dir
						tmpX++;
					}
				}
				// Height
				if (depthPointCloud[--offset] < currentMinHeight) { // height 
					currentMinHeight = depthPointCloud[offset]; // height
				}
				offset += 3; // height -> next
			}
		}
	}
		
	// Determine height slice (polar coordinates)
	float heightDiffList[640/DEPTH_SCALE_FACTOR];
	for (int i = 0; i < 640/DEPTH_SCALE_FACTOR; i++) {
		heightSlices[i*2] = 999999.0f;
		heightDiffList[i] = 999999.0f;
	}
	currentMinHeight += 150.0;
	offset = 0;
	imOffset = 0;
	ijOffset = 0;
	float factor = (640.0f/DEPTH_SCALE_FACTOR)/(currentMaxDir - currentMinDir + 0.000001f);
	for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*(480/DEPTH_SCALE_FACTOR); i++) {
		if (depthPointCloud[offset++] == 999999.0f) { // height -> dir
			offset += 2; // -> next
			imOffset++;
			ijOffset += 2;
		} else {
			int dirIndex = (int)floor((depthPointCloud[offset--]-currentMinDir)*factor); // dir -> height
			float heightDiff = abs(depthPointCloud[offset++]-currentMinHeight); // height -> dir
			if ((heightDiff < 10) && (heightDiff < heightDiffList[dirIndex])) {
				if (abs(dirIndex) > 40) {
					dirIndex++;
				}
				heightDiffList[dirIndex] = heightDiff;
				heightSlices[dirIndex*2] = depthPointCloud[offset++]; // dir -> dis
				heightSlices[(dirIndex*2)+1] = depthPointCloud[offset++]; // dis -> next
				heightSliceColors[dirIndex] = colorPointCloud[imOffset++]; // Set color value
				heightSliceIJ[dirIndex*2] = ijPointCloud[ijOffset++]; // i
				heightSliceIJ[(dirIndex*2)+1] = ijPointCloud[ijOffset++]; // j
			} else {
				offset += 2; // -> next
				imOffset++;
				ijOffset += 2;
			}
		}
	}
		
	// Convert Slices to Cartesian
	for (int i=0; i < (640/DEPTH_SCALE_FACTOR)*2; ) {
		if (heightSlices[i] != 999999.0f) {
			float tmpDir = heightSlices[i++]; // Dir -> Dis
			float tmpDis = heightSlices[i--]; // Dis -> Dir
			heightSlices[i++] = tmpDis*cos(tmpDir); // x -> y
			heightSlices[i++] = tmpDis*sin(tmpDir); // y -> next
		} else {
			i+=2; // -> next
		}
	}

#ifdef RECORD_SLICES
	if (mColorFrameOn > 0) {
		recordSlices(heightSlices, heightSliceColors, 640/DEPTH_SCALE_FACTOR, outFileOn);
	}
#endif

	currentMinHeight -= 150;
	sensorHeight = -currentMinHeight;
}