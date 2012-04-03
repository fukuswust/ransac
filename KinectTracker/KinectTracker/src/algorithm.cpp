#include "algorithm.h"
#include "globals.h"
#include "record.h"
#include <math.h>

#include <iostream>

using namespace std;

void initAlgorithm() {
	// SET INITIAL ROLL AND PITCH VALUES TO GRAVITY
	curUpVector[0] = accelVector[0];
	curUpVector[1] = accelVector[1];
	curUpVector[2] = accelVector[2];
	normalizeVector(curUpVector);
	findRotationToUp(curUpVector[0], curUpVector[1], curUpVector[2], pitchRollMatrix);

	// SET INITIAL HEIGHT TO 1.5m
	heightValue = -150.0f;
}

void runAlgorithm() {
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
	for (int i = 0; i < NUM_SLICES; i++) {
		wallSlicePoints[i].dis = -999999.0f;
		wallSlicePoints[i].dir = -999999.0f;
	}
	int floorHist[25] = {0};
	numFloorPoints = 0;
	numWallPoints = 0;
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

	int lineID[40];
	numTdWallPts = flattenWall(wallSlicePoints, wallHist, tdWall, lineID);
	numTdLines = detectTdLines(tdWall, numTdWallPts, lineID, tdLine);

	float avgRateYaw = 0.75f;
	if (numTdLines > 1) {
		//if (tdPrevDir == 999999.0f) {
		float tdDir0 = atan(-tdLine[0].m);
		float tdDir1 = atan(-tdLine[1].m);
		float xDir, zDir;
		if (tdDir0 <= 0) {
			xDir = tdDir0;
			zDir = tdDir1;
		} else {
			xDir = tdDir1;
			zDir = tdDir0;
		}
		float dirDiff = abs(tdDir0 - tdDir1)*180.f/PI;
		if (dirDiff - 10.0f < 90.0f && dirDiff + 10.0f > 90.0f) {
			yawValue = (yawValue*avgRateYaw)+(xDir*(1.0f-avgRateYaw));
		}
	}

	#pragma region AVERAGING
	// Determine new pitch and roll
	float avgRate = 0.0f;
	for (int i = 0; i < 3; i++) {
		avgRate += curUpVector[i]*accelVector[i];
	}
	float degVal = acos(avgRate)*180.0f/PI;
	avgRate = degVal*(AVG_STRENGTH/2.5f);
	if (avgRate > 1.0f) {
		avgRate = 1.0f;
	}
	avgRate = 1 - abs(avgRate);
	for (int i = 0; i < 3; i++) {
		//if (numFloorPoints > MIN_FLOOR_POINTS) {
		//	curUpVector[i] = (curUpVector[i]*(1.0f-avgRate)) + (alignFloor[i]*avgRate);
		//} else {
			curUpVector[i] = (curUpVector[i]*avgRate) + (accelVector[i]*(1.0f-avgRate));
		//}
	}
	normalizeVector(curUpVector);

	// Determine new height
	if (numFloorPoints > MIN_FLOOR_POINTS) {
		avgRate = abs(heightValue-floorHeight)*(AVG_STRENGTH/1.5f);
		if (avgRate > 1.0f) {
			avgRate = 1.0f;
		}
		heightValue = (heightValue*(1.0f-avgRate)) + (floorHeight*avgRate);
	}
	#pragma endregion Dynamically average in new position and orientation values 

	// Set augmentation transformation variables
	rollValue = atan2(curUpVector[0], curUpVector[1]);
	pitchValue = atan2(curUpVector[2], curUpVector[1]);
	setPositionAndOrientation();
}

void findRotationToUp(float xVect, float yVect, float zVect, float M[]) {
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

void solveVector(float M[3][3], float R[3]) {
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

	r = -M[2][1]/M[1][1];
	//M[2][1] = 0;
	M[2][2] += r*M[1][2];
	R[2]    += r*R[1];

	r = -M[0][1]/M[1][1];
	//M[0][1] = 0;
	M[0][2] += r*M[1][2];
	R[0]    += r*R[1];

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

void normalizeVector(float R[3]) {
	float mag = sqrt((R[0]*R[0])+(R[1]*R[1])+(R[2]*R[2]));
	R[0] /= mag;
	R[1] /= mag;
	R[2] /= mag;
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

int  detectTdLines(SlicePoint tdWall[], int numTdWallPts, int lineID[40], Line tdLine[]) {
	int n[8] = {0};
	float sX[8] = {0.0f};
	float sZ[8] = {0.0f};
	float sXX[8] = {0.0f};
	float sZZ[8] = {0.0f};
	float sXZ[8] = {0.0f};
	float m[8] = {0.0f};
	float b[8] = {0.0f};
	float err[8] = {0.0f};

	for (int rep = 0; rep < 5; rep++) {
		// Initialize counters to 0
		for (int i = 0; i < 8; i++) {
			n[i] = 0;
			sX[i] = 0.0f;
			sZ[i] = 0.0f;
			sXX[i] = 0.0f;
			sZZ[i] = 0.0f;
			sXZ[i] = 0.0f;
		}

		// Get sums for regession
		for (int i = 0; i < numTdWallPts; i++) {
			int lID = lineID[i];
			float x = tdWall[i].x;
			float z = tdWall[i].z;
			n[lID]++;
			sX[lID] += x;
			sZ[lID] += z;
			sXX[lID] += x*x;
			sZZ[lID] += z*z;
			sXZ[lID] += x*z;
		}

		// Solve regressions
		for (int secOn = 0; secOn < 8; secOn++) {
			if (n[secOn] > 1) {
				m[secOn] = ( (float)n[secOn] * sXZ[secOn] - sZ[secOn] * sX[secOn] ) / 
					( (float)n[secOn] * sXX[secOn] - sX[secOn] * sX[secOn] );
				b[secOn] = ( sZ[secOn] - m[secOn] * sX[secOn] ) / (float)n[secOn];   
				float ssx = m[secOn] * ( sXZ[secOn] - sX[secOn] * sZ[secOn] / (float)n[secOn] );
				float ssz2 = sZZ[secOn] - sZ[secOn] * sZ[secOn] / (float)n[secOn];
				float ssz = ssz2 - ssx;
				if (n[secOn] == 2) {
					err[secOn] = 0.0f;
				} else {
					err[secOn] = sqrt(ssz / ((float)n[secOn] - 2));
				}
			} else if (n[secOn] == 1) { 
				m[secOn] = sZ[secOn]/sX[secOn];
				b[secOn] = 0.0f;
				err[secOn] = 0.0f;
			} else {
				m[secOn] = 999999.0f;
				b[secOn] = 999999.0f;
				err[secOn] = 999999.0f;
			}
		}

		// Join Lines
		if (rep > 2) {
			for (int l1 = 0; l1 < 7; l1++) {
				for (int l2 = l1+1; l2 < 7; l2++) {
					if (n[l1] > 1 && n[l2] > 0) {
						// Get Sums
						int   jn   = n[l1] + n[l2];
						float jsX  = sX[l1] + sX[l2];
						float jsZ  = sZ[l1] + sZ[l2];
						float jsXX = sXX[l1] + sXX[l2];
						float jsZZ = sZZ[l1] + sZZ[l2];
						float jsXZ = sXZ[l1] + sXZ[l2];
						// Perform Regression
						float jm = ((float)jn * jsXZ - jsZ * jsX ) / ( (float)jn * jsXX - jsX * jsX );
						float jb = ( jsZ - jm * jsX ) / (float)jn;   
						float jssx = jm * ( jsXZ - jsX * jsZ / (float)jn );
						float jssz2 = jsZZ - jsZ * jsZ / (float)jn;
						float jssz = jssz2 - jssx;
						float jerr = sqrt(jssz / ((float)jn - 2));
						// Check for joining lines
						if (n[l2] > 1) {
							if (jerr*(float)jn < err[l1]*(float)n[l1] + err[l2]*(float)n[l2] + (float)n[l2]*10.0f) {
								n[l1] = 0;
								n[l2] = jn;
								sX[l2] = jsX;
								sZ[l2] = jsZ;
								sXX[l2] = jsXX;
								sZZ[l2] = jsZZ;
								sXZ[l2] = jsXZ;
								m[l2] = jm;
								b[l2] = jb;
								err[l2] = jerr;
							}
						} else {
							if (jerr < err[l1] + 0.5) {
								n[l1] = 0;
								n[l2] = jn;
								sX[l2] = jsX;
								sZ[l2] = jsZ;
								sXX[l2] = jsXX;
								sZZ[l2] = jsZZ;
								sXZ[l2] = jsXZ;
								m[l2] = jm;
								b[l2] = jb;
								err[l2] = jerr;
							}
						}
					}
				}
			}
		}

		// Reassign points to nearest line
		for (int i = 0; i < numTdWallPts; i++) {
			float minV = 999999.0f;
			int minI = 0;
			for (int secOn = 0; secOn < 8; secOn++) {
				if (n[secOn] > 0) {
					float tmpDis = abs(m[secOn]*tdWall[i].x - tdWall[i].z + b[secOn])/ sqrt((m[secOn]*m[secOn]) + 1);
					if (tmpDis < minV) {
						minV = tmpDis;
						minI = secOn;
					}
				}
			}
			lineID[i] = minI;
		}

	}

	// Determine Final Lines
	numTdLines = 0;
	for (int itr = 0; itr < 8; itr++) {
		int maxV = 0;
		int maxI = -1;
		for (int secOn = 0; secOn < 8; secOn++) {
			if (n[secOn] > maxV) {
				maxV = n[secOn];
				maxI = secOn;
			}
		}
		if (maxI != -1 && maxV > 3) {
			tdLine[numTdLines].m = m[maxI];
			tdLine[numTdLines].b = b[maxI];
			tdLine[numTdLines].n = maxV;
			n[maxI] = 0;
			numTdLines++;
		} else {
			break;
		}
	}
	return numTdLines;
}