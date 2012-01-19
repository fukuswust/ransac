#include "algorithm.h"
#include "globals.h"
#include <math.h>

#include <iostream>

using namespace std;

void runAlgorithm() {
	// DECLARE POINT CLOUD DATA as Local Array of Floats (40x30x3 on Stack)
	// DECLARE COLOR DATA as Local Array of Floats (40*30 on Stack)
	float depthPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))*3]; // Stored in the order of [x,y,z]
	float colorPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))];
	int ijPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))*2]; // Stored in the order of [i,j]
	int offset = 0;
	int imOffset = 0;
	int ijOffset = 0;

	// GET ROLL AND PITCH VALUES
	float xAccelAvg = xAccel;
	float yAccelAvg = yAccel;
	float zAccelAvg = zAccel;
	rollValue = atan2(xAccelAvg, yAccelAvg);
	pitchValue = atan2(zAccelAvg, yAccelAvg);
	
	// GET GRAVITY ROTATION MATRIX (Simlified from MATLAB code for UP vector = [0 1 0])
	findRotationToUp(xAccelAvg, yAccelAvg, zAccelAvg);
	float R11 = pitchRollMatrix[0];
	float R12 = pitchRollMatrix[1];
	float R13 = pitchRollMatrix[2];
	float R21 = pitchRollMatrix[3];
	float R22 = pitchRollMatrix[4];
	float R23 = pitchRollMatrix[5];
	float R31 = pitchRollMatrix[6];
	float R32 = pitchRollMatrix[7];
	float R33 = pitchRollMatrix[8];

	// SET UP MIN AND MAX PLACEHOLDERS
	float currentMinHeight = 999999.0f;
	float currentMinDir = 999999.0f;
	float currentMaxDir = -999999.0f;

	// FIRST PASS
	// Convert depth data to cartesian point cloud data aligned to initial vector
	// Detect min height, min direction, and max direction
	// Assign color data to each point
	for (int j = 0; j < 480; j+=DEPTH_SCALE_FACTOR) {
		for (int i = 0; i < 640; i+=DEPTH_SCALE_FACTOR) {
			//Acquire Raw Depth Value
			float tmpX, tmpX2, tmpY, tmpY2, tmpZ, tmpZ2, fi, fj;
			tmpZ = (float)(K->mDepthBuffer[(j*640)+i]);
				
			//Check Sensor Data for Error
			if (tmpZ == 2047.0f) {
				depthPointCloud[offset] = 999999.0f;
				offset += 3; // -> next
				imOffset++;
				ijOffset += 2;
			} else {
				// Depth to Z
				tmpZ = -100.0f/((-0.00307f * tmpZ) + 3.33f); //z -> y
					
				// Z to Point Cloud
				tmpX = (float)(i - 320) * (tmpZ - 10.0f) * -0.0021f;
				tmpY = (float)(j - 240) * (tmpZ - 10.0f) * 0.0021f ;

				// Determine fi,fj for color data
				fi = ((( tmpX - 1.8f) / 0.0023f)/ (-tmpZ - 10)) + 320.0f - 1.0f;
				fj = (((-tmpY - 2.4f) / 0.0023f)/ (-tmpZ - 10)) + 240.0f - 1.0f;

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
				depthPointCloud[offset++] = tmpX2 = (tmpX*R11)+(tmpY*R21)+(tmpZ*R31); // x -> y
				depthPointCloud[offset++] = tmpY2 = (tmpX*R12)+(tmpY*R22)+(tmpZ*R32);  // y	-> z		
				depthPointCloud[offset++] = tmpZ2 = (tmpX*R13)+(tmpY*R23)+(tmpZ*R33);  // z -> next

				// Check for min and max Dir
				float dir = atan2(tmpZ2,tmpX2);
				if (dir < currentMinDir) { // dir
					currentMinDir = dir; // dir
				} 
				if (dir > currentMaxDir) { // dir
					currentMaxDir = dir; // dir
				}

				// Find Min Height from Y's
				if (tmpY2 < currentMinHeight) { 
					currentMinHeight = tmpY2;
				}
			}
		}
	}
	float dirFactor = (640.0f/DEPTH_SCALE_FACTOR)/(currentMaxDir - currentMinDir + 0.000001f);
	heightValue = -currentMinHeight;

	// SECOND PASS
	// Determine Floor Points
	// Find max distances for each direction
	float maxDisPerDirList[(640/DEPTH_SCALE_FACTOR)*2];
	float maxDisPerDirListIJ[(640/DEPTH_SCALE_FACTOR)*2];
	for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*2; i++) {
		maxDisPerDirList[i] = -999999.0f;
	}
	currentMinHeight += 25.0f;
	offset = 0;
	ijOffset = 0;
	int fpIjOffset = 0;
	int fpOffset = 0;
	numFloorPoints = 0;
	for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*(480/DEPTH_SCALE_FACTOR); i++) {
		float tmpDir, tmpDis;
		float tmpX = depthPointCloud[offset++];
		if (tmpX == 999999.0f) { // height -> dir
			offset += 2; // -> next
			ijOffset += 2;
		} else {
			// Set temporary variables
			float tmpY = depthPointCloud[offset++];
			float tmpZ = depthPointCloud[offset];
			offset -= 2;

			// Convert to polar coordinates
			depthPointCloud[offset++] = tmpY; // height -> dir
			depthPointCloud[offset++] = tmpDir = atan2(tmpZ,tmpX); // dir -> dis
			depthPointCloud[offset++] = tmpDis = sqrt((tmpZ*tmpZ)+(tmpX*tmpX)); // dis -> dir

			// Determine Floor Points
			if ((fpOffset < MAX_FLOOR_POINTS*3) && (tmpY < currentMinHeight)) {
				numFloorPoints++;
				floorPoints[fpOffset++] = tmpX;
				floorPoints[fpOffset++] = tmpY;
				floorPoints[fpOffset++] = tmpZ;
				floorIJ[fpIjOffset++] = ijPointCloud[ijOffset++];
				floorIJ[fpIjOffset++] = ijPointCloud[ijOffset--];
			} 

			// Determine max Distances for each Direction
			int dirIndex = (int)floor((tmpDir-currentMinDir)*dirFactor)*2;
			if (tmpDis > maxDisPerDirList[dirIndex+1]) {
				maxDisPerDirList[dirIndex  ] = tmpDir;
				maxDisPerDirList[dirIndex+1] = tmpDis;
				maxDisPerDirListIJ[dirIndex ] = ijPointCloud[ijOffset++];
				maxDisPerDirListIJ[dirIndex+1] = ijPointCloud[ijOffset++];
			} else {
				ijOffset += 2;
			}
		}
	}
	currentMinHeight -= 25.0f;

	// Convert to cartesian coordinates and remove NaNs
	int newIndex = 0;
	for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*2; i+=2) {
		float tmpDir = maxDisPerDirList[i];
		if (tmpDir != -999999.0f) {
			float tmpDis = maxDisPerDirList[i+1];
			curWallSlice[newIndex++] = tmpDis*cos(tmpDir);
			curWallSlice[newIndex++] = -tmpDis*sin(tmpDir);
		}
	}
	numCurWallSlices = newIndex/2;

	if (yawValue == 999999.0) { //First Frame
		//Toggle wall slice arrays
		double *tmpWallSlice = curWallSlice;
		curWallSlice = prevWallSlice;
		prevWallSlice = tmpWallSlice;
		numPrevWallSlices = numCurWallSlices;

		// Set final values
		yawValue = 0;
		xValue = 0;
		zValue = 0;

		// Camera Orientation
		// Translation
		translationMatrix[0] = xValue; // x translation
		translationMatrix[1] = heightValue; // height translation
		translationMatrix[2] = zValue; // z translation

		// Yaw Rotation
		yawMatrix[0] = cos(-yawValue);
		yawMatrix[1] = 0;
		yawMatrix[2] = -sin(-yawValue);
		yawMatrix[3] = 0;
		yawMatrix[4] = 1;
		yawMatrix[5] = 0;
		yawMatrix[6] = sin(-yawValue);
		yawMatrix[7] = 0;
		yawMatrix[8] = cos(-yawValue);
	} else {
		//Toggle wall slice arrays
		double *tmpWallSlice = curWallSlice;
		curWallSlice = prevWallSlice;
		prevWallSlice = tmpWallSlice;
		numPrevWallSlices = numCurWallSlices;

		// Camera Orientation
		// Translation
		translationMatrix[0] = xValue; // x translation
		translationMatrix[1] = heightValue; // height translation
		translationMatrix[2] = zValue; // z translation

		// Yaw Rotation
		yawMatrix[0] = cos(-yawValue);
		yawMatrix[1] = 0;
		yawMatrix[2] = -sin(-yawValue);
		yawMatrix[3] = 0;
		yawMatrix[4] = 1;
		yawMatrix[5] = 0;
		yawMatrix[6] = sin(-yawValue);
		yawMatrix[7] = 0;
		yawMatrix[8] = cos(-yawValue);
	}
		
	// Determine height slice (in polar coordinates)
	// Determine wall points
	numWallPoints = 0;
	float heightDiffList[640/DEPTH_SCALE_FACTOR];
	for (int i = 0; i < 640/DEPTH_SCALE_FACTOR; i++) {
		heightSlices[i*2] = 999999.0f;
		heightDiffList[i] = 999999.0f;
	}
	currentMinHeight += 150.0;
	offset = 0;
	imOffset = 0;
	ijOffset = 0;
	int wijOffset = 0;
	for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*(480/DEPTH_SCALE_FACTOR); i++) {
		float tmpHeight = depthPointCloud[offset++];
		if (tmpHeight == 999999.0f) { // height -> dir
			offset += 2; // -> next
			imOffset++;
			ijOffset += 2;
		} else {
			// Set temporary variables
			float tmpDir = depthPointCloud[offset++]; // dir -> height
			float tmpDis = depthPointCloud[offset++];   // height -> next

			// Find Height Slice
			int dirIndex = (int)floor((tmpDir-currentMinDir)*dirFactor); 
			float heightDiff = abs(tmpHeight - currentMinHeight);
			if ((heightDiff < 10) && (heightDiff < heightDiffList[dirIndex])) {
				heightDiffList[dirIndex] = heightDiff;
				heightSlices[dirIndex*2] = tmpDir;
				heightSlices[(dirIndex*2)+1] = tmpDis;
				heightSliceColors[dirIndex] = colorPointCloud[imOffset++]; // Set color value
				heightSliceIJ[dirIndex*2] = ijPointCloud[ijOffset++]; // i -> j
				heightSliceIJ[(dirIndex*2)+1] = ijPointCloud[ijOffset--]; // j -> i
			} else {
				imOffset++;
			}

			// Find Wall Points
			if (tmpDis > maxDisPerDirList[(dirIndex*2)+1] - 20) {
				numWallPoints++;
				wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // i -> j
				wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // j -> next
			} else {
				ijOffset += 2;
			}
		}
	}
		
	// Convert Slices to Cartesian
	cartesianToPolar2D(heightSlices, 640/DEPTH_SCALE_FACTOR);

#ifdef RECORD_SLICES
	if (mColorFrameOn > 0) {
		recordSlices(heightSlices, heightSliceColors, 640/DEPTH_SCALE_FACTOR, outFileOn);
	}
#endif

	currentMinHeight -= 150;
}

void cartesianToPolar2D(float inArray[], int inSize) {
	for (int i=0; i < inSize*2; ) {
		if (inArray[i] != 999999.0f) {
			float tmpDir = inArray[i++]; // Dir -> Dis
			float tmpDis = inArray[i--]; // Dis -> Dir
			inArray[i++] = tmpDis*cos(tmpDir); // x -> y
			inArray[i++] = tmpDis*sin(tmpDir); // y -> next
		} else {
			i+=2; // -> next
		}
	}
}

void findRotationToUp(float xVect, float yVect, float zVect) {
	//Get unit vector and magnitude of gravity
	float gravMag = sqrt((xVect*xVect)+(yVect*yVect)+(zVect*zVect)); //Quality = diff from 819/512
	float uGravX = xVect/gravMag;
	float uGravY = yVect/gravMag;
	float uGravZ = zVect/gravMag;
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
	pitchRollMatrix[0] = uGravY + (t*uX*uX);
	pitchRollMatrix[1] = -sz;
	pitchRollMatrix[2] = t*xz;
	pitchRollMatrix[3] = sz;
	pitchRollMatrix[4] = uGravY;
	pitchRollMatrix[5] = -sx;
	pitchRollMatrix[6] = t*xz;
	pitchRollMatrix[7] = sx;
	pitchRollMatrix[8] = uGravY + (t*uZ*uZ);
}