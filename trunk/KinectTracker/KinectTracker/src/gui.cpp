#include "gui.h"
#include <string.h>
#include "globals.h"
#include "input.h"
#include "algorithm.h"
#include "basicShapes.h"
#include "hud.h"
#include "augment.h"

void initGui(int argc, char **argv) {
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
	update(4);
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
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

// Main Update Loop
void update(int value) {
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, K->mColorBuffer );
	glutPostRedisplay();
	glutTimerFunc(0, update, 0);
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

void drawScene() {
	char printBuff[256];
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw 2D Scene in Background
	orthogonalStart (viewWidth, viewHeight);

	// Draw RGB Camera in background (in 2D)
	drawColorBackground(viewWidth, viewHeight, texID);

	// Draw Floor Points
	drawFloorPoints(floorIJ, numFloorPoints);

	// Draw Wall Points
	drawWallPoints(wallIJ, numWallPoints);

	// Draw Height Slice
#ifdef DRAW_HEIGHT_SLICE
	drawHeightLine(heightSlices, heightSliceIJ, 640/DEPTH_SCALE_FACTOR);
#endif

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
	//drawAugmentedCube(0.0f, 0.0f, 0.0f, CUBE_SIZE);

	//Draw Wall Corners
	drawAugmentedCorners();

	//Draw Frame Count (in 2D)
#define HUD_FPS_X 5
#define HUD_FPS_Y 7
	glColor3f(1.0f, 1.0f, 1.0f);
	fpsStopWatch->stopTimer();
	avgFrameTime = (0.1*(float)(fpsStopWatch->getElapsedTime()))+(0.9f*avgFrameTime);
	sprintf(printBuff, "FPS: %u", (unsigned int)(1.0f/avgFrameTime));
	orthoPrint(HUD_FPS_X, viewHeight - HUD_FPS_Y, printBuff);
	fpsStopWatch->startTimer();

	//Yaw
	glColor3f(1.0f, 1.0f, 1.0f);
	sprintf(printBuff, "Yaw: %f", (yawValue/PI)*180.0f);
	orthoPrint(150, viewHeight - HUD_FPS_Y, printBuff);

	//X
	glColor3f(1.0f, 1.0f, 1.0f);
	sprintf(printBuff, "X: %f", xValue);
	orthoPrint(300, viewHeight - HUD_FPS_Y, printBuff);

	//Z
	glColor3f(1.0f, 1.0f, 1.0f);
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
#define HUD_MAP_CIRCLE_SIZE 200
#define HUD_MAP_X (5 + HUD_MAP_CIRCLE_SIZE)
#define HUD_MAP_Y (5 + HUD_MAP_CIRCLE_SIZE)
	drawTopDownMap(viewWidth - HUD_MAP_X, HUD_MAP_Y, HUD_MAP_CIRCLE_SIZE);

	//Return to Default
	glColor3f(1.0f, 1.0f, 1.0f);
	orthogonalEnd();

	//Send the scene to the screen
	glutSwapBuffers();
}

void orthogonalStart (int viewWidth, int viewHeight) {
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, viewWidth, 0, viewHeight);
	glDisable(GL_DEPTH_TEST);
	glScalef(1, -1, 1);
	glTranslatef(0, -viewHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void orthogonalEnd (void) {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

