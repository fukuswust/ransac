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
// Sensor Height
float sensorHeight = 0.0f;
// FPS calculation
CStopWatch* fpsStopWatch;
float avgFrameTime = 0.0f;
// Pitch and Roll
float xAccel, yAccel, zAccel;
float rollValue=0.0f, pitchValue=0.0f;
// Height Slices
float heightSlices[(640/DEPTH_SCALE_FACTOR)*2];
float heightSliceColors[(640/DEPTH_SCALE_FACTOR)];

Kinect::Kinect *K;
Listener *L;

GLuint texID;