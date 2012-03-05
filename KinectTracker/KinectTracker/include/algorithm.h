#include "globals.h"

void runAlgorithm();

void findRotationToUp(float xVect, float yVect, float zVect);

int  sliceRemoveOutsideRange(float pWallSliceNan[], float cWallSlice[], float pWallSlice[], int wallStatus[]);

void sliceDetectCorners(float cWallSlice[], float pWallSlice[], int wallStatus[], int numWallSlicePts);

void updateGlobalMap();

void setPositionAndOrientation();

void compareToLocalMap();

void  moveOffsets(SlicePoint set1[] , SlicePoint set2[], int closestLocal[]);

float determineError(SlicePoint set1[] , SlicePoint set2[], int closestLocal[]);

void performRotation(SlicePoint set[], float rot);

void performTranslation(SlicePoint set[], float x, float z);

int minimizeTranslationError(SlicePoint set1[] , SlicePoint set2[], int closestLocal[], float &delX, float &delZ);