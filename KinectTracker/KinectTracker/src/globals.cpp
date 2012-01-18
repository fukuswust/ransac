#include "globals.h"

// Recording file counter
int outFileOn = 0;
// Wait for new depth and color frames
int mDepthFrameOn = 0;
int mPrevDepthFrameOn = 0;
int mColorFrameOn = 0;
int mPrevColorFrameOn = 0;
// View size and offset
int viewWidth = 640;
int viewHeight = 480;
int viewXOffset = 0;
int viewYOffset = 0;
float xViewFactor = 0.0f;
float yViewFactor = 0.0f;
// Sensor Height
float heightValue = 0.0f;
// FPS calculation
CStopWatch* fpsStopWatch;
float avgFrameTime = 0.0f;
// Pitch and Roll
float xAccel, yAccel, zAccel;
float rollValue=0.0f, pitchValue=0.0f;
// Height Slices
float heightSlices[(640/DEPTH_SCALE_FACTOR)*2];
float heightSliceColors[(640/DEPTH_SCALE_FACTOR)];
int   heightSliceIJ[(640/DEPTH_SCALE_FACTOR)*2];
// Floor Points
float floorPoints[MAX_FLOOR_POINTS*3];
int   floorIJ[MAX_FLOOR_POINTS*2];
int   numFloorPoints = 0;
// Wall Points
float wallPoints[MAX_WALL_POINTS*3];
int   wallIJ[MAX_WALL_POINTS*2];
int   numWallPoints;
// Wall Slices
double *curWallSlice = NULL;
int   numCurWallSlices = -1;
double *prevWallSlice = NULL;
int   numPrevWallSlices = -1;
// Top Down Position
float yawValue = 999999.0f;
float xValue = 999999.0f;
float zValue = 999999.0f;
// Camera Orientation
float yawMatrix[9];
float pitchRollMatrix[9];
float translationMatrix[3];

int maxZi = 0;
int maxZj = 0;

Kinect::Kinect *K;
Listener *L;

GLuint texID;