void runAlgorithm();

void findRotationToUp(float xVect, float yVect, float zVect);

int  sliceRemoveOutsideRange(float pWallSliceNan[], float cWallSlice[], float pWallSlice[], int wallStatus[]);

void sliceDetectCorners(float cWallSlice[], float pWallSlice[], int wallStatus[], int numWallSlicePts);

void updateGlobalMap();

void setPositionAndOrientation();