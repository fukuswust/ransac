#include "globals.h"

void drawColorBackground(int viewWidth, int viewHeight, GLuint texID);
void drawHeightLine(float heightSlices[], int heightSliceIJ[], int numSlices);
void drawFloorPoints(int floorIJ[], int numPoints);
void drawWallPoints(int wallIJ[], int numPoints);
void drawAugmentedPoint(float x, float y, float z);
void drawTopDownViewPoint(float x,float y, float z);
void drawAugmentedCube(float x, float y, float z, float yaw, float s);
void drawAugmentedCorners();