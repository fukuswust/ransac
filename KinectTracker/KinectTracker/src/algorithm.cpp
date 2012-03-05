#include "algorithm.h"
#include "globals.h"
#include "record.h"
#include <math.h>

#include <iostream>

using namespace std;

void runAlgorithm() {
	// DECLARE POINT CLOUD DATA as Local Array of Floats (40x30x3 on Stack)
	float depthPointCloud[CLOUD_SIZE*3]; // Stored in the order of [x,y,z]
	// DECLARE COLOR DATA as Local Array of Floats (40x30 on Stack)
	float colorPointCloud[CLOUD_SIZE]; // Stored value for greyscale
	// DECLARE IJ DATA as Local Array of Floats (40x30x2 on Stack)
	int ijPointCloud[CLOUD_SIZE*2]; // Stored in the order of [i,j]
	// Common iterators used in phases
	int offset = 0;
	int imOffset = 0;
	int ijOffset = 0;

	// GET ROLL AND PITCH VALUES
	float xAccelAvg = xAccel;
	float yAccelAvg = yAccel;
	float zAccelAvg = zAccel;
	rollValue = atan2(xAccelAvg, yAccelAvg);
	pitchValue = atan2(zAccelAvg, yAccelAvg);

	#pragma region FIRST PASS
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
	heightValue = -currentMinHeight;
	#pragma endregion Convert depth to polar, determine color, and mins/maxes

	#pragma region SECOND PASS
	// Determine Floor Points
	// Find max distances for each direction
	//float pWallSliceNan[NUM_SLICES*2];
	for (int i = 0; i < NUM_SLICES; i++) {
		wallSlicePoints[i].dis = -999999.0f;
		wallSlicePoints[i].dir = -999999.0f;
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
			} else {
				ijOffset+=2;
			}

			// Determine max Distances for each Direction
			int dirIndex = (int)floor((tmpDir+(3.0f*PI/4.0))/(PI/2.0f/40.0f));
			if (dirIndex >= 0 && dirIndex < NUM_SLICES) {
				if (tmpDis > wallSlicePoints[dirIndex].dis && tmpY > currentMinHeight && tmpDis < MAX_ALLOWED_DIS) {
					wallSlicePoints[dirIndex].dir = tmpDir;
					wallSlicePoints[dirIndex].dis = tmpDis;
				}
			}
		}
	}
	currentMinHeight -= 25.0f;
	#pragma endregion Determine floor points and max distances for each direction

	// Determine X, Z, and Yaw from slices and update maps
	//compareToLocalMap(); 
	
	// Set augmentation transformation variables
	setPositionAndOrientation();
		
	#pragma region THIRD PASS
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
			if (tmpDis > wallSlicePoints[dirIndex].dis - 20) {
				numWallPoints++;
				wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // i -> j
				wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // j -> next
			} else {
				ijOffset += 2;
			}
		}
	}
	currentMinHeight -= 150;
	#pragma endregion
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

void setPositionAndOrientation() {
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

void compareToLocalMap() {
	if (yawValue != 999999.0) { //Not in initial state
		// Convert to Cartesian
		for (int i = 0; i < NUM_SLICES; i++) {
			float tmpDis = wallSlicePoints[i].dis;
			if (tmpDis != -999999.0) {
				float tmpDir = wallSlicePoints[i].dir;
				wallSlicePoints[i].x = tmpDis*cos(tmpDir);
				wallSlicePoints[i].z = tmpDis*sin(tmpDir);
			}
			tmpDis = localMapPoints[i].dis;
			if (tmpDis != -999999.0) {
				float tmpDir = localMapPoints[i].dir;
				localMapPoints[i].x = tmpDis*cos(tmpDir);
				localMapPoints[i].z = tmpDis*sin(tmpDir);
			}
		}

		// Iterate
		float incAmount = PI/720.0f;
		for (int i = 0; i < 40; i++) {
			moveOffsets(wallSlicePoints, localMapPoints, closestLocal);
			float be = determineError(wallSlicePoints, localMapPoints, closestLocal);
			performRotation(wallSlicePoints, incAmount);
			float rre = determineError(wallSlicePoints, localMapPoints, closestLocal);
			performRotation(wallSlicePoints, -incAmount*2.0f);
			float rle = determineError(wallSlicePoints, localMapPoints, closestLocal);
			if (be <= rre && be <= rle) {
				performRotation(wallSlicePoints, incAmount);
			} else if (rre <= rle) {
				performRotation(wallSlicePoints, incAmount*2.0f);
				yawValue += incAmount;
			} else {
				yawValue -= incAmount;
			}

			float delX, delZ;
			int numMatches;
			numMatches = minimizeTranslationError(wallSlicePoints, localMapPoints, closestLocal, delX, delZ);
			if (numMatches > 4) {
				xValue += delX;
				zValue += delZ;
				performTranslation(wallSlicePoints, delX, delZ);
			}
		}

		// Convert to Polar
		for (int i = 0; i < NUM_SLICES; i++) {
			float tmpX = wallSlicePoints[i].x;
			if (tmpX != -999999.0) {
				float tmpZ = wallSlicePoints[i].z;
				wallSlicePoints[i].dis = sqrt((tmpX*tmpX)+(tmpZ*tmpZ));
				wallSlicePoints[i].dir = atan2(tmpZ,tmpX);
			}
		}

	} else { //In initial state
		numGlobalPoints = 0;
		for (int i=0; i<NUM_SLICES; i++) { //Create global map from first view
			if (wallSlicePoints[i].dis != -999999.0) {
				float tmpDis = wallSlicePoints[i].dis;
				float tmpDir = wallSlicePoints[i].dir;
				globalMapPoints[numGlobalPoints].x = tmpDis*cos(tmpDir);
				globalMapPoints[numGlobalPoints].z = tmpDis*sin(tmpDir);
				numGlobalPoints++;
			}
		}
		if (numGlobalPoints > 20) { //Enough points to start map
			yawValue = 0.0f;
			xValue = 0.0f;
			zValue = 0.0f;
			for (int i = 0; i < NUM_SLICES; i++) {
				closestLocal[i] = i;
			}
		} else { //Not enough points to start map
			numGlobalPoints = 0;
		}

		// Convert to Cartesian
		for (int i = 0; i < NUM_SLICES; i++) {
			float tmpDis = wallSlicePoints[i].dis;
			if (tmpDis != -999999.0) {
				float tmpDir = wallSlicePoints[i].dir;
				wallSlicePoints[i].x = tmpDis*cos(tmpDir);
				wallSlicePoints[i].z = tmpDis*sin(tmpDir);
			}
		}
	}

	// UPDATE MAPS
	//Update Local Map
	for (int i=0; i < NUM_SLICES; i++) {
		localMapPoints[i].dis = 999999.0f;
	}
	for (int i=0; i < numGlobalPoints; i++) {
		float tmpX = globalMapPoints[i].x;
		float tmpZ = globalMapPoints[i].z;
		float delX = tmpX - xValue;
		float delZ = tmpZ - zValue;
		float tmpDis = sqrt((delX*delX)+(delZ*delZ));
		if (tmpDis < MAX_ALLOWED_DIS) {
			float tmpDir = atan2(delZ,delX) - yawValue;
			int dirIndex = (int)floor((tmpDir+(3.0f*PI/4.0))/(PI/2.0f/40.0f));	
			if (dirIndex >= 0 && dirIndex < NUM_SLICES) {
				if (tmpDis < localMapPoints[dirIndex].dis) {
					localToGlobal[dirIndex] = i;
					localMapPoints[dirIndex].dir = tmpDir;
					localMapPoints[dirIndex].dis = tmpDis;
				}
			}
		}
	}

	for (int i=0; i < NUM_SLICES; i++) {
		if (localMapPoints[i].dis == 999999.0f) {
			localMapPoints[i].dis == -999999.0f;
			localMapPoints[i].dir == -999999.0f;
		}
	}

	// Average in new wall slice
	float rate = 0.1;
	for (int i=0; i < NUM_SLICES; i++) {
		if (localMapPoints[i].dis == 999999.0f) { //Missing local model point
			if (wallSlicePoints[i].dis != -999999.0f) { //Available wall slice: Add
				if (numGlobalPoints < 1023) {
					localMapPoints[i].dis = wallSlicePoints[i].dis;
					localMapPoints[i].dir = wallSlicePoints[i].dir;
					localToGlobal[i] = numGlobalPoints;
					numGlobalPoints++;
				}
			} else { //No new points
				localMapPoints[i].dis = -999999.0f;
				localMapPoints[i].dir = -999999.0f;
			}
		} else {
			if (wallSlicePoints[i].dis != -999999.0f) { //Available wall slice: Average
				localMapPoints[i].dis = (localMapPoints[i].dis*(1-rate))+(wallSlicePoints[i].dis*rate);
				localMapPoints[i].dir = (localMapPoints[i].dir*(1-rate))+(wallSlicePoints[i].dir*rate);
			}
		}
	}

	// Update Global Model
	for (int i=0; i < NUM_SLICES; i++) {
		float tmpDis = localMapPoints[i].dis;
		if (tmpDis != -999999.0f) {
			float tmpDir = localMapPoints[i].dir + yawValue;
			int tmpIndex = localToGlobal[i];
			globalMapPoints[tmpIndex].x = (tmpDis*cos(tmpDir))+xValue;
			globalMapPoints[tmpIndex].z = (tmpDis*sin(tmpDir))+zValue;
		}
	}
	
}

void  moveOffsets(SlicePoint set1[] , SlicePoint set2[], int closestLocal[]) {
	for (int i = 0; i < NUM_SLICES; i++) {
		int link = closestLocal[i];
		// Calculate current link error
		float delX = (set1[i].x-set2[link].x);
		float delZ = (set1[i].z-set2[link].z);
		float curMin = (delX*delX) + (delZ*delZ);
		int curMinI = link;
		// Calculate previous link error
		if (link != 0) {
			delX = (set1[i].x-set2[link-1].x);
			delZ = (set1[i].z-set2[link-1].z);
			float err = (delX*delX) + (delZ*delZ);
			if (err < curMin) {
				curMin = err;
				curMinI = link-1;
			}
		}
		if (link != NUM_SLICES) {
			delX = (set1[i].x-set2[link+1].x);
			delZ = (set1[i].z-set2[link+1].z);
			float err = (delX*delX) + (delZ*delZ);
			if (err < curMin) {
				curMinI = link+1;
			}
		}
		closestLocal[i] = curMinI;
	}
}

float determineError(SlicePoint set1[] , SlicePoint set2[], int closestLocal[]) {
	float tot = 0.0f;
	for (int i = 0; i < NUM_SLICES; i++) {
		int link = closestLocal[i];
		if (set1[i].x != -999999.0f && set2[link].x != -999999.0f) {
			float delX = (set1[i].x-set2[link].x);
			float delZ = (set1[i].z-set2[link].z);
			tot += (delX*delX) + (delZ*delZ);
		}
	}
	return tot;
}

void performRotation(SlicePoint set[], float rot) {
	for (int i = 0; i < NUM_SLICES; i++) {
		float tmpX = set[i].x;
		if (tmpX != -999999.0) {
			float tmpZ = set[i].z;
			float tmpDis = sqrt((tmpX*tmpX)+(tmpZ*tmpZ));
			float tmpDir = atan2(tmpZ,tmpX)+rot;
			set[i].x = tmpDis*cos(tmpDir);
			set[i].z = tmpDis*sin(tmpDir);
		}
	}
}

void performTranslation(SlicePoint set[], float x, float z) {
	for (int i = 0; i < NUM_SLICES; i++) {
		if (set[i].x != -999999.0) {
			set[i].x += x;
			set[i].z += z;
		}
	}
}

int  minimizeTranslationError(SlicePoint set1[] , SlicePoint set2[], int closestLocal[], float &delX, float &delZ) {
	delX = 0.0f;
	delZ = 0.0f;
	int num = 0;
	for (int i = 0; i < NUM_SLICES; i++) {
		int link = closestLocal[i];
		if (set1[i].x != -999999.0f && set2[link].x != -999999.0f) {
			delX += (set2[link].x-set1[i].x);
			delZ += (set2[link].z-set1[i].z);
			num++;
		}
	}

	#define MAX_TRANS 5.0f
	delX /= num;
	if (delX > MAX_TRANS) {
		delX = MAX_TRANS;
	} else if (delX < -MAX_TRANS) {
		delX = -MAX_TRANS;
	}
	delZ /= num;
	if (delZ > MAX_TRANS) {
		delZ = MAX_TRANS;
	} else if (delZ < -MAX_TRANS) {
		delZ = -MAX_TRANS;
	}
	return num;
}