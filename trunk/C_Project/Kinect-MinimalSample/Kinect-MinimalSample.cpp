#include "globals.h"

#include <math.h>
#include <stdio.h>
#include <conio.h> // for _kbhit and _getch 
#include <stdlib.h> //Needed for "exit" function
#include "../Kinect-win32.h"
#include <fstream>
#include "record.h"
#include "gui.h"

// The "Kinect" Project has been added to the project dependencies of this project. 

//Include OpenGL header files, so that we can use OpenGL
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// the listener callback object. Implement these methods to do your own processing
class Listener: public Kinect::KinectListener
{
public:

		virtual void KinectDisconnected(Kinect::Kinect *K) 
		{
			printf("Kinect disconnected!\n");
		};
		
		// Depth frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseDepthBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
		virtual void DepthReceived(Kinect::Kinect *K) 
		{
			mDepthFrameOn++;
			// K->mDepthBuffer is now valid and usable!
			// see Kinect-Demo.cpp for a more complete example on what to do with this buffer
		};
		
		// Color frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseColorBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
		virtual void ColorReceived(Kinect::Kinect *K) 
		{
			mColorFrameOn++;
			// K->mColorBuffer is now valid and usable!
			// see Kinect-Demo.cpp for a more complete example on what to do with this buffer
		};
		
		// not functional yet:
		virtual void AudioReceived(Kinect::Kinect *K) {};
};

Kinect::Kinect *K;
Listener *L;

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
					int x, int y) {    //The current mouse coordinates
	switch (key) {

		case 27: { //Escape key
			// remove and delete the listener instance
			K->RemoveListener(L);
			delete L;
	
			//turn the led off
			K->SetLedMode(Kinect::Led_Off);
	
			// when the KinectFinder instance is destroyed, it will tear down and free all kinects.
			exit(0); //Exit the program
		}
	}
}

GLuint texID;

//Initializes 3D rendering
void initRendering() {
	//Initialize Texture Space for RGB Camera
	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 640, 480,
							  0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
}

//Called when the window is resized
void handleResize(int w, int h) {
	float ratio = ((float)w/(float)h);
	if (ratio < 640.0f/480.0f) { //Width is the smaller of the two
		viewWidth = w;
		viewHeight = (w*480)/640;
		viewXOffset = 0;
		viewYOffset = (h-viewHeight)/2;
	} else if (ratio > 640.0f/480.0f) { //Height is the smaller of the two
		viewWidth = (h*640)/480;
		viewHeight = h;
		viewXOffset = (w-viewWidth)/2;
		viewYOffset = 0;
	} else { //Perfect Ratio
		viewWidth = w;
		viewHeight = h;
		viewXOffset = 0;
		viewYOffset = 0;
	}

	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(viewXOffset, viewYOffset, viewWidth, viewHeight);

	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective

	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0,                  //The camera angle
				   (double)viewWidth / (double)viewHeight, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   200.0);                //The far z clipping coordinate
}

//Draws the 3D scene

void drawScene() {
	char printBuff[256];
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw RGB Camera in background (in 2D)
	drawColorBackground(viewWidth, viewHeight, texID);

	//Draw 3D Scene
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	// Draw Crosshair (in 2D)
	drawCrosshair(viewWidth, viewHeight);

	//Draw Frame Count (in 2D)
#define HUD_FPS_X 5
#define HUD_FPS_Y 7
	fpsStopWatch->stopTimer();
	avgFrameTime = (0.1*(float)(fpsStopWatch->getElapsedTime()))+(0.9f*avgFrameTime);
	sprintf(printBuff, "FPS: %u", (unsigned int)(1.0f/avgFrameTime));
	orthoPrint(HUD_FPS_X, viewHeight - HUD_FPS_Y, printBuff);
	fpsStopWatch->startTimer(); 

	// Draw Height Measurement Bar
#define HUD_HEIGHT_BAR_X 5
#define HUD_HEIGHT_BAR_Y 5
	drawHeightHud(HUD_HEIGHT_BAR_X, HUD_HEIGHT_BAR_Y, sensorHeight);

	// Enable antialiasing.  Do we want to do this?
	glEnable(GL_LINE_SMOOTH);

	// Draw Roll
#define HUD_ROLL_RADIUS 30
	float xRollLbl = (viewWidth/3);
	float yRollLbl = 35;
	drawRollHud(xRollLbl, yRollLbl, HUD_ROLL_RADIUS, rollValue);
	
	// Draw Pitch
#define HUD_PITCH_RADIUS 30
	float xPitchLbl = (2*viewWidth/3);
	float yPitchLbl = 35;
	drawPitchHud(xPitchLbl, yPitchLbl, HUD_PITCH_RADIUS, pitchValue);

	// Draw Local Top Down Map (in 2D, upper right)
#define HUD_MAP_CIRCLE_SIZE 80
#define HUD_MAP_X (5 + HUD_MAP_CIRCLE_SIZE)
#define HUD_MAP_Y (5 + HUD_MAP_CIRCLE_SIZE)
	drawTopDownMap(viewWidth - HUD_MAP_X, HUD_MAP_Y, HUD_MAP_CIRCLE_SIZE, 
		heightSlices, heightSliceColors, 640/DEPTH_SCALE_FACTOR);

	//Return to Default
	glColor3f(1.0f, 1.0f, 1.0f);
	orthogonalEnd();

	//Send the scene to the screen
	glutSwapBuffers();
}

void runAlgorithm() {
	float xAccelAvg = xAccel;
	float yAccelAvg = yAccel;
	float zAccelAvg = zAccel;

	rollValue = atan2(xAccelAvg, yAccelAvg);
	pitchValue = atan2(zAccelAvg, yAccelAvg);
	
	// GET GRAVITY ROTATION MATRIX (Simlified from MATLAB code for UP vector = [0 1 0])
	//Get unit vector and magnitude of gravity
	float gravMag = sqrt((xAccelAvg*xAccelAvg)+(yAccelAvg*yAccelAvg)+(zAccelAvg*zAccelAvg)); //Quality = diff from 819/512
	float uGravX = xAccelAvg/gravMag;
	float uGravY = yAccelAvg/gravMag;
	float uGravZ = zAccelAvg/gravMag;
	//Set temporary variables to reduce calculations
	float s = sqrt(1-(uGravY*uGravY));
	float t = 1-uGravY;
	//Perform the cross product of the UP vector and gravity to produce the following rotational axis
	float RotAxisX = -uGravZ;
	float RotAxisZ = uGravX;
	//Determine the unit rotational axis
	float magRotAxis = sqrt((RotAxisX*RotAxisX)+(RotAxisZ*RotAxisZ));
	float uX = RotAxisX/magRotAxis;
	float uZ = RotAxisZ/magRotAxis;
	//Solve multiplications in advance that occur more than once
	float xz = uX*uZ;
	float sx = s*uX;
	float sz = s*uZ;
	//Calculate Individual Matrix Elements
	float R11 = uGravY + (t*uX*uX);
	float R12 = -sz;
	float R13 = t*xz;
	float R21 = sz;
	float R22 = uGravY;
	float R23 = -sx;
	float R31 = t*xz;
	float R32 = sx;
	float R33 = uGravY + (t*uZ*uZ);

	// DECLARE POINT CLOUD DATA as Local Array of Floats (40x30x3 on Stack)
	// DECLARE COLOR DATA as Local Array of Floats (40*30 on Stack)
	float depthPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))*3]; // Stored in the order of [z,y,x]
	float colorPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))];
	int offset = 0;
	int imOffset = 0;
	float currentMinHeight = 999999.0f;
	float currentMinDir = 999999.0f;
	float currentMaxDir = -999999.0f;

	// Convert depth data to polar point cloud data aligned to initial vector
	// Detect mins and maxes
	// Assign color data to each point
	for (int j = 0; j < 480; j+=DEPTH_SCALE_FACTOR) {
		for (int i = 0; i < 640; i+=DEPTH_SCALE_FACTOR) {
			//Acquire Raw Depth Value
			float z;
			z = (float)(K->mDepthBuffer[(j*640)+i]);
				
			//Check Sensor Data for Error
			if (z == 2047.0f) {
				depthPointCloud[offset] = 999999.0f;
				offset += 3; // -> next
				imOffset++;
			} else {
				// Depth to Z
				depthPointCloud[offset++] = z = -100.0f/((-0.00307f * z) + 3.33f); //z -> y
					
				// Z to Point Cloud
				depthPointCloud[offset++] = (float)(j - 240) * (z - 10.0f) * 0.0021f ; //y -> x
				depthPointCloud[offset] = (float)(i - 320) * (z - 10.0f) * -0.0021f ;  //x
					 
				// Set up initial temporary variables
				float tmpX = depthPointCloud[offset--]; // x -> y
				float tmpY = depthPointCloud[offset--]; // y -> z
				float tmpZ = depthPointCloud[offset];   // z

				// Determine fi,fj for color data
				float fi = ((( tmpX - 1.8f) / 0.0023f)/ (-tmpZ - 10)) + 320.0f - 1.0f;
				float fj = (((-tmpY - 2.4f) / 0.0023f)/ (-tmpZ - 10)) + 240.0f - 1.0f;

				// Set Color Value
				int imI = floor(fi);
				int imJ = floor(fj);
				if ( (imI >= 0) && (imI < 640-2) && (imJ >= 0) && (imJ < 480-2) ) {
					int imSum = 0;
					int imOffset3 = (((imJ*640)+imI)*3);
					for (int count3 = 0; count3 < 3; count3++) { 
						for (int count33 = 0; count33 < 3; count33++) {
							imSum += ((int)K->mColorBuffer[imOffset3++] + 
								(int)K->mColorBuffer[imOffset3++] + (int)K->mColorBuffer[imOffset3++]);
						}
						imOffset3 += -9 + 640;
					}
					colorPointCloud[imOffset++] = imSum / (9.0 * 3.0 * 255.0);
				} else {
					colorPointCloud[imOffset++] = 999999.0f;
				}

				// Reorient Y-Axis to Gravity
				depthPointCloud[offset++] = (tmpX*R13)+(tmpY*R23)+(tmpZ*R33);  // z -> y
				depthPointCloud[offset++] = (tmpX*R12)+(tmpY*R22)+(tmpZ*R32);  // y -> x					
				depthPointCloud[offset--] = tmpX = (tmpX*R11)+(tmpY*R21)+(tmpZ*R31); // x -> y
				tmpY = depthPointCloud[offset--]; // y -> z
				tmpZ = depthPointCloud[offset]; // z

				// Convert to polar coordinates
				depthPointCloud[offset++] = tmpY; // height -> dir
				depthPointCloud[offset++] = atan2(tmpZ,tmpX); // dir -> dis
				depthPointCloud[offset--] = sqrt((tmpZ*tmpZ)+(tmpX*tmpX)); // dis -> dir

				// Check for min and max
				// Dir
				if (depthPointCloud[offset] < currentMinDir) { // dir
					currentMinDir = depthPointCloud[offset]; // dir
					tmpX++;
				} else {
					if (depthPointCloud[offset] > currentMaxDir) { // dir
						currentMaxDir = depthPointCloud[offset]; // dir
						tmpX++;
					}
				}
				// Height
				if (depthPointCloud[--offset] < currentMinHeight) { // height 
					currentMinHeight = depthPointCloud[offset]; // height
				}
				offset += 3; // height -> next
			}
		}
	}
		
	// Determine height slice (polar coordinates)
	float heightDiffList[640/DEPTH_SCALE_FACTOR];
	for (int i = 0; i < 640/DEPTH_SCALE_FACTOR; i++) {
		heightSlices[i*2] = 999999.0f;
		heightDiffList[i] = 999999.0f;
	}
	currentMinHeight += 150.0;
	offset = 0;
	imOffset = 0;
	float factor = (640.0f/DEPTH_SCALE_FACTOR)/(currentMaxDir - currentMinDir + 0.000001f);
	for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*(480/DEPTH_SCALE_FACTOR); i++) {
		if (depthPointCloud[offset++] == 999999.0f) { // height -> dir
			offset += 2; // -> next
			imOffset++;
		} else {
			int dirIndex = (int)floor((depthPointCloud[offset--]-currentMinDir)*factor); // dir -> height
			float heightDiff = abs(depthPointCloud[offset++]-currentMinHeight); // height -> dir
			if ((heightDiff < 10) && (heightDiff < heightDiffList[dirIndex])) {
				if (abs(dirIndex) > 40) {
					dirIndex++;
				}
				heightDiffList[dirIndex] = heightDiff;
				heightSlices[dirIndex*2] = depthPointCloud[offset++]; // dir -> dis
				heightSlices[(dirIndex*2)+1] = depthPointCloud[offset++]; // dis -> next
				heightSliceColors[dirIndex] = colorPointCloud[imOffset++]; // Set color value
			} else {
				offset += 2; // -> next
				imOffset++;
			}
		}
	}
		
	// Convert Slices to Cartesian
	for (int i=0; i < (640/DEPTH_SCALE_FACTOR)*2; ) {
		if (heightSlices[i] != 999999.0f) {
			float tmpDir = heightSlices[i++]; // Dir -> Dis
			float tmpDis = heightSlices[i--]; // Dis -> Dir
			heightSlices[i++] = tmpDis*cos(tmpDir); // x -> y
			heightSlices[i++] = tmpDis*sin(tmpDir); // y -> next
		} else {
			i+=2; // -> next
		}
	}

#ifdef RECORD_SLICES
	if (mColorFrameOn > 0) {
		recordSlices(heightSlices, heightSliceColors, 640/DEPTH_SCALE_FACTOR, outFileOn);
	}
#endif

	currentMinHeight -= 150;
	sensorHeight = -currentMinHeight;
}

void update(int value) {
	if ((mDepthFrameOn != mPrevDepthFrameOn) && (mColorFrameOn > 0)) { //New Depth Frame and at least one color frame
		// GET DEPTH BUFFER
		mPrevDepthFrameOn = mDepthFrameOn;
		K->ParseDepthBuffer(); 

		// GET ACCEL DATA
		K->GetAcceleroData(&xAccel, &yAccel, &zAccel);

		// RECORD DATA if recording and if at least one RGB camera frame has been captured
#ifdef RECORD_RAW
		recordColor(K->mColorBuffer, outFileOn);
		recordDepth(K->mDepthBuffer, outFileOn);
		recordAccel(xAccel, yAccel, zAccel, outFileOn);
#endif

		runAlgorithm();
	}

	if (mColorFrameOn != mPrevColorFrameOn) { 
		mPrevColorFrameOn = mColorFrameOn;
		K->ParseColorBuffer();

		glBindTexture( GL_TEXTURE_2D, texID );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, K->mColorBuffer );
	}
	
	glutPostRedisplay();
	glutTimerFunc(10, update, 0);
	outFileOn++;
}

int main(int argc, char **argv)
{
	Kinect::KinectFinder KF;
	if (KF.GetKinectCount() < 1)
	{
		printf("Unable to find Kinect devices... Is one connected?\n");
		return 0;
	}

	K = KF.GetKinect();
	if (K == 0)
	{
		printf("error getting Kinect...\n");
		return 0;
	};
	
	// create a new Listener instance
	L = new Listener();
	
	// register the listener with the kinect. Make sure you remove the 
	// listener before deleting the instance! A good place to unregister 
	// would be your listener destructor.
	K->AddListener(L);

	// SetMotorPosition accepts 0 to 1 range
	//K->SetMotorPosition(1);
	
	// Led mode ranges from 0 to 7, see the header for possible values
	K->SetLedMode(Kinect::Led_Yellow);
	
	// Grab 10 accelerometer values from the kinect
	float x,y,z;
	for (int i =0 ;i<10;i++)
	{
		if (K->GetAcceleroData(&x,&y,&z))
		{
			printf("accelerometer reports: %f,%f,%f\n", x,y,z);
		}
		Sleep(5);
	};

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480); //Set the window size

	//Create the window
	glutCreateWindow("New Algorithm");
	initRendering(); //Initialize rendering

	//Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(10, update, 0);

	//Start Timer
	fpsStopWatch = new CStopWatch();
	fpsStopWatch->startTimer(); 
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.

	return 0;
};
