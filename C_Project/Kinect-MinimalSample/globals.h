#ifndef H_GLOBALS
#define H_GLOBALS

#include "hr_time.h"

//#define RECORD_RAW
//#define RECORD_SLICES
#define DEPTH_SCALE_FACTOR 16

#define PI 3.14159265

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
// Sensor Height
extern float sensorHeight;
// FPS calculation
extern CStopWatch* fpsStopWatch;
extern float avgFrameTime;
// Pitch and Roll
extern float xAccel, yAccel, zAccel;
extern float rollValue, pitchValue;
// Height Slices
extern float heightSlices[(640/DEPTH_SCALE_FACTOR)*2];
extern float heightSliceColors[(640/DEPTH_SCALE_FACTOR)];

#endif