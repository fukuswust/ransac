#include "globals.h"

void initAlgorithm();

void runAlgorithm();

void findRotationToUp(float xVect, float yVect, float zVect, float M[]);

int  sliceRemoveOutsideRange(float pWallSliceNan[], float cWallSlice[], float pWallSlice[], int wallStatus[]);

void sliceDetectCorners(float cWallSlice[], float pWallSlice[], int wallStatus[], int numWallSlicePts);

void updateGlobalMap();

void setPositionAndOrientation();

void performRotation(SlicePoint set[], float rot);

void performTranslation(SlicePoint set[], float x, float z);

void solveVector(float M[3][3], float R[3]);

void normalizeVector(float R[3]);

void segmentFloor(float floorPoints[], int &numFloorPoints, int floorHist[], float currentMinHeight, float alignFloor[], float &floorHeight);