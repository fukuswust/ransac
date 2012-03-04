#include "globals.h"

void runAlgorithm();

void findRotationToUp(float xVect, float yVect, float zVect);

int  sliceRemoveOutsideRange(float pWallSliceNan[], float cWallSlice[], float pWallSlice[], int wallStatus[]);

void sliceDetectCorners(float cWallSlice[], float pWallSlice[], int wallStatus[], int numWallSlicePts);

void updateGlobalMap();

void setPositionAndOrientation();

void compareToLocalMap();

float determineCartesianError(SlicePoint set1[] , SlicePoint set2[], int sep);

void  minimizeCartesianError(SlicePoint set1[] , SlicePoint set2[], int sep, float &delX, float &delZ);

float determinePolarDirError(SlicePoint set1[] , SlicePoint set2[], int sep);

float minimizePolarDirError(SlicePoint set1[] , SlicePoint set2[], int sep);

float determinePolarDisError(SlicePoint set1[] , SlicePoint set2[], int sep);

float minimizePolarDirError2(SlicePoint set1[] , SlicePoint set2[], int sep);

