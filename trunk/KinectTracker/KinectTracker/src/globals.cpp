#include "globals.h"

// Mouse Location
int mouseX = 0;
int mouseY = 0;
// Recording file counter
int outFileOn = 0;
// View size and offset
int viewWidth = 640;
int viewHeight = 480;
int viewXOffset = 0;
int viewYOffset = 0;
float xViewFactor = 0.0f;
float yViewFactor = 0.0f;
// Sensor Orientation and Position
float heightValue, xValue, zValue;
float yawValue, rollValue, pitchValue;
float accelVector[3];
float curUpVector[3];
// Camera Orientation
float yawMatrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
float pitchRollMatrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
float translationMatrix[3] = {0, -heightValue, 0};
// Floor Points
int floorIJ[MAX_FLOOR_POINTS*2];
int numFloorPoints = 0;
// Wall Points
int wallIJ[MAX_WALL_POINTS*2];
int numWallPoints;
// Augmentations
float augCubeX = 0.0f;
float augCubeY = 0.0f;
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

// Kinect Objects
Kinect::Kinect *K;
Listener *L;
Kinect::KinectFinder KF;

// Texture for RGB feed
GLuint texID;

// Set inital values for gui keyboard shortcut states.
bool isFullscreen = false;
bool showHud = true;
bool showFloorPoints = false;
bool showWallPoints = false;

// Models
AugModel* model;

//Button Colors
float btnColors[8][3] = {{0.50f, 0.50f, 0.50f},
						 {1.00f, 1.00f, 1.00f},
						 {1.00f, 0.00f, 0.00f},
						 {0.00f, 1.00f, 0.00f},
						 {0.00f, 0.00f, 1.00f},
						 {1.00f, 1.00f, 0.00f},
						 {0.00f, 1.00f, 1.00f},
						 {1.00f, 0.00f, 1.00f}};