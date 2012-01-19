#include "globals.h"
#include "algorithm.h"

#include <math.h>
#include <stdio.h>
#include <conio.h> // for _kbhit and _getch 
#include <stdlib.h> //Needed for "exit" function
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

			//Deallocate Wall Arrays
			free(curWallSlice);
			free(prevWallSlice);
	
			// when the KinectFinder instance is destroyed, it will tear down and free all kinects.
			exit(0); //Exit the program
		}
	}
}

//Initializes 3D rendering
void initRendering() {
	//Initialize Texture Space for RGB Camera
	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 640, 480,
							  0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	//Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function
	// Enable antialiasing.  Do we want to do this?
	glEnable(GL_LINE_SMOOTH);
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
	xViewFactor = viewWidth/640.0f;
	yViewFactor = viewHeight/480.0f;

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

//Draws the Scene
void drawScene() {
	char printBuff[256];
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw 2D Scene in Background
	orthogonalStart (viewWidth, viewHeight);

	// Draw RGB Camera in background (in 2D)
	drawColorBackground(viewWidth, viewHeight, texID);

	/*
	// Draw Floor Points
	drawFloorPoints(floorIJ, numFloorPoints);

	// Draw Wall Points
	drawWallPoints(wallIJ, numWallPoints);

	// Draw Height Slice
#ifdef DRAW_HEIGHT_SLICE
	drawHeightLine(heightSlices, heightSliceIJ, 640/DEPTH_SCALE_FACTOR);
#endif
	*/

	orthogonalEnd();

	//Draw 3D Scene
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	//Draw 2D Overlay
	orthogonalStart(viewWidth, viewHeight);

	// Draw Crosshair (in 2D)
	drawCrosshair(viewWidth, viewHeight);

	// Draw Augmentation
    #define CUBE_SIZE 50.0f
	drawAugmentedCube(0.0f, 0.0f, 0.0f, CUBE_SIZE);

	//Draw Frame Count (in 2D)
#define HUD_FPS_X 5
#define HUD_FPS_Y 7
	fpsStopWatch->stopTimer();
	avgFrameTime = (0.1*(float)(fpsStopWatch->getElapsedTime()))+(0.9f*avgFrameTime);
	sprintf(printBuff, "FPS: %u", (unsigned int)(1.0f/avgFrameTime));
	orthoPrint(HUD_FPS_X, viewHeight - HUD_FPS_Y, printBuff);
	fpsStopWatch->startTimer();

	//Yaw
	sprintf(printBuff, "Yaw: %f", (yawValue/PI)*180.0f);
	orthoPrint(150, viewHeight - HUD_FPS_Y, printBuff);

	//X
	sprintf(printBuff, "X: %f", xValue);
	orthoPrint(300, viewHeight - HUD_FPS_Y, printBuff);

	//Z
	sprintf(printBuff, "Z: %f", zValue);
	orthoPrint(450, viewHeight - HUD_FPS_Y, printBuff);

	// Draw Height Measurement Bar
#define HUD_HEIGHT_BAR_X 5
#define HUD_HEIGHT_BAR_Y 5
	drawHeightHud(HUD_HEIGHT_BAR_X, HUD_HEIGHT_BAR_Y, heightValue);

	// Draw Roll
#define HUD_ROLL_RADIUS 30
	float xRollLbl = 95;
	float yRollLbl = 48;
	drawRollHud(xRollLbl, yRollLbl, HUD_ROLL_RADIUS, rollValue);
	
	// Draw Pitch
#define HUD_PITCH_RADIUS 30
	float xPitchLbl = 95;
	float yPitchLbl = 130;
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

	//Allocate Wall Arrays
	curWallSlice = (double*)calloc((640/DEPTH_SCALE_FACTOR)*2,sizeof(double));
	prevWallSlice = (double*)calloc((640/DEPTH_SCALE_FACTOR)*2,sizeof(double));

	//Start Timer
	fpsStopWatch = new CStopWatch();
	fpsStopWatch->startTimer(); 
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.

	return 0;
};
