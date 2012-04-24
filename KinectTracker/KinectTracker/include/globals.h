#ifndef H_GLOBALS
#define H_GLOBALS

#include "hr_time.h"
#include "listener.h"
#include "Kinect-win32.h"
#include "topDownMap.h"
#include "types.h"
#include "augModel.h"
#include "NeheTGATexture.h"

#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <string>
using namespace std;

// Mouse Location
extern int mouseX;
extern int mouseY;

// View size and offset
extern int viewWidth;
extern int viewHeight;
extern int viewXOffset;
extern int viewYOffset;
extern float xViewFactor;
extern float yViewFactor;
extern float maxAllowedDis;

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

// Top Down Map
extern TopDownMap topDownMap;
extern SlicePoint drawTdWall[40];
extern int drawNumTdWallPts;

// Top Down Lines
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
extern Kinect::KinectFinder *KF;

// Texture for RGB feed
extern GLuint texID;

// Setup booleans for keyboard toggles
extern bool isFullscreen;
extern bool showHud;
extern bool showFloorPoints;
extern bool showWallPoints;
extern bool mapRecord;
extern bool tdDisplayTracking;
extern bool showHelp;

// Models
extern AugModel* modelHead;
extern AugModel* modelTail;
extern string** modelPaths;

// Edit Mode
extern float btnColors[8][3];
extern bool editPlacing;
extern float editModelHeight;
extern float editModelWidth;
extern float editModelLength;

// Icons
extern NeHe::imageTGA editIcon[6];

// Warnings
extern bool showWarningFloor;
extern bool showWarningWallX;
extern bool showWarningWallZ;
extern bool showWarningPR;
extern bool showWarningYaw;

extern int depthReceived;
extern bool kinectConnected;

#endif