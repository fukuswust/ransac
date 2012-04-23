#include "algorithm.h"
#include "globals.h"
#include <math.h>
#include <assert.h>

#include <iostream>

using namespace std;

void initAlgorithm() {
	// SET INITIAL ROLL AND PITCH VALUES TO GRAVITY
	curUpVector[0] = 0.0f;
	curUpVector[1] = 1.0f;
	curUpVector[2] = 0.0f;

	// SET INITIAL HEIGHT TO 1.5m
	heightValue = -150.0f;
	yawValue = 0.0f;
	xValue = 0.0f;
	zValue = 0.0f;
}

void runAlgorithm() {
	static bool algHasInit = false;
	if (!algHasInit) {
		initAlgorithm();
		algHasInit = true;
	}

	#pragma region START
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
	int fpIjOffset = 0;
	int fpOffset = 0;
	int wijOffset = 0;
	int wOffset = 0;
	// Get Pitch and Roll Rotation Matrix
	float R11 = pitchRollMatrix[0];
	float R12 = pitchRollMatrix[1];
	float R13 = pitchRollMatrix[2];
	float R21 = pitchRollMatrix[3];
	float R22 = pitchRollMatrix[4];
	float R23 = pitchRollMatrix[5];
	float R31 = pitchRollMatrix[6];
	float R32 = pitchRollMatrix[7];
	float R33 = pitchRollMatrix[8];
	// SET UP MIN HEIGHT PLACEHOLDER
	float currentMinHeight = 999999.0f;
	// SET UP WALL SLICE
	SlicePoint wallSlicePoints[NUM_SLICES];
	for (int i = 0; i < NUM_SLICES; i++) {
		wallSlicePoints[i].dis = -999999.0f;
		wallSlicePoints[i].dir = -999999.0f;
	}
	// SET UP TD WALL
	SlicePoint tdWall[40];
	int numTdWallPts = 0;
	// FLLOR AND WALL POINTS
	int floorHist[25] = {0};
	numFloorPoints = 0;
	float floorPoints[MAX_FLOOR_POINTS*3];
	numWallPoints = 0;
	float wallPoints[MAX_WALL_POINTS*3];
	#pragma endregion Initialize common variables

	#pragma region FIRST PASS
	// Convert depth data to cartesian point cloud data aligned to initial vector
	// Assign color data to each point
	// Detect min height for initial height guess
	// Detect maximum distances for each direction (wall slice)
	for (int j = 0; j < 480; j+=DEPTH_SCALE_FACTOR) {
		for (int i = 0; i < 640; i+=DEPTH_SCALE_FACTOR) {
			//Acquire Raw Depth Value
			float tmpX, tmpX2, tmpY, tmpY2, tmpZ, tmpZ2, fi, fj;
			tmpZ = ((float)(K->mDepthBuffer[(j*640)+i]));
				
			//Check Sensor Data for Error
			if (tmpZ >= 1084.0f) {
				depthPointCloud[offset] = 999999.0f;
				offset += 3; // -> next
				imOffset++;
				ijOffset += 2;
			} else {
				assert(tmpZ >= 0.0f && tmpZ < 1084.0f);
				// Depth to Z
				tmpZ = (-100.0f/((-0.00307f * tmpZ) + 3.33f));
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
				/*if ( (imI >= 0) && (imI < 640-2) && (imJ >= 0) && (imJ < 480-2) ) {
					int imSum = 0;
					int imOffset3 = (((imJ*640)+imI)*3);
					for (int count3 = 0; count3 < 3; count3++) { 
						for (int count33 = 0; count33 < 3; count33++) {
							imSum += ((int)K->mColorBuffer[imOffset3++] + 
								(int)K->mColorBuffer[imOffset3++] + (int)K->mColorBuffer[imOffset3++]);
						}
						imOffset3 += -9 + 640;
					}
					colorPointCloud[imOffset++] = imSum / (9.0f * 3.0f * 255.0f);
				} else {
					colorPointCloud[imOffset++] = 999999.0f;
				}*/

				// Reorient Y-Axis to Gravity
				tmpX2 = (tmpX*R11)+(tmpY*R21)+(tmpZ*R31); 
				tmpY2 = (tmpX*R12)+(tmpY*R22)+(tmpZ*R32);  		
				tmpZ2 = (tmpX*R13)+(tmpY*R23)+(tmpZ*R33);  

				// Find Min Height from Y's
				if (tmpY2 < currentMinHeight) { 
					currentMinHeight = tmpY2;
				}

				// Convert to polar coordinates
				float tmpDir, tmpDis;
				depthPointCloud[offset++] = tmpY2; // height -> dir
				depthPointCloud[offset++] = tmpDir = atan2(tmpZ2,tmpX2); // dir -> dis
				depthPointCloud[offset++] = tmpDis = sqrt((tmpZ2*tmpZ2)+(tmpX2*tmpX2)); // dis -> next

				// Determine max Distances for each Direction
				int dirIndex = (int)floor((tmpDir+(3.0f*PI/4.0))/(PI/2.0f/40.0f));
				if (dirIndex >= 0 && dirIndex < NUM_SLICES) {
					if (tmpDis > wallSlicePoints[dirIndex].dis) {
						assert(dirIndex >= 0 && dirIndex < NUM_SLICES);
						wallSlicePoints[dirIndex].dir = tmpDir;
						wallSlicePoints[dirIndex].dis = tmpDis;
					}
				}

			}
		}
	}
	#pragma endregion Convert depth to up vector oriented polar, determine color, min height, and wall slice

	#pragma region SECOND PASS
	// Determine initial floor points guess
	// Determine initial wall points guess
	for (int i = 0; i < NUM_SLICES; i++) {
		if (wallSlicePoints[i].dis == -999999.0f) {
			wallSlicePoints[i].dis = 999999.0f;
			wallSlicePoints[i].dir = 999999.0f;
		}
	}
	offset = 0;
	ijOffset = 0;
	int wallHist[40][20] = {0};
	for (int i = 0; i < CLOUD_SIZE; i++) {
		float tmpY = depthPointCloud[offset++]; // height -> dir
		if (tmpY == 999999.0f) { 
			offset += 2; // -> next
			ijOffset += 2; // -> next
		} else {
			// Set temporary variables
			float tmpDir = depthPointCloud[offset++]; // dir -> dis
			float tmpDis = depthPointCloud[offset++]; // dis -> next
			float tmpX = tmpDis*cos(tmpDir);
			float tmpZ = tmpDis*sin(tmpDir);

			// Determine Floor Points
			if (fpOffset < MAX_FLOOR_POINTS*3) {
				int floorDiff = floor(tmpY-currentMinHeight);
				if (floorDiff < 25) {
					numFloorPoints++;
					floorPoints[fpOffset++] = tmpX;
					floorPoints[fpOffset++] = tmpY;
					floorPoints[fpOffset++] = tmpZ;
					floorIJ[fpIjOffset++] = ijPointCloud[ijOffset++];
					floorIJ[fpIjOffset++] = ijPointCloud[ijOffset++];
					ijOffset -= 2;
					assert(floorDiff >= 0 && floorDiff < 25);
					floorHist[floorDiff]++;
				}
			}

			// Find Wall Points
			int dirIndex = (int)floor((tmpDir+(3.0f*PI/4.0))/(PI/2.0f/40.0f));
			if (dirIndex >= 0 && dirIndex < NUM_SLICES && tmpDis < MAX_ALLOWED_DIS) {
				if (tmpY - currentMinHeight > 50.0f && tmpY - currentMinHeight < 200.0f && tmpDis < MAX_ALLOWED_DIS) {
					int disDiff = floor(wallSlicePoints[dirIndex].dis - tmpDis);
					if (disDiff < 20) {
						numWallPoints++;
						wallPoints[wOffset++] = tmpX;
						wallPoints[wOffset++] = tmpY;
						wallPoints[wOffset++] = tmpZ;
						wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // i -> j
						wallIJ[wijOffset++] = ijPointCloud[ijOffset++]; // j -> next
						assert(dirIndex >= 0 && dirIndex < NUM_SLICES);
						assert(disDiff >= 0 && disDiff < 20);
						wallHist[dirIndex][disDiff]++;
					} else {
						ijOffset += 2;
					}
				} else {
					ijOffset += 2;
				}
			} else {
				ijOffset += 2;
			}
		}
	}
	#pragma endregion Determine floor and wall points

	float alignFloor[3];
	float floorHeight;
	segmentFloor(floorPoints, numFloorPoints, floorHist, currentMinHeight, alignFloor, floorHeight);

	#pragma region Yaw
	// Estimate new yaw
	int lineID[40];
	numTdWallPts = flattenWall(wallSlicePoints, wallHist, tdWall, lineID);
	float estYaw = estimateYaw(tdWall, numTdWallPts, yawValue);
	if (estYaw == 999999.0f) {
		estYaw = yawValue;
	}

	// Average new yaw
	float degDiff = dirDiffAngle(yawValue, estYaw)*180.0f/PI;
	float yawAvg = ((0.1f/5.0f)*degDiff);
	if (yawAvg > 1.0f) {
		yawAvg = 1.0f;
	}
	static float lastLargeDiffYaw = 1.0f;
	if (lastLargeDiffYaw != 0.0f && degDiff < 4.0f) {
		assert(lastLargeDiffYaw > 0.0f);
		yawAvg /= lastLargeDiffYaw;
	}
	float nyawValue = weighedAngleAvg(yawValue, estYaw, 1.0f - yawAvg);
	float yawDiff = dirDiffAngle(yawValue, nyawValue);
	if (yawDiff < 0.05f) {
		lastLargeDiffYaw += 1.0f;
	} else {
		lastLargeDiffYaw = 1.0f;
	}
	#pragma endregion Determine yaw from top down view and dynamically average
	
	#pragma region X,Z
	// Determine delX and delZ
	performRotation(tdWall, -nyawValue);
	numTdWallPts = xzMedianFilter(tdWall, numTdWallPts);
	if (numTdWallPts >= 5) {
		float estYaw2;
		determineAxisLines(tdWall, numTdWallPts, tdLineSegX, numLineSegX, tdLineSegZ, numLineSegZ, estYaw2);
		if (estYaw2 == 999999.0f) {
			yawValue = nyawValue;
		} else {
			estYaw2 = estYaw - estYaw2;
			// Average new yaw2
			performRotation(tdWall, nyawValue);
			float degDiff2 = dirDiffAngle(yawValue, estYaw2)*180.0f/PI;
			float yawAvg2 = ((0.1f/5.0f)*degDiff2);
			if (yawAvg2 > 1.0f) {
				yawAvg2 = 1.0f;
			}
			static float lastLargeDiffYaw2 = 1.0f;
			if (lastLargeDiffYaw2 != 0.0f && degDiff2 < 4.0f) {
				assert(lastLargeDiffYaw2 > 0.0f);
				yawAvg2 /= lastLargeDiffYaw2;
			}
			float nyawValue2 = weighedAngleAvg(yawValue, estYaw2, 1.0f - yawAvg2);
			float yawDiff2 = dirDiffAngle(yawValue, nyawValue2);
			if (yawDiff2 < 0.05f) {
				lastLargeDiffYaw2 += 1.0f;
			} else {
				lastLargeDiffYaw2 = 1.0f;
			}
			yawValue = nyawValue2;
			performRotation(tdWall, -yawValue);
		}
		drawNumTdWallPts = numTdWallPts;
		for (int i = 0; i < numTdWallPts; i++) {
			drawTdWall[i] = tdWall[i];
		}

		if (mapRecord) { // Recording new map
			addToMap(tdLineSegX, numLineSegX, lineMapX, numLineMapX, true);
			addToMap(tdLineSegZ, numLineSegZ, lineMapZ, numLineMapZ, false);
		} else { // Compare to recorded map
			float delX, delZ;
			delX = compareToMap(tdLineSegZ, numLineSegZ, lineMapZ, numLineMapZ, false);
			if (delX == 999999.0f) {
				showWarningWallZ = true;
			} else {
				// Determine new x with dynamic averaging
 				float avgRateX = abs(delX)*(AVG_STRENGTH/1.5f);
				if (avgRateX > 1.0f) {
					avgRateX = 1.0f;
				}
				static float lastLargeDiffX = 0.0f;
				if (lastLargeDiffX != 0.0f && abs(delX) < 5.0f) {
					avgRateX /= lastLargeDiffX;
					assert(lastLargeDiffX > 0.0f);
				}
				assert(avgRateX >= 0.0f && avgRateX <= 1.0f);
				float nxValue = (xValue*(1.0f-avgRateX)) + ((delX+xValue)*avgRateX);
				float xDiff = abs(nxValue - xValue);
				if (xDiff < 1.0f) {
					lastLargeDiffX += 1.0f;
				} else {
					lastLargeDiffX = 0.0f;
				}
				xValue = nxValue;
				showWarningWallX = false;
			}
			delZ = compareToMap(tdLineSegX, numLineSegX, lineMapX, numLineMapX, true);
			if (delZ == 999999.0f) {
				showWarningWallX = true;
			} else {
				// Determine new z with dynamic averaging
				float avgRateZ = abs(delZ)*(AVG_STRENGTH/1.5f);
				if (avgRateZ > 1.0f) {
					avgRateZ = 1.0f;
				}
				static float lastLargeDiffZ = 0.0f;
				if (lastLargeDiffZ != 0.0f && abs(delZ) < 5.0f) {
					avgRateZ /= lastLargeDiffZ;
					assert(lastLargeDiffZ > 0.0f);
				}
				assert(avgRateZ >= 0.0f && avgRateZ <= 1.0f);
				float nzValue = (zValue*(1.0f-avgRateZ)) + ((delZ+zValue)*avgRateZ);
				float zDiff = abs(nzValue - zValue);
				if (zDiff < 1.0f) {
					lastLargeDiffZ += 1.0f;
				} else {
					lastLargeDiffZ = 0.0f;
				}
				zValue = nzValue;
				showWarningWallZ = false;
			}
		}
	} else {
		showWarningWallZ = true;
		showWarningWallX = true;
		yawValue = nyawValue;;
	}
	#pragma endregion Determine X,Z from top down view and dynamically average
	
	#pragma region PITCH AND ROLL
	// Determine new pitch and roll
	float accelError = normalizeVector(accelVector);
	if (accelError < MAX_ACCEL_ERROR) {
		float avgRate = 0.0f;
		for (int i = 0; i < 3; i++) {
			avgRate += curUpVector[i]*accelVector[i];
		}
		if (abs(avgRate) <= 1.0f) {
			float degVal = acos(avgRate)*180.0f/PI;
			avgRate = degVal*(AVG_STRENGTH/2.5f);
		} else {
			avgRate = 1.0f;
		}
		if (avgRate > 1.0f) {
			avgRate = 1.0f;
		}
		avgRate = 1 - abs(avgRate);
		for (int i = 0; i < 3; i++) {
			//if (numFloorPoints > MIN_FLOOR_POINTS) {
			//	curUpVector[i] = (curUpVector[i]*(1.0f-avgRate)) + (alignFloor[i]*avgRate);
			//} else {
				assert(avgRate >= 0.0f && avgRate <= 1.0f);
				curUpVector[i] = (curUpVector[i]*avgRate) + (accelVector[i]*(1.0f-avgRate));
			//}
		}
		normalizeVector(curUpVector);
		showWarningPR = false;
	} else {
		showWarningPR = true;
	}
	#pragma endregion Dynamically average in new pitch and roll values 

	#pragma region HEIGHT
	// Determine new height
	if (numFloorPoints > MIN_FLOOR_POINTS) {
		//floorHeight -= 16.4f;
		float avgRateY = abs(heightValue-floorHeight)*(AVG_STRENGTH/1.5f);
		if (avgRateY > 1.0f) {
			avgRateY = 1.0f;
		}
		static float lastLargeDiffY = 0.0f;
		if (lastLargeDiffY != 0.0f && abs(heightValue-floorHeight) < 5.0f) {
			assert(lastLargeDiffY > 0.0f);
			avgRateY /= lastLargeDiffY;
		}
		assert(avgRateY >= 0.0f && avgRateY <= 1.0f);
		float nheightValue = (heightValue*(1.0f-avgRateY)) + (floorHeight*avgRateY);
		float heightDiff = abs(nheightValue - heightValue);
		if (heightDiff < 1.0f) {
			lastLargeDiffY += 1.0f;
		} else {
			lastLargeDiffY = 0.0f;
		}
		heightValue = nheightValue;
		showWarningFloor = false;
	} else {
		showWarningFloor = true;
	}
	#pragma endregion Dynamically average in new height value

	// Set augmentation transformation variables
	rollValue = atan2(curUpVector[0], curUpVector[1]);
	pitchValue = atan2(curUpVector[2], curUpVector[1]);
	setPositionAndOrientation();
}

void findRotationToUp(float xVect, float yVect, float zVect, float M[]) {
	//Get unit vector and magnitude of gravity
	float gravMag = sqrt((xVect*xVect)+(yVect*yVect)+(zVect*zVect)); //Quality = diff from 819/512
	if (gravMag == 0.0f) {return;}
	assert(gravMag != 0.0f);
	float uGravX = xVect/gravMag;
	float uGravY = yVect/gravMag;
	float uGravZ = zVect/gravMag;
	//Set temporary variables to reduce calculations
	assert(1-(uGravY*uGravY) >= 0.0f);
	float s = sqrt(1-(uGravY*uGravY));
	float t = 1-uGravY;
	//Perform the cross product of the UP vector and gravity to produce the following rotational axis
	float RotAxisX = -uGravZ;
	float RotAxisZ = uGravX;
	//Determine the unit rotational axis
	float magRotAxis = sqrt((RotAxisX*RotAxisX)+(RotAxisZ*RotAxisZ));
	if (magRotAxis == 0.0f) { // Already perfect
		M[0] = 1.0f;
		M[1] = 0.0f;
		M[2] = 0.0f;
		M[3] = 0.0f;
		M[4] = 1.0f;
		M[5] = 0.0f;
		M[6] = 0.0f;
		M[7] = 0.0f;
		M[8] = 1.0f;
	} else {
		float uX = RotAxisX/magRotAxis;
		float uZ = RotAxisZ/magRotAxis;
		//Solve multiplications in advance that occur more than once
		float xz = uX*uZ;
		float sx = s*uX;
		float sz = s*uZ;
		//Calculate Individual Matrix Elements
		M[0] = uGravY + (t*uX*uX);
		M[1] = -sz;
		M[2] = t*xz;
		M[3] = sz;
		M[4] = uGravY;
		M[5] = -sx;
		M[6] = t*xz;
		M[7] = sx;
		M[8] = uGravY + (t*uZ*uZ);
	}
}

void setPositionAndOrientation() {
	// Camera Orientation
	// Translation
	translationMatrix[0] = xValue; // x translation
	translationMatrix[1] = heightValue; // height translation
	translationMatrix[2] = zValue; // z translation

	// Pitch and Roll Rotation
	// GET  ROTATION MATRIX (Simlified from MATLAB code for UP vector = [0 1 0])
	findRotationToUp(curUpVector[0], curUpVector[1], curUpVector[2], pitchRollMatrix);

	// Yaw Rotation
	yawMatrix[0] = cos(yawValue);
	yawMatrix[1] = 0;
	yawMatrix[2] = -sin(yawValue);
	yawMatrix[3] = 0;
	yawMatrix[4] = 1;
	yawMatrix[5] = 0;
	yawMatrix[6] = sin(yawValue);
	yawMatrix[7] = 0;
	yawMatrix[8] = cos(yawValue);
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

void solveVector(float M[3][3], float R[3]) {
	assert(M[0][0] != 0.0f);
	float r = -M[1][0]/M[0][0];
	//M[1][0] = 0;
	M[1][1] += r*M[0][1];
	M[1][2] += r*M[0][2];
	R[1]    += r*R[0];

	r = -M[2][0]/M[0][0];
	//M[2][0] = 0;
	M[2][1] += r*M[0][1];
	M[2][2] += r*M[0][2];
	R[2]    += r*R[0];

	assert(M[1][1] != 0.0f);
	r = -M[2][1]/M[1][1];
	//M[2][1] = 0;
	M[2][2] += r*M[1][2];
	R[2]    += r*R[1];

	r = -M[0][1]/M[1][1];
	//M[0][1] = 0;
	M[0][2] += r*M[1][2];
	R[0]    += r*R[1];

	assert(M[2][2] != 0.0f);
	r = -M[1][2]/M[2][2];
	//M[1][2] = 0;
	R[1]    += r*R[2];

	r = -M[0][2]/M[2][2];
	//M[0][2] = 0;
	R[0]    += r*R[2];

	R[0] /= M[0][0];
	R[1] /= M[1][1];
	R[2] /= M[2][2];
}

float normalizeVector(float R[3]) {
	float mag = sqrt((R[0]*R[0])+(R[1]*R[1])+(R[2]*R[2]));
	assert(mag != 0.0f);
	R[0] /= mag;
	R[1] /= mag;
	R[2] /= mag;
	return abs(mag-(819.0f/512.0f)); //error
}

void segmentFloor(float floorPoints[], int &numFloorPoints, int floorHist[], float currentMinHeight, float alignFloor[], float &floorHeight) {
	floorHeight = 999999.0f;
	if (numFloorPoints > MIN_FLOOR_POINTS) {
		// Determine Mode of Y's
		int maxHistAm = 0;
		int maxHistI = 0;
		for (int i = 0; i < 25; i++) {
			int curAm = floorHist[i];
			if (curAm > maxHistAm) {
				maxHistAm = curAm;
				maxHistI = i;
			}
		}
		float floorCenterY = currentMinHeight + (float)maxHistI;
		// Eliminate Points Futher than Amount from Mode
		int tmpNumFP = 0;
		float sumX = 0;
		float sumX2 = 0;
		float sumXY = 0;
		float sumXZ = 0;
		float sumY = 0;
		float sumY2 = 0;
		float sumYZ = 0;
		float sumZ = 0;
		for (int i = 0; i < numFloorPoints; i++) {
			float tmpY = floorPoints[(i*3)+1];
			if (abs(tmpY-floorCenterY) < 5.0f) {
				float tmpX = floorPoints[(i*3)+0];
				float tmpZ = floorPoints[(i*3)+2];
				floorPoints[(tmpNumFP*3)+0] = tmpX;
				floorPoints[(tmpNumFP*3)+1] = tmpY;
				floorPoints[(tmpNumFP*3)+2] = tmpZ;
				floorIJ[(tmpNumFP*2)+0]     = floorIJ[(i*2)+0];
				floorIJ[(tmpNumFP*2)+1]     = floorIJ[(i*2)+1];
				tmpNumFP++;
				sumX  += tmpX;
				sumX2 += tmpX*tmpX;
				sumXY += tmpX*tmpZ;
				sumXZ += tmpX*tmpY;
				sumY  += tmpZ;
				sumY2 += tmpZ*tmpZ;
				sumYZ += tmpY*tmpZ;
				sumZ  += tmpY;
			}
		}
		numFloorPoints = tmpNumFP;
		if (numFloorPoints > MIN_FLOOR_POINTS) {
			// Regress Plane
			float M[3][3] = {
				{sumX2, sumXY, sumX},
				{sumXY, sumY2, sumY},
				{sumX,  sumY,  numFloorPoints}
			};
			float R[3] = {sumXZ, sumYZ, sumZ};
			solveVector(M, R);
			float midX = sumX / numFloorPoints;
			float midZ = sumY / numFloorPoints;
			float midY = (R[0]*midX)+(R[1]*midZ)+R[2];
			alignFloor[0] = -R[0]/R[2];
			alignFloor[1] = 1/R[2];
			alignFloor[2] = -R[1]/R[2];
			normalizeVector(alignFloor);
			if (alignFloor[1] < 0) {
				alignFloor[0] *= -1.0f;
				alignFloor[1] *= -1.0f;
				alignFloor[2] *= -1.0f;
			}
			floorHeight = -midY;
		}
	}
}

int  flattenWall(SlicePoint wallSlicePoints[], int wallHist[][20], SlicePoint tdWall[], int lineID[]) {
	int numTdWallPts = 0;
	for (int dirOn = 0; dirOn < 40; dirOn++) {
		// Find Hist Maximum
		int maxV = 0;
		int maxI = -1;
		for (int i = 0; i < 20; i++) {
			if (wallHist[dirOn][i] > maxV) {
				maxV = wallHist[dirOn][i];
				maxI = i;
			}
		}
		// Add point if sufficient data 
		if (maxV > 0) {
			float tmpDir = (dirOn * (PI/2.0f/40.0f)) - (3.0f*PI/4.0);
			float tmpDis = wallSlicePoints[dirOn].dis - maxI;
			tdWall[numTdWallPts].x = tmpDis*cos(tmpDir);
			tdWall[numTdWallPts].z = tmpDis*sin(tmpDir);
			lineID[numTdWallPts] = dirOn/5;
			numTdWallPts++;
		}
	}
	return numTdWallPts;
}

float estimateYaw(SlicePoint tdWall[], int numTdWallPts, float yawValue) {
	// Find all possible slopes and directions and put into histogram
	#define YAW_HIST_SIZE 45
	int yawHist[YAW_HIST_SIZE] = {0};
	for (int i = 0; i < numTdWallPts; i++) {
		for (int j = i-5; j <= i+5; j++) {
			if (i!=j && j >= 0 && j < numTdWallPts) {
				float dir = atan2(tdWall[i].z - tdWall[j].z, tdWall[i].x - tdWall[j].x) + PI;
				float integral;
				dir = modf(dir/(PI/2.0f), &integral);
				assert((int)floor(dir*YAW_HIST_SIZE) >= 0 && (int)floor(dir*YAW_HIST_SIZE) < YAW_HIST_SIZE);
				yawHist[(int)floor(dir*YAW_HIST_SIZE)]++;
			}
		}
	}

	// Find histogram maximum
	int curMaxV = 0;
	int curMaxI = -1;
	for (int i = 0; i < YAW_HIST_SIZE; i++) {
		if (yawHist[i] > curMaxV) {
			curMaxV = yawHist[i];
			curMaxI = i;
		}
	}
	if (curMaxI == -1) {return 999999.0f;}

	// Determine closest quadrant to current direction
	float dirEstQuad = (((float)curMaxI/YAW_HIST_SIZE)*(PI/2.0f));
	if (dirDiffAngle(dirEstQuad+((PI*0.0f)/2.0f), yawValue) <= PI/4.0f) {return dirEstQuad+((PI*0.0f)/2.0f);}
	if (dirDiffAngle(dirEstQuad+((PI*1.0f)/2.0f), yawValue) <= PI/4.0f) {return dirEstQuad+((PI*1.0f)/2.0f);}
	if (dirDiffAngle(dirEstQuad+((PI*2.0f)/2.0f), yawValue) <= PI/4.0f) {return dirEstQuad+((PI*2.0f)/2.0f);}
	return dirEstQuad+((PI*3.0f)/2.0f);
}

float dirDiffAngle(float dir1, float dir2) {
	float tmp = (cos(dir1)*cos(dir2)) + (sin(dir1)*sin(dir2));
	if (tmp >= 1.0f) { 
		return 0.0f;
	} else if (tmp <= -1.0f) {
		return PI;
	} else {
		assert(tmp >= -1.0f && tmp <= 1.0f);
		return acos(tmp);
	}
}

float dirDiffAngleSign(float dir1, float dir2) {
	if (dir1 > dir2) {
		if (dir1 - dir2 < PI) {
			return 1.0f;
		} else {
			return -1.0f;
		}
	} else {
		if (dir2 - dir1 < PI) {
			return -1.0f;
		} else {
			return 1.0f;
		}
	}
}

float weighedAngleAvg(float dir1, float dir2, float w) {
	assert(w >= 0.0f && w <= 1.0f);
	if (abs(dir2-dir1) < PI) {
		return (dir1*w)+(dir2*(1-w));
	} else {
		float avg;
		if (dir1 > dir2) {
			avg = (((dir1-(2.0f*PI))*w)+(dir2*(1-w)));
		} else {
			avg = ((dir1*w)+((dir2-(2.0f*PI))*(1-w)));
		}
		if (avg < 0.0f) {avg += 2.0f*PI;}
		return avg;
	}
}

int xzMedianFilter(SlicePoint tdWall[], int numTdWallPts) {
	for (int i = 2; i < numTdWallPts-2; i++) {
		// Find x median
		float minVal[3];
		minVal[0] = tdWall[i-2].x;
		minVal[1] = 999999.0f;
		minVal[2] = 999999.0f;
		for (int j = i-1; j <= i+2; j++) {
			float val = tdWall[j].x;
			if (val < minVal[0]) {
				minVal[2] = minVal[1];
				minVal[1] = minVal[0];
				minVal[0] = val;
			} else if (val < minVal[1]) {
				minVal[2] = minVal[1];
				minVal[1] = val;
			} else if (val < minVal[2]) {
				minVal[2] = val;
			}
		}
		tdWall[i-2].x = minVal[2];
		
		// Find z median
		minVal[0] = tdWall[i-2].z;
		minVal[1] = 999999.0f;
		minVal[2] = 999999.0f;
		for (int j = i-1; j <= i+2; j++) {
			float val = tdWall[j].z;
			if (val < minVal[0]) {
				minVal[2] = minVal[1];
				minVal[1] = minVal[0];
				minVal[0] = val;
			} else if (val < minVal[1]) {
				minVal[2] = minVal[1];
				minVal[1] = val;
			} else if (val < minVal[2]) {
				minVal[2] = val;
			}
		}
		tdWall[i-2].z = minVal[2];
	}
	if (numTdWallPts-4 <= 0) {
		return 0;
	} else {
		return numTdWallPts-4;
	}
}

void determineAxisLines(SlicePoint tdWall[], int numTdWallPts, LineSeg lineSegX[], int &numLineSegX, LineSeg lineSegZ[], int &numLineSegZ, float &estYaw) {
	// Initialize IDs
	int idX[40], idZ[40];
	for (int i = 0; i < numTdWallPts; i++) {
		idX[i] = i/5;
		idZ[i] = i/5;
	}

	float avgX[8], avgZ[8];
	int   nX[8], nZ[8];
	for (int rep = 0; rep < 5; rep++) {
		// Set n and avg to 0
		for (int i = 0; i < 8; i++) {
			nX[i] = nZ[i] = 0;
			avgX[i] = avgZ[i] = 0.0f; 
		}

		// Increment counters and calculate averages
		for (int i = 0; i < numTdWallPts; i++) {
			int midX = idX[i];
			int midZ = idZ[i];
			nX[midX]++;
			nZ[midZ]++;
			avgX[midX] += tdWall[i].x;
			avgZ[midZ] += tdWall[i].z;
		}
		for (int i = 0; i < 8; i++) {
			if (nX[i] > 0) {
				assert(nX[i] > 0);
				avgX[i] /= nX[i];
			}
			if (nZ[i] > 0) {
				assert(nZ[i] > 0);
				avgZ[i] /= nZ[i];
			}
		}

		// Join Similar Lines
		if (rep >= 2)  {
			for (int id1 = 0; id1 < 8; id1++) {
				for (int id2 = id1+1; id2 < 8; id2++) {
					if (nX[id1] > 0 && nX[id2] > 0) {
						if (abs(avgX[id1] - avgX[id2]) < 20) {
							assert(nX[id1]+nX[id2] > 0);
							avgX[id2] = ((avgX[id1]*nX[id1])+(avgX[id2]*nX[id2]))/(nX[id1]+nX[id2]);
							nX[id2] += nX[id1];
							nX[id1] = 0;
						}
					}
					if (nZ[id1] > 0 && nZ[id2] > 0) {
						if (abs(avgZ[id1] - avgZ[id2]) < 20) {
							assert(nZ[id1]+nZ[id2]);
							avgZ[id2] = ((avgZ[id1]*nZ[id1])+(avgZ[id2]*nZ[id2]))/(nZ[id1]+nZ[id2]);
							nZ[id2] += nZ[id1];
							nZ[id1] = 0;
						}
					}
				}
			}
		}

		// Reassign Points
		for (int i = 0; i < numTdWallPts; i++) {
			float minX = 999999.0f;
			float minZ = 999999.0f;
			int   minXi = 0;
			int   minZi = 0;
			for (int mid = 0; mid < 8; mid++) {
				if (nX[mid] > 0) {
					float tmpDiff = abs(avgX[mid] - tdWall[i].x);
					if (tmpDiff < minX) {
						minX = tmpDiff;
						minXi = mid;
					}
				}
				if (nZ[mid] > 0) {
					float tmpDiff = abs(avgZ[mid] - tdWall[i].z);
					if (tmpDiff < minZ) {
						minZ = tmpDiff;
						minZi = mid;
					}
				}
			}
			idX[i] = minXi;
			idZ[i] = minZi;
		}
	}
	// Set n and avg to 0
	for (int i = 0; i < 8; i++) {
		nX[i] = nZ[i] = 0;
		avgX[i] = avgZ[i] = 0.0f; 

	}
	// Increment counters and calculate averages
	for (int i = 0; i < numTdWallPts; i++) {
		int midX = idX[i];
		int midZ = idZ[i];
		nX[midX]++;
		nZ[midZ]++;
		avgX[midX] += tdWall[i].x;
		avgZ[midZ] += tdWall[i].z;
	}
	for (int i = 0; i < 8; i++) {
		if (nX[i] > 0) {
			assert(nX[i] > 0);
			avgX[i] /= nX[i];
		}
		if (nZ[i] > 0) {
			assert(nZ[i] > 0);
			avgZ[i] /= nZ[i];
		}
	}

	// Initialize Sums for Yaw
	float xSumX[8] = {0.0f};
	float xSumZ[8] = {0.0f};
	float xSumXX[8] = {0.0f};
	float xSumXZ[8] = {0.0f};
	float zSumX[8] = {0.0f};
	float zSumZ[8] = {0.0f};
	float zSumXX[8] = {0.0f};
	float zSumXZ[8] = {0.0f};
	float xM = 0.0f, zM = 0.0f;
	int xTotN = 0, zTotN = 0;
	// Split lines based on spacing
	float startX[8];
	float prevX[8];
	int   nPtsX[8] = {0};
	float startZ[8];
	float prevZ[8];
	int   nPtsZ[8] = {0};
	numLineSegX = 0;
	numLineSegZ = 0;
	for (int i = 0; i < numTdWallPts; i++) {
		int midX = idX[i];
		int midZ = idZ[i];
		float mX = tdWall[i].x;
		float mZ = tdWall[i].z;
		// X line
		if (nPtsX[midZ] > 0) {
			if (abs(mX-prevX[midZ]) > 40.0f) { // X line discontinuity
				// Create previous line if at least 3 points and 50cm long
				if (nPtsX[midZ] > 3 && abs(prevX[midZ]-startX[midZ]) > 50.0f) {
					lineSegX[numLineSegX].isTypeX = true;
					lineSegX[numLineSegX].loc = avgZ[midZ];
					lineSegX[numLineSegX].n = nPtsX[midZ];
					if (startX[midZ] < prevX[midZ]) {
						lineSegX[numLineSegX].start = startX[midZ];
						lineSegX[numLineSegX++].stop = prevX[midZ];
					} else {
						lineSegX[numLineSegX].start = prevX[midZ];
						lineSegX[numLineSegX++].stop = startX[midZ];
					}
					// Calculate Yaw
					float dir = atan2(float(nPtsX[midZ]) * xSumXZ[midZ] - xSumZ[midZ] * xSumX[midZ], 
						float(nPtsX[midZ]) * xSumXX[midZ] - xSumX[midZ] * xSumX[midZ]);
					if (dir > PI/2.0f) {
						dir -= PI;
					} else if (dir < -PI/2.0f) {
						dir += PI;
					}
					dir = 0.0f - dir; 
					float w = xTotN/float(xTotN+nPtsX[midZ]);
					xM = (xM*w) + (dir*(1.0f-w));
					xTotN += nPtsX[midZ];
				}
				// Add new point to next line
				nPtsX[midZ] = 1;
				startX[midZ] = mX;
				xSumX[midZ] = mX;
				xSumZ[midZ] = mZ;
				xSumXX[midZ] = mX * mX;
				xSumXZ[midZ] = mX * mZ;
			} else {
				nPtsX[midZ]++;
				xSumX[midZ] += mX;
				xSumZ[midZ] += mZ;
				xSumXX[midZ] += mX * mX;
				xSumXZ[midZ] += mX * mZ;
			}
		} else {
			// Add new point to next line
			nPtsX[midZ] = 1;
			startX[midZ] = mX;
			xSumX[midZ] = mX;
			xSumZ[midZ] = mZ;
			xSumXX[midZ] = mX * mX;
			xSumXZ[midZ] = mX * mZ;
		}
		prevX[midZ] = mX;

		// Z line
		if (nPtsZ[midX] > 0) {
			if (abs(mZ-prevZ[midX]) > 40.0f) { // Z line discontinuity
				// Create previous line if at least 3 points and 50cm long
				if (nPtsZ[midX] > 3 && abs(prevZ[midX]-startZ[midX]) > 50.0f) {
					lineSegZ[numLineSegZ].isTypeX = false;
					lineSegZ[numLineSegZ].loc = avgX[midX];
					lineSegZ[numLineSegZ].n = nPtsZ[midX];
					if (startZ[midX] < prevZ[midX]) {
						lineSegZ[numLineSegZ].start = startZ[midX];
						lineSegZ[numLineSegZ++].stop = prevZ[midX];
					} else {
						lineSegZ[numLineSegZ].start = prevZ[midX];
						lineSegZ[numLineSegZ++].stop = startZ[midX];
					}
					// Calculate Yaw
					float dir = atan2(float(nPtsZ[midX]) * zSumXZ[midX] - zSumZ[midX] * zSumX[midX], 
						float(nPtsZ[midX]) * zSumXX[midX] - zSumX[midX] * zSumX[midX]);
					if (dir > PI/2.0f) {
						dir -= PI;
					} else if (dir < -PI/2.0f) {
						dir += PI;
					}
					dir = 0.0f - dir; 
					float w = zTotN/float(zTotN+nPtsZ[midX]);
					zM = (zM*w) + (dir*(1.0f-w));
					zTotN += nPtsZ[midX];
				}
				// Add new point to next line
				nPtsZ[midX] = 1;
				startZ[midX] = mZ;
				zSumX[midX] = mZ;
				zSumZ[midX] = mX;
				zSumXX[midX] = mZ * mZ;
				zSumXZ[midX] = mZ * mX;
			} else {
				nPtsZ[midX]++;
				zSumX[midX] += mZ;
				zSumZ[midX] += mX;
				zSumXX[midX] += mZ * mZ;
				zSumXZ[midX] += mZ * mX;
			}
		} else {
			// Add new point to next line
			nPtsZ[midX] = 1;
			startZ[midX] = mZ;
			zSumX[midX] = mZ;
			zSumZ[midX] = mX;
			zSumXX[midX] = mZ * mZ;
			zSumXZ[midX] = mZ * mX;
		}
		prevZ[midX] = mZ;
	}

	// Finish all lines
	for (int mid = 0; mid < 8; mid++) {
		int midX = mid;
		int midZ = mid;

		// X line
		// Create previous line if at least 3 points and 50cm long
		if (nPtsX[midZ] > 3 && abs(prevX[midZ]-startX[midZ]) > 50.0f) {
			lineSegX[numLineSegX].isTypeX = true;
			lineSegX[numLineSegX].loc = avgZ[midZ];
			lineSegX[numLineSegX].n = nPtsX[midZ];
			if (startX[midZ] < prevX[midZ]) {
				lineSegX[numLineSegX].start = startX[midZ];
				lineSegX[numLineSegX++].stop = prevX[midZ];
			} else {
				lineSegX[numLineSegX].start = prevX[midZ];
				lineSegX[numLineSegX++].stop = startX[midZ];
			}
			// Calculate Yaw
			float dir = atan2(float(nPtsX[midZ]) * xSumXZ[midZ] - xSumZ[midZ] * xSumX[midZ], 
				float(nPtsX[midZ]) * xSumXX[midZ] - xSumX[midZ] * xSumX[midZ]);
			if (dir > PI/2.0f) {
				dir -= PI;
			} else if (dir < -PI/2.0f) {
				dir += PI;
			}
			dir = 0.0f - dir; 
			float w = xTotN/float(xTotN+nPtsX[midZ]);
			xM = (xM*w) + (dir*(1.0f-w));
			xTotN += nPtsX[midZ];
		}

		// Z line
		// Create previous line if at least 3 points and 50cm long
		if (nPtsZ[midX] > 3 && abs(prevZ[midX]-startZ[midX]) > 50.0f) {
			lineSegZ[numLineSegZ].isTypeX = false;
			lineSegZ[numLineSegZ].loc = avgX[midX];
			lineSegZ[numLineSegZ].n = nPtsZ[midX];
			if (startZ[midX] < prevZ[midX]) {
				lineSegZ[numLineSegZ].start = startZ[midX];
				lineSegZ[numLineSegZ++].stop = prevZ[midX];
			} else {
				lineSegZ[numLineSegZ].start = prevZ[midX];
				lineSegZ[numLineSegZ++].stop = startZ[midX];
			}
			// Calculate Yaw
			float dir = atan2(float(nPtsZ[midX]) * zSumXZ[midX] - zSumZ[midX] * zSumX[midX], 
				float(nPtsZ[midX]) * zSumXX[midX] - zSumX[midX] * zSumX[midX]);
			if (dir > PI/2.0f) {
				dir -= PI;
			} else if (dir < -PI/2.0f) {
				dir += PI;
			}
			dir = 0.0f - dir;
			float w = zTotN/float(zTotN+nPtsZ[midX]);
			zM = (zM*w) + (dir*(1.0f-w));
			zTotN += nPtsZ[midX];
		}
	}

	// Calculate Final Yaw Estimate
	if (xTotN + zTotN <= 0) {
		estYaw = 999999.0f;
	} else {
		float w = float(xTotN)/float((xTotN + zTotN));
		estYaw = (xM*w)+(-zM*(1.0f-w));
	}
}

void addToMap(LineSeg tdLineSeg[], int numLineSeg, LineSeg lineMap[], int &numLineMap, bool isTypeX) {
	for (int i = 0; i < numLineSeg; i++) { // Loop through all new line segments
		float mLoc;
		if (isTypeX) {
			mLoc = tdLineSeg[i].loc + zValue;
		} else {
			mLoc = tdLineSeg[i].loc + xValue;
		}
		float mStart = tdLineSeg[i].start;
		float mStop = tdLineSeg[i].stop;
		float minDis = 999999.0f;
		int mN = tdLineSeg[i].n;
		int minJ = -1;
		for (int j = 0; j < numLineMap; j++) { // Compare to all current map segments
			float disDiff = abs(mLoc - lineMap[j].loc);
			if (disDiff < minDis) {
				minDis = disDiff;
				minJ = j;
			}
		}
		if (minDis < 200.0f) { // Can be associated with map line
			if (mStart < lineMap[minJ].start) {
				lineMap[minJ].start = mStart;
			}
			if (mStop > lineMap[minJ].stop) {
				lineMap[minJ].stop = mStop;
			}
			assert(lineMap[minJ].n + mN > 0);
			lineMap[minJ].loc = ((lineMap[minJ].loc * lineMap[minJ].n) + (mLoc * mN)) / (lineMap[minJ].n + mN);
			lineMap[minJ].n += mN;
		} else { // New Line
			lineMap[numLineMap].isTypeX = isTypeX;
			lineMap[numLineMap].loc = mLoc;
			lineMap[numLineMap].n = mN;
			lineMap[numLineMap].start = mStart;
			lineMap[numLineMap].stop = mStop;
			numLineMap++;
		}
	}
}

float compareToMap(LineSeg tdLineSeg[], int numLineSeg, LineSeg lineMap[], int &numLineMap, bool isTypeX) {
	float diffTotal = 0.0f;
	int nMax = -1;
	for (int i = 0; i < numLineSeg; i++) { // Loop through all new line segments
		float mLoc;
		if (isTypeX) {
			mLoc = tdLineSeg[i].loc + zValue;
		} else {
			mLoc = tdLineSeg[i].loc + xValue;
		}
		int mN = tdLineSeg[i].n;
		float minDis = 999999.0f;
		int minJ = -1;
		for (int j = 0; j < numLineMap; j++) { // Compare to all current map segments
			float disDiff = abs(mLoc - lineMap[j].loc);
			if (disDiff < minDis) {
				minDis = disDiff;
				minJ = j;
			}
		}
		if (minDis < 200.0f) { // Can be associated with map line
			if (mN > nMax) {
				nMax = mN;
				diffTotal = lineMap[minJ].loc - mLoc;
			}
		}
	}
	if (nMax > 0) {
		return diffTotal;
	} else {
		return 999999.0f;
	}
}