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
float heightValue = -150.0f;
// FPS calculation
CStopWatch* fpsStopWatch;
float avgFrameTime = 0.0f;
// Pitch and Roll
float accelVector[3];
float rollValue=0.0f, pitchValue=0.0f;
float curUpVector[3] = {0.0f, 1.0f, 0.0f};
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
// Top Down Position
float yawValue = 0.0f;
float xValue = 0.0f;
float zValue = 0.0f;
// Camera Orientation
float yawMatrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
float pitchRollMatrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
float translationMatrix[3] = {0, -heightValue, 0};
int maxZi = 0;
int maxZj = 0;
// Running Average
float origZ[CLOUD_SIZE] = {0.0f};
// Global Map
GlobalMapCorner globalMapCorners[100];
int numGlobalCorners = 0;
// Global/Local Map Points
SlicePoint wallSlicePoints[NUM_SLICES];
SlicePoint localMapPoints[NUM_SLICES];
int localToGlobal[NUM_SLICES];
GlobalMapPoint globalMapPoints[1023];
int numGlobalPoints = 0;
int closestLocal[NUM_SLICES];
// Augmentations
//local
//global
float augCubeX = 0.0f;
float augCubeY = 50.0f;
float augCubeZ = 0.0f;
float augCubeYaw = 0.0f;
// Top Down Map
TopDownMap topDownMap;
SlicePoint tdWall[40];
int numTdWallPts = 0;
Line tdLine[8];
int numTdLines = 0;
float tdPrevDir = 999999.0f;
float tdPrevX = 999999.0f;
float tdPrevZ = 999999.0f;
LineSeg tdLineSegX[8];
int numLineSegX = 0;
LineSeg tdLineSegZ[8];
int numLineSegZ = 0;
LineSeg lineMapX[32];
int numLineMapX = 0;
LineSeg lineMapZ[32];
int numLineMapZ = 0;

Kinect::Kinect *K;
Listener *L;
Kinect::KinectFinder KF;
bool algHasInit = false;

GLuint texID;

// Set inital values for gui keyboard shortcut states.
bool isFullscreen = false;
bool showHud = true;
bool showFloorPoints = false;
bool showWallPoints = false;

// Models
AugModel* model;