#ifndef H_GLOBALS
#define H_GLOBALS

#include "hr_time.h"
#include "listener.h"
#include "Kinect-win32.h"

#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

//#define RECORD_RAW
//#define RECORD_SLICES
#define DRAW_HEIGHT_SLICE

#define DEPTH_SCALE_FACTOR 16
#define MAX_FLOOR_POINTS ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MAX_WALL_POINTS ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MAX_ALLOWED_DIS 700.0f
#define NUM_SLICES (640/DEPTH_SCALE_FACTOR)
#define CLOUD_SIZE ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define CLOUD_AVG_FACTOR 0.5f

#define PI 3.14159265

struct MapCorner {
	float x;
	float z;
	bool truth;
	bool leftConn;
	float leftWeight;
	float rightWeight;
};

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
extern float xAccel, yAccel, zAccel;
extern float rollValue, pitchValue;
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
extern float cWallSlice[NUM_SLICES*2];
extern int numWallSlicePts;
extern int wallStatus[NUM_SLICES];
// Camera Orientation
extern float yawMatrix[9];
extern float pitchRollMatrix[9];
extern float translationMatrix[3];
// Running Average
extern float origZ[CLOUD_SIZE];
extern float stdErrorList[39];
extern float stdErrorListDis[39];
// Corners
extern float wallCorners[40*6]; // x, z, t(2)/f(1), Lconn, Lweight, Rweight
extern int   numCorners;
// Global Map
extern MapCorner globalMapCorners[100];
extern int numGlobalCorners;

extern int maxZi;
extern int maxZj;

extern Kinect::Kinect *K;
extern Listener *L;

extern GLuint texID;


#endif