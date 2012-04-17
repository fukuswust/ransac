#ifndef H_GLOBALS
#define H_GLOBALS

#include "hr_time.h"
#include "listener.h"
#include "Kinect-win32.h"
#include "topDownMap.h"
#include "types.h"
#include "augModel.h"

#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

//#define RECORD_RAW

// Mouse Location
extern int mouseX;
extern int mouseY;
// Recording file counter
extern int outFileOn;
// View size and offset
extern int viewWidth;
extern int viewHeight;
extern int viewXOffset;
extern int viewYOffset;
extern float xViewFactor;
extern float yViewFactor;
// Sensor Orientation and Position
extern float heightValue, xValue, zValue;
extern float yawValue, rollValue, pitchValue;
extern float accelVector[3];
extern float curUpVector[3];
// Camera Orientation
extern float yawMatrix[9];
extern float pitchRollMatrix[9];
extern float translationMatrix[3];
// Floor Points
extern int floorIJ[MAX_FLOOR_POINTS*2];
extern int numFloorPoints;
// Wall Points
extern int wallIJ[MAX_WALL_POINTS*2];
extern int numWallPoints;
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

// Kinect Objects
extern Kinect::Kinect *K;
extern Listener *L;
extern Kinect::KinectFinder KF;

// Texture for RGB feed
extern GLuint texID;

// Setup booleans for keyboard toggles
extern bool isFullscreen;
extern bool showHud;
extern bool showFloorPoints;
extern bool showWallPoints;

// Models
extern AugModel* model;

//Button Colors
extern float btnColors[8][3];

#endif