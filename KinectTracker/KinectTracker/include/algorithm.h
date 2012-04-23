#include "globals.h"

void initAlgorithm();

void runAlgorithm();

void findRotationToUp(float xVect, float yVect, float zVect, float M[]);

int  sliceRemoveOutsideRange(float pWallSliceNan[], float cWallSlice[], float pWallSlice[], int wallStatus[]);

void sliceDetectCorners(float cWallSlice[], float pWallSlice[], int wallStatus[], int numWallSlicePts);

void updateGlobalMap();

void setPositionAndOrientation();

void performRotation(SlicePoint set[], float rot);

void solveVector(float M[3][3], float R[3]);

float normalizeVector(float R[3]);

void segmentFloor(float floorPoints[], int &numFloorPoints, int floorHist[], float currentMinHeight, float alignFloor[], float &floorHeight);

int  flattenWall(SlicePoint wallSlicePoints[], int wallHist[][20], SlicePoint tdWall[], int lineID[]);

float estimateYaw(SlicePoint tdWall[], int numTdWallPts, float yawValue);

float dirDiffAngle(float dir1, float dir2);

float dirDiffAngleSign(float dir1, float dir2);

float weighedAngleAvg(float dir1, float dir2, float w);

int  xzMedianFilter(SlicePoint tdWall[], int numTdWallPts);

void determineAxisLines(SlicePoint tdWall[], int numTdWallPts, LineSeg lineSegX[], int &numLineSegX, LineSeg lineSegZ[], int &numLineSegZ, float &estYaw);

void addToMap(LineSeg tdLineSeg[], int numLineSeg, LineSeg lineMap[], int &numLineMap, bool isTypeX);

float compareToMap(LineSeg tdLineSeg[], int numLineSeg, LineSeg lineMap[], int &numLineMap, bool isTypeX);