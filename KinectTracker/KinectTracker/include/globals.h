#ifndef H_GLOBALS
#define H_GLOBALS

#include "hr_time.h"
#include "listener.h"
#include "Kinect-win32.h"
#include "topDownMap.h"
#include "types.h"

#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

//#define RECORD_RAW

// Recording file counter
extern int outFileOn;
// Wait for new depth and color frames
extern int mDepthFrameOn;
extern int mPrevDepthFrameOn;
extern int mColorFrameOn;
extern int mPrevColorFrameOn;
// View size and offset
extern int viewWidth;
extern int viewHeight;
extern int viewXOffset;
extern int viewYOffset;
extern float xViewFactor;
extern float yViewFactor;
// Sensor Height
extern float heightValue;
// FPS calculation
extern CStopWatch* fpsStopWatch;
extern float avgFrameTime;
// Pitch and Roll
extern float accelVector[3];
extern float rollValue, pitchValue;
extern float curUpVector[3];
// Height Slices
extern float heightSlices[(640/DEPTH_SCALE_FACTOR)*2];
extern float heightSliceColors[(640/DEPTH_SCALE_FACTOR)];
extern int   heightSliceIJ[(640/DEPTH_SCALE_FACTOR)*2];
// Floor Points
extern float floorPoints[MAX_FLOOR_POINTS*3];
extern int   floorIJ[MAX_FLOOR_POINTS*2];
extern int   numFloorPoints;
// Wall Points
extern float wallPoints[MAX_WALL_POINTS*3];
extern int   wallIJ[MAX_WALL_POINTS*2];
extern int   numWallPoints;
// Top Down Position
extern float yawValue;
extern float xValue;
extern float zValue;
// Camera Orientation
extern float yawMatrix[9];
extern float pitchRollMatrix[9];
extern float translationMatrix[3];
// Running Average
extern float origZ[CLOUD_SIZE];
// Global Map
extern GlobalMapCorner globalMapCorners[100];
extern int numGlobalCorners;
// Global/Local Map Points
extern SlicePoint wallSlicePoints[NUM_SLICES];
extern SlicePoint localMapPoints[NUM_SLICES];
extern int localToGlobal[NUM_SLICES];
extern GlobalMapPoint globalMapPoints[1023];
extern int numGlobalPoints;
extern int closestLocal[NUM_SLICES];
// Augmentations
#define AUG_CUBE_SIZE 100.0f
extern float augCubeX;
extern float augCubeY;
extern float augCubeZ;
extern float augCubeYaw;
// Top Down Map
extern TopDownMap topDownMap;
extern SlicePoint tdWall[40];
extern int numTdWallPts;
extern Line tdLine[8];
extern int numTdLines;
extern float tdPrevDir;
extern float tdPrevX;
extern float tdPrevZ;
extern LineSeg tdLineSegX[8];
extern int numLineSegX;
extern LineSeg tdLineSegZ[8];
extern int numLineSegZ;
extern LineSeg lineMapX[32];
extern int numLineMapX;
extern LineSeg lineMapZ[32];
extern int numLineMapZ;

extern int maxZi;
extern int maxZj;

extern Kinect::Kinect *K;
extern Listener *L;
extern Kinect::KinectFinder KF;
extern bool algHasInit;

extern GLuint texID;

// Setup booleans for keyboard toggles
extern bool isFullscreen;
extern bool showHud;
extern bool showFloorPoints;
extern bool showWallPoints;

#endif