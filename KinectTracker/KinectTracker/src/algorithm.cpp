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
	float dirSection = 1/dirFactor;
	heightValue = -currentMinHeight;
	#pragma endregion Convert depth to polar, determine color, and mins/maxes

	#pragma region SECOND PASS
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
			} else {
				ijOffset+=2;
			}

			// Determine max Distances for each Direction
			int dirIndex = (int)floor((tmpDir-currentMinDir)*dirFactor)*2;
			if (tmpDis > pWallSliceNan[dirIndex+1]) {
				pWallSliceNan[dirIndex  ] = -tmpDir;//offset - 1; //Distance array location
				pWallSliceNan[dirIndex+1] = tmpDis;
			}
		}
	}
	currentMinHeight -= 25.0f;
	#pragma endregion Determine floor points and max distances for each direction

	//Remove NaNs and distances outside of max
	float pWallSlice[NUM_SLICES*2];
	float cWallSlice[NUM_SLICES*2];
	int wallStatus[NUM_SLICES];
	int numWallSlicePts = 0;
	numWallSlicePts = sliceRemoveOutsideRange(pWallSliceNan, cWallSlice, pWallSlice, wallStatus);

#ifdef RECORD_SLICES
	//Record slice data
	recordSlices(pWallSlice, numWallSlicePts, outFileOn);
#endif

	// Detect corner points
	sliceDetectCorners(cWallSlice, pWallSlice, wallStatus, numWallSlicePts);

	// Loop through determined corner values and compare to the global model
	// Update model points, translation, and orientation
	//updateGlobalMap();

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

int sliceRemoveOutsideRange(float pWallSliceNan[], float cWallSlice[], float pWallSlice[], int wallStatus[]) {
	for (int iStat=0; iStat < NUM_SLICES; iStat++) {
		wallStatus[iStat] = 0;
	}
	int newIndex = 0;
	int statOn = 0;
	bool fromOut = false;
	float prevDis = pWallSliceNan[1];
	for (int i = 1; i < NUM_SLICES-1; i++) {
		int startIndex = pWallSliceNan[i*2];
		float tmpDis = pWallSliceNan[(i*2)+1];
		float tmpNxtDis = pWallSliceNan[(i*2)+3];
		float diffLeftDis = abs(tmpDis - prevDis);
		float diffRightDis = abs(tmpNxtDis - tmpDis);
		bool  isNoise = (min(diffLeftDis,diffRightDis) > 35.0f);
		if (startIndex != -999999.0f && tmpDis < MAX_ALLOWED_DIS && !isNoise) {
			float tmpDir = pWallSliceNan[(i*2)+0];
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
		} else if (tmpDis >= MAX_ALLOWED_DIS && !isNoise) {
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
	int numWallSlicePts = newIndex/2;
	if (wallStatus[0] == 0) {
		wallStatus[0] = 1;
	}
	if (wallStatus[numWallSlicePts-1] == 0) {
		wallStatus[numWallSlicePts-1] = 1;
	}
	return numWallSlicePts;
}

void sliceDetectCorners(float cWallSlice[], float pWallSlice[], int wallStatus[], int numWallSlicePts) {
	float prevLineM = 999999.0;
	float prevLineB = 999999.0;
	numCorners = 0;
	for (int i = 0; i < numWallSlicePts-1; ) { 
		float origX = cWallSlice[(i*2)+0];
		float origZ = cWallSlice[(i*2)+1];
		float prevDisToPoint = 0.0f;

		// Initialize Regression
		float sumX = origX;
		float sumZ = origZ;
		float sumXsquared = origX * origX;
		float sumZsquared = origZ * origZ;
		float sumXZ = origX * origZ;
		int   nPoints = 1;
		float prevX = 999999.0f;

		int   nextPoint = i+1;
		bool endStrip = false;
		for (int j = i+1; j < numWallSlicePts; j++) {
			float ptX = cWallSlice[(j*2)+0];
			float ptZ = cWallSlice[(j*2)+1];
			float delX = ptX - origX;
			float delZ = ptZ - origZ;
			float disToPoint = sqrt((delX*delX)+(delZ*delZ));

			// Check for Huge Jumps
			if (disToPoint - prevDisToPoint > 40.0f) {
				float leftDisFromCam = pWallSlice[(j*2)-1];
				float rightDisFromCam = pWallSlice[(j*2)+1];
				float camDisDiff = abs(leftDisFromCam - rightDisFromCam);
				if (leftDisFromCam < rightDisFromCam) { //Left corner is in front of right
					wallStatus[j-1] = 2;
					wallStatus[j] = 1;
					endStrip = true;
				} else { //Right corner is in front of left
					wallStatus[j-1] = 1;
					wallStatus[j] = 2;
					endStrip = true;
				}
			}
			prevDisToPoint = disToPoint;

			// Create Line Equation
			float m = delZ/delX;
			float b = ptZ - m*ptX;

			// Determine sum of error between each point in between ends
			float error = 0.0;
			for (int k = i+1; k < j; k++) { //Loop through points in between
				float predictZ = m*cWallSlice[(k*2)+0] + b;
				float actualZ = cWallSlice[(k*2)+1];
				float predDiff = predictZ - actualZ;
				error += abs(predDiff);
			}

			// Check for error exceeding allowable amount (point not on line)
			float errorDir = atan2(error,disToPoint/2.0f);
			if (errorDir > PI/4) {
				if (j == numWallSlicePts-1) { //End the strip if at the end of data
					nextPoint = j+1;
					endStrip = true;
					break;
				}
				float leftDisFromCam = pWallSlice[(j*2)-1];
				float rightDisFromCam = pWallSlice[(j*2)+1];
				float camDisDiff = abs(leftDisFromCam - rightDisFromCam);
				if (camDisDiff < 25.0f) { //Points are close (Both true corners)
					wallStatus[j-1] = 2;
					wallStatus[j] = 2;
				} else if (leftDisFromCam < rightDisFromCam) { //Left corner is in front of right
					wallStatus[j-1] = 2;
					wallStatus[j] = 1;
					endStrip = true;
				} else { //Right corner is in front of left
					wallStatus[j-1] = 1;
					wallStatus[j] = 2;
					endStrip = true;
				}
				nextPoint = j;
				break;
			} else { // Add to regression as long as value on line
				nPoints++;
				sumX += ptX;
				sumZ += ptZ;
				sumXsquared += ptX * ptX;
				sumZsquared += ptZ * ptZ;
				sumXZ += ptX * ptZ;
				prevX = ptX;
			}
			if (j == numWallSlicePts-1) { //End the strip if at the end of data
				endStrip = true;
				break;
			}
			if (wallStatus[j] != 0) { //End the strip if reach another corner (break in data)
				endStrip = true;
				break;
			}
			nextPoint = j;
		}

		// Perform Regression for Line from i to j-1
		if (nPoints > 2) {
			float lineM = ( float(nPoints) * sumXZ - sumZ * sumX) /
				( float(nPoints) * sumXsquared - sumX * sumX);
			float lineB = (sumZ - lineM * sumX) / float(nPoints);
			if (endStrip) { //Just ended line strip - Estimate right corner
				if (prevLineM == 999999.0) { //Just started line strip - Estimate left corner
					wallCorners[(numCorners*6)+0] = origX;
					wallCorners[(numCorners*6)+1] = (origX*lineM)+lineB;
					wallCorners[(numCorners*6)+2] = wallStatus[i];
					wallCorners[(numCorners*6)+3] = 0.0f; // Not connected to left
					numCorners++;
				} else { // Determine line intersection with previous
					float lineSlopeDiff = abs(atan(lineM) - atan(prevLineM));
					if (lineSlopeDiff < PI/6) {
						//Predict right part of previous line
						float prevPrevX = cWallSlice[(i*2)-2];
						wallCorners[(numCorners*6)+0] = prevPrevX;
						wallCorners[(numCorners*6)+1] = (prevPrevX*prevLineM)+prevLineB;
						wallCorners[(numCorners*6)+2] = wallStatus[i-1];
						numCorners++;
						//Predict left part of current line
						wallCorners[(numCorners*6)+0] = origX;
						wallCorners[(numCorners*6)+1] = (origX*lineM)+lineB;
						wallCorners[(numCorners*6)+2] = wallStatus[i];
						wallCorners[(numCorners*6)+3] = 0.0f; // Not connected to left
						numCorners++;
					} else {
						float interX = (lineB-prevLineB)/(prevLineM-lineM);
						wallCorners[(numCorners*6)+0] = interX;
						wallCorners[(numCorners*6)+1] = (interX*lineM)+lineB;
						wallCorners[(numCorners*6)+2] = wallStatus[i];
						wallCorners[(numCorners*6)+3] = 1.0f; // Connected to left
						numCorners++;
					}
				}
				//Estimate right corner
				wallCorners[(numCorners*6)+0] = prevX;
				wallCorners[(numCorners*6)+1] = (prevX*lineM)+lineB;
				wallCorners[(numCorners*6)+2] = wallStatus[nextPoint-1];
				wallCorners[(numCorners*6)+3] = 1.0f; // Connected to left
				numCorners++;
				prevLineM = 999999.0;
			} else { //Not done with strip
				if (prevLineM == 999999.0) { //Just started line strip - Estimate left corner
					wallCorners[(numCorners*6)+0] = origX;
					wallCorners[(numCorners*6)+1] = (origX*lineM)+lineB;
					wallCorners[(numCorners*6)+2] = wallStatus[i];
					wallCorners[(numCorners*6)+3] = 0.0f; // Not connected to left
					numCorners++;
				} else { // Determine line intersection with previous
					float lineSlopeDiff = abs(atan(lineM) - atan(prevLineM));
					if (lineSlopeDiff < PI/6) {
						//Predict right part of previous line
						float prevPrevX = cWallSlice[(i*2)-2];
						wallCorners[(numCorners*6)+0] = prevPrevX;
						wallCorners[(numCorners*6)+1] = (prevPrevX*prevLineM)+prevLineB;
						wallCorners[(numCorners*6)+2] = wallStatus[i-1];
						numCorners++;
						//Predict left part of current line
						wallCorners[(numCorners*6)+0] = origX;
						wallCorners[(numCorners*6)+1] = (origX*lineM)+lineB;
						wallCorners[(numCorners*6)+2] = wallStatus[i];
						wallCorners[(numCorners*6)+3] = 0.0f; // Not connected to left
						numCorners++;
					} else {
						float interX = (lineB-prevLineB)/(prevLineM-lineM);
						wallCorners[(numCorners*6)+0] = interX;
						wallCorners[(numCorners*6)+1] = (interX*lineM)+lineB;
						wallCorners[(numCorners*6)+2] = wallStatus[i];
						wallCorners[(numCorners*6)+3] = 1.0f; // Connected to left
						numCorners++;		
					}
				}
				prevLineM = lineM;
				prevLineB = lineB;
			}
		} else {
			prevLineM = 999999.0;
		}

		// Next start on j
		i = nextPoint;
	}
}

void updateGlobalMap() {
	for (int i=0; i < numCorners; i++) {
		float localX = wallCorners[(i*6)+0];
		float localZ = wallCorners[(i*6)+1];
		float localTruthValue = wallCorners[(i*6)+2];
		float localLeftConn = wallCorners[(i*6)+3];
		float localLeftWeight = wallCorners[(i*6)+4];
		float localRightWeight = wallCorners[(i*6)+5];
		if (localTruthValue == 2.0f) {
			bool found = false;
			for (int j=0; j < numGlobalCorners; j++) {
				float globX = globalMapCorners[j].x;
				float globZ = globalMapCorners[j].z;
				float delX = localX - globX;
				float delZ = localZ - globZ;
				float disToGlob = sqrt((delX*delX)+(delZ*delZ));
				if (disToGlob < 20.0f) { //Close enough to match
					float globW = globalMapCorners[numGlobalCorners].leftWeight;
					globalMapCorners[numGlobalCorners].x += localX/globW;
					globalMapCorners[numGlobalCorners].z += localZ/globW;
					globalMapCorners[numGlobalCorners].leftWeight++;
					found = true;
					break;
				}
			}
			if (!found) { //Add
				globalMapCorners[numGlobalCorners].x = localX;
				globalMapCorners[numGlobalCorners].z = localZ;
				globalMapCorners[numGlobalCorners].leftWeight = 1.0f;
				numGlobalCorners++;
			}
		}
	}
}

void setPositionAndOrientation() {
	// Set final top-down values
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
}