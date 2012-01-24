#include "algorithm.h"
#include "globals.h"
#include "record.h"
#include <math.h>

#include <iostream>

using namespace std;

void runAlgorithm() {
	// DECLARE POINT CLOUD DATA as Local Array of Floats (40x30x3 on Stack)
	// DECLARE COLOR DATA as Local Array of Floats (40*30 on Stack)
	float depthPointCloud[CLOUD_SIZE*3]; // Stored in the order of [x,y,z]
	float colorPointCloud[CLOUD_SIZE];
	int ijPointCloud[CLOUD_SIZE*2]; // Stored in the order of [i,j]
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
			tmpZ = ((float)(K->mDepthBuffer[(j*640)+i]));
				
			//Check Sensor Data for Error
			if (tmpZ == 2047.0f) {
				depthPointCloud[offset] = 999999.0f;
				origZ[imOffset] = 0.0f;
				offset += 3; // -> next
				imOffset++;
				ijOffset += 2;
			} else {
				// Depth to Z
				if (origZ[imOffset] != 0.0f) {
					origZ[imOffset] = tmpZ = (-100.0f/((-0.00307f * tmpZ) + 3.33f))*(1-CLOUD_AVG_FACTOR)
						+ CLOUD_AVG_FACTOR*origZ[imOffset]; //z -> y
				} else {
					origZ[imOffset] = tmpZ = (-100.0f/((-0.00307f * tmpZ) + 3.33f)); //z -> y
				}
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
	float dirSection = 1/dirFactor;
	heightValue = -currentMinHeight;

	// SECOND PASS
	// Determine Floor Points
	// Find max distances for each direction
	float pWallSliceNan[NUM_SLICES*2];
	for (int i = 0; i < NUM_SLICES*2; i++) {
		pWallSliceNan[i] = -999999.0f;
	}
	currentMinHeight += 25.0f;
	offset = 0;
	ijOffset = 0;
	int fpIjOffset = 0;
	int fpOffset = 0;
	numFloorPoints = 0;
	for (int i = 0; i < CLOUD_SIZE; i++) {
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
				floorIJ[fpIjOffset++] = ijPointCloud[ijOffset++];
			} 

			// Determine max Distances for each Direction
			int dirIndex = (int)floor((tmpDir-currentMinDir)*dirFactor)*2;
			if (tmpDis > pWallSliceNan[dirIndex+1]) {
				pWallSliceNan[dirIndex  ] = offset - 1; //Distance array location
				pWallSliceNan[dirIndex+1] = tmpDis;
			}
		}
	}
	currentMinHeight -= 25.0f;

	//Remove NaNs and distances outside of max
	for (int iStat=0; iStat < NUM_SLICES; iStat++) {
		wallStatus[iStat] = 0;
	}
	float pWallSlice[NUM_SLICES*2];
	int newIndex = 0;
	int statOn = 0;
	bool fromOut = false;
	float prevDis = 0.0f;
	for (int i = 1; i < NUM_SLICES; i++) {
		int startIndex = pWallSliceNan[i*2];
		float tmpDis = pWallSliceNan[(i*2)+1];
		if (startIndex != -999999.0f && tmpDis < MAX_ALLOWED_DIS) {
			float tmpDir = -(currentMinDir + (i*dirSection) + (dirSection/2.0f));
			cWallSlice[newIndex++] = tmpDis*cos(tmpDir);
			cWallSlice[newIndex--] = -tmpDis*sin(tmpDir);
			pWallSlice[newIndex++] = tmpDir;
			pWallSlice[newIndex++] = tmpDis;
			if (fromOut) {
				if (tmpDis < MAX_ALLOWED_DIS-50.0f) {
					wallStatus[statOn] = 2;
				} else {
					wallStatus[statOn] = 1;
				}
				fromOut = false;
			}
			statOn++;
			prevDis = tmpDis;
		} else if (tmpDis >= MAX_ALLOWED_DIS) {
			if (statOn != 0) {
				if (prevDis < MAX_ALLOWED_DIS-50.0f) {
					wallStatus[statOn-1] = 2;
				} else {
					wallStatus[statOn-1] = 1;
				}
			}
			fromOut = true;
		}
	}
	numWallSlicePts = newIndex/2;
	if (wallStatus[0] == 0) {
		wallStatus[0] = 1;
	}
	if (wallStatus[numWallSlicePts-1] == 0) {
		wallStatus[numWallSlicePts-1] = 1;
	}

	for (int i = 0; i < numWallSlicePts-1; ) { 
		float origX = cWallSlice[(i*2)+0];
		float origZ = cWallSlice[(i*2)+1];
		float prevDisToPoint = -1.0f;

		float sumX = origX;
		float sumZ = origZ;
		float sumXsquared = origX * origX;
		float sumZsquared = origZ * origZ;
		float sumXZ = origX * origZ;
		int   nPoints = 1;
		float prevError = 999999.0;
		int   cornersInARow = 1;

		int   nextPoint = i+1;
		for (int j = 0; j < 39; j++) {
			stdErrorList[j] = 999999.0f;
		}
		for (int j = i+1; j < numWallSlicePts; j++) {
			float ptX = cWallSlice[(j*2)+0];
			float ptZ = cWallSlice[(j*2)+1];
			float delX = ptX - origX;
			float delZ = ptZ - origZ;
			float disToPoint = sqrt((delX*delX)+(delZ*delZ));
			nPoints++;
			sumX += ptX;
			sumZ += ptZ;
			sumXsquared += ptX * ptX;
			sumZsquared += ptZ * ptZ;
			sumXZ += ptX * ptZ;

			float stdError;
			if (nPoints > 2 && fabs( float(nPoints) * sumXsquared - sumX * sumX) > FLT_EPSILON) {
				float b = ( float(nPoints) * sumXZ - sumZ * sumX) /
					( float(nPoints) * sumXsquared - sumX * sumX);
				float a = (sumZ - b * sumX) / float(nPoints);

				float sx  = b * ( sumXZ - sumX * sumZ / float(nPoints) );
				float sz2 = sumZsquared - sumZ * sumZ / float(nPoints);
				float sz  = sz2 - sx;

				//float coefD = sx / sz2;
				//float coefC = sqrt(coefD);
				stdError = sqrt(sz / float(nPoints - 2));
			} else {
				stdError = 0.0f;
			}
			stdErrorList[j-1] = stdError;

			if (prevError != 999999.0f) {
				float errorDiff = abs(stdError - prevError);
				if (errorDiff > 4.0f) {
					wallStatus[j-1] = 2;
					nextPoint = j-1;
					break;
					/*if (cornersInARow < 2) {
						cornersInARow++;
						wallStatus[j-1] = 2;
					} else {
						nextPoint = j-1;
						break;
					}*/
				} else {
					cornersInARow = 0;
				}
			}
			prevError = stdError;
			
			/*if (prevDisToPoint != -1) {
				if (abs(disToPoint - prevDisToPoint) < 10.0f) {
					prevDisToPoint = (disToPoint/nPoints)*(nPoints+1);
				} else {
					wallStatus[j] = 2;
					nextPoint = j;
					break;
				}
			} else {
				prevDisToPoint = disToPoint*2.0f;
			}*/
		}
		i = nextPoint;
	}
	

#ifdef RECORD_SLICES
	recordSlices(pWallSlice, numWallSlicePts, outFileOn);
#endif

	if (yawValue == 999999.0) { //First Frame
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
	for (int i = 0; i < CLOUD_SIZE; i++) {
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
			if (tmpDis > pWallSliceNan[(dirIndex*2)+1] - 20) {
				numWallPoints++;
				wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // i -> j
				wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // j -> next
			} else {
				ijOffset += 2;
			}
		}
	}

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