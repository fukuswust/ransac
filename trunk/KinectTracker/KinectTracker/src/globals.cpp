#include "globals.h"
#include <string>
using namespace std;

// Mouse Location
int mouseX = 0;
int mouseY = 0;

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

// Top Down Map
TopDownMap topDownMap;
SlicePoint drawTdWall[40];
int drawNumTdWallPts = 0;

// Top Down Lines
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
bool mapRecord = false;
bool tdDisplayTracking = true;
bool showHelp = true;

// Models
AugModel* modelHead = NULL;
AugModel* modelTail = NULL;
string** modelPaths;

// Edit Mode
float btnColors[8][3] = {{0.25f, 0.25f, 0.25f}, // Cancel
						 {0.58f, 0.29f, 0.00f}, // Beds
						 {1.00f, 0.00f, 0.00f}, // Storage
						 {0.00f, 1.00f, 0.00f}, // Chairs
						 {0.00f, 0.00f, 1.00f}, // Tables
						 {1.00f, 1.00f, 0.00f}, // Lights
						 {0.00f, 1.00f, 1.00f}, // Electronics
						 {1.00f, 0.00f, 1.00f}}; // Misc
float editModelHeight = 0.0f;
float editModelWidth = 0.0f;
float editModelLength = 0.0f;
bool editPlacing = false;

// Icons
NeHe::imageTGA editIcon[6];