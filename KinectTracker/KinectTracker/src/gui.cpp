#include "gui.h"
#include <string.h>
#include "globals.h"
#include "input.h"
#include "algorithm.h"
#include "basicShapes.h"

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

	/*// Draw Floor Points
	drawFloorPoints(floorIJ, numFloorPoints);

	// Draw Wall Points
	drawWallPoints(wallIJ, numWallPoints);

	// Draw Height Slice
#ifdef DRAW_HEIGHT_SLICE
	drawHeightLine(heightSlices, heightSliceIJ, 640/DEPTH_SCALE_FACTOR);
#endif*/

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

//http://pyopengl.sourceforge.net/documentation/manual/glutBitmapCharacter.3GLUT.html
void orthoPrint(int x, int y, char *string)
{
  int len, i;
  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
  }
}

#define RGB_USE_SMOOTHING
void drawColorBackground(int viewWidth, int viewHeight, GLuint texID){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);
#ifdef RGB_USE_SMOOTHING
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0, viewHeight);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(viewWidth, viewHeight);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(viewWidth, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawCrosshair(int viewWidth, int viewHeight){
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f((viewWidth/2)-6, (viewHeight/2));
	glVertex2f((viewWidth/2)+5, (viewHeight/2));
	glVertex2f((viewWidth/2), (viewHeight/2)-5);
	glVertex2f((viewWidth/2), (viewHeight/2)+6);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
}

// From http://slabode.exofire.net/circle_draw.shtml - In the public domain
void drawCircle(float cx, float cy, float r, int num_segments) 
{ 
	glBegin(GL_LINE_LOOP); 
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 

		float x = r * cosf(theta);//calculate the x component 
		float y = r * sinf(theta);//calculate the y component 

		glVertex2f(x + cx, y + cy);//output vertex 

	} 
	glEnd(); 
}

// From http://slabode.exofire.net/circle_draw.shtml - In the public domain - modified to fill circle with color
void drawCircleSolid(float cx, float cy, float r, int num_segments) 
{ 
	glBegin(GL_TRIANGLE_FAN);
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 

		float x = r * cosf(theta);//calculate the x component 
		float y = r * sinf(theta);//calculate the y component 

		glVertex2f(x + cx, y + cy);//output vertex 

	} 
	// Connect end
	glEnd(); 
}

void drawCenteredTiltedLine(float cx, float cy, float r, float angle) {
	// Draws a line centered at cx, cy, with radius r, and angle "angle" (in radians)
	glBegin(GL_LINES);
	float x = r * cosf(angle);
	float y = r * sinf(angle);
	glVertex2f(cx + x, cy + y);
	glVertex2f(cx - x, cy - y);
	glEnd();
}

void drawCircleHash(float cx, float cy, float r, float l, int num_segments) 
{
	// Draws hashes on the circle at radius r, with length l
	glBegin(GL_LINES);
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 

		float x = r * cosf(theta);//calculate the x component 
		float y = r * sinf(theta);//calculate the y component 
		glVertex2f(x + cx, y + cy);//output vertex

		x = (r-l) * cosf(theta);//calculate the x component 
		y = (r-l) * sinf(theta);//calculate the y component
		glVertex2f(x + cx, y + cy);//output vertex
	} 
	glEnd(); 
}

// Draws height bar with the top left corner at topx, topy.
// Goes from 0 to 3 meters by defaults.  Height input in cm.
void drawHeightHud(int topx, int topy, float height)
{
	// Draw Big White Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f); // White
	glRectf(topx, topy, topx+45.0f, topy+195.0f);
	// Draw Big White Background Border
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawRectBorder(topx, topy, topx+45.0f, topy+195.0f);
	// Draw Sensor Background - white to red gradiant
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f); // White
	glVertex2f(topx+5, topy+5);
	glVertex2f(topx+20, topy+5);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	glVertex2f(topx+20, topy+185);
	glVertex2f(topx+5, topy+185);
	glEnd();
	// Draw Sensor Background Border - black
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawRectBorder(topx+5, topy+5, topx+20, topy+185);
	// Calculate and place height bar correctly
	float barLocation;
	barLocation = ((300-height)/300)*185;
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(topx+8, (int)barLocation+topy);
	glVertex2f(topx+17, (int)barLocation+topy);
	glEnd();
	// Draw values next to bar
	orthoPrint(topx+25, topy+5+9, "3m");
	orthoPrint(topx+25, topy+65+6, "2m");
	orthoPrint(topx+25, topy+125+3, "1m");
	orthoPrint(topx+25, topy+185, "0m");
}

void drawRollHud(int cx, int cy, int r, float roll)
{
	// Draw Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f); // White
	drawCircleSolid(cx, cy, r, 16);
	// Draw Label
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	orthoPrint(cx-11, cy+15, "Roll");
	// Draw Outline
	drawCircle(cx, cy, r, 16);
	// Draw hash marks
	drawCircleHash(cx, cy, r, 2, 8);
	// Draw Needle
	glLineWidth(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	drawCenteredTiltedLine(cx, cy, r-6, roll);
	glLineWidth(1.0f);
	// Draw center point
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleSolid(cx, cy, 3.5, 8);
}

void drawPitchHud(int cx, int cy, int r, float pitch)
{
	// Draw Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f); // White
	drawCircleSolid(cx, cy, r, 16);
	// Draw Label
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	orthoPrint(cx-11, cy+15, "Pitch");
	// Draw Outline
	drawCircle(cx, cy, r, 16);
	// Draw hash marks
	drawCircleHash(cx, cy, r, 2, 8);
	// Draw Needle
	glLineWidth(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	drawCenteredTiltedLine(cx, cy, r-6, pitch);
	glLineWidth(1.0f);
	// Draw center point
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleSolid(cx, cy, 3.5, 8);
}

void drawTopDownMap(int cx, int cy, int r) {
	//Draw Local Top Down Map Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // White
	drawCircleSolid(cx, cy, r, 32);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 32);
	//Draw Local Top Down Map 
	float delX = 15.0f*cos((50.0f*PI)/180.0f);
	float delY = 15.0f*sin((50.0f*PI)/180.0f);
	//Draw Camera
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(cx, cy);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cx+delX, cy-delY);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cx-delX, cy-delY);
	glEnd();
	// Draw Local Wall Corner Points
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < numCorners; i++) {
		float tmpX = wallCorners[(i*6)+0]/(MAX_ALLOWED_DIS/r);
		float tmpZ = wallCorners[(i*6)+1]/(MAX_ALLOWED_DIS/r);
		float cornerType = wallCorners[(i*6)+2];
		if (cornerType == 1) { //false
			glColor3f(1.0f, 0.0f, 0.0f);
		} else {
			glColor3f(0.0f, 1.0f, 0.0f);
		}
		glVertex2f(cx+tmpX, cy+tmpZ);
	}
	// Draw Local Wall Corner Connectors
	glEnd();
	glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < numCorners; i++) {
		float tmpX = wallCorners[(i*6)+0]/(MAX_ALLOWED_DIS/r);
		float tmpZ = wallCorners[(i*6)+1]/(MAX_ALLOWED_DIS/r);
		float cornerConnectivity = wallCorners[(i*6)+3];
		if (cornerConnectivity == 0) { //Not Connected
			glEnd();
			glBegin(GL_LINE_STRIP);
			glVertex2f(cx+tmpX, cy+tmpZ);
		} else {
			glVertex2f(cx+tmpX, cy+tmpZ);
		}		
	}
	glPointSize(7.0f);
	glEnd();
	//Draw Global Map
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i=0; i < numGlobalCorners; i++) {
		float tmpX = globalMapCorners[i].x/(MAX_ALLOWED_DIS/r);
		float tmpZ = globalMapCorners[i].z/(MAX_ALLOWED_DIS/r);
		glVertex2f(cx+tmpX, cy+tmpZ);
	}
	glEnd();
	glPointSize(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawHeightLine(float heightSlices[], int heightSliceIJ[], int numSlices) {
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (int i=0, ij=0; i < numSlices*2; i+=2) {
		if (heightSlices[i] != 999999.0f) {
			int tmpI = heightSliceIJ[ij++]*xViewFactor;
			int tmpJ = heightSliceIJ[ij++]*yViewFactor;
			glVertex2f(tmpI, tmpJ);
		} else {
			ij+=2;
		}
	}
	glEnd();
	glLineWidth(1.0f);
}

void drawFloorPoints(int floorIJ[], int numPoints) {
	glColor3f(0.0f, 1.0f, 0.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (int ij=0; ij < numPoints*2; ) {
		int tmpI = floorIJ[ij++]*xViewFactor;
		int tmpJ = floorIJ[ij++]*yViewFactor;
		glVertex2f(tmpI, tmpJ);
	}
	glEnd();
	glLineWidth(1.0f);
}

void drawWallPoints(int wallIJ[], int numPoints) {
	glColor3f(0.0f, 0.0f, 1.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (int ij=0; ij < numPoints*2; ) {
		int tmpI = wallIJ[ij++]*xViewFactor;
		int tmpJ = wallIJ[ij++]*yViewFactor;
		glVertex2f(tmpI, tmpJ);
	}
	glEnd();
	glLineWidth(1.0f);
}

void drawAugmentedPoint(float x, float y, float z) {
	// Get X,Y,Z Coordinates
	float transX = translationMatrix[0] - x;
	float transY = translationMatrix[1] - y;
	float transZ = translationMatrix[2] - z;

	// Apply Yaw Rotation
	float yawTmpX = (yawMatrix[0]*transX) + (yawMatrix[1]*transY) + (yawMatrix[2]*transZ);
	float yawTmpY = (yawMatrix[3]*transX) + (yawMatrix[4]*transY) + (yawMatrix[5]*transZ);
	float yawTmpZ = (yawMatrix[6]*transX) + (yawMatrix[7]*transY) + (yawMatrix[8]*transZ);

	// Apply Pitch and Roll
	float fx = (pitchRollMatrix[0]*yawTmpX) + (pitchRollMatrix[1]*yawTmpY) + (pitchRollMatrix[2]*yawTmpZ);
	float fy = (pitchRollMatrix[3]*yawTmpX) + (pitchRollMatrix[4]*yawTmpY) + (pitchRollMatrix[5]*yawTmpZ);
	float fz = (pitchRollMatrix[6]*yawTmpX) + (pitchRollMatrix[7]*yawTmpY) + (pitchRollMatrix[8]*yawTmpZ);

	float fi = ((( fx - 1.8f) / 0.0023f)/ (-fz - 10)) + 320.0f - 1.0f;
	float fj = (((-fy - 2.4f) / 0.0023f)/ (-fz - 10)) + 240.0f - 1.0f;

	glVertex2f(fi*xViewFactor, fj*yViewFactor);
}

void drawTopDownViewPoint(float x, float y, float z) {
	// Get X,Y,Z Coordinates
	float transX = -x;
	float transY = translationMatrix[1] - y;
	float transZ = -z;

	// Apply Pitch and Roll
	float fx = (pitchRollMatrix[0]*transX) + (pitchRollMatrix[1]*transY) + (pitchRollMatrix[2]*transZ);
	float fy = (pitchRollMatrix[3]*transX) + (pitchRollMatrix[4]*transY) + (pitchRollMatrix[5]*transZ);
	float fz = (pitchRollMatrix[6]*transX) + (pitchRollMatrix[7]*transY) + (pitchRollMatrix[8]*transZ);

	float fi = ((( fx - 1.8f) / 0.0023f)/ (-fz - 10)) + 320.0f - 1.0f;
	float fj = (((-fy - 2.4f) / 0.0023f)/ (-fz - 10)) + 240.0f - 1.0f;

	glVertex2f(fi*xViewFactor, fj*yViewFactor);
}

void drawAugmentedCube(float x, float y, float z, float s) {
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x+s, y, z);
	glColor3f(0.0f, 1.0f, 0.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x, y+s, z);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x, y, z+s);

	glColor3f(0.0f, 0.0f, 0.0f);
	drawAugmentedPoint(x+s, y+s, z+s);
	drawAugmentedPoint(x, y+s, z+s);
	drawAugmentedPoint(x+s, y+s, z+s);
	drawAugmentedPoint(x+s, y, z+s);
	drawAugmentedPoint(x+s, y+s, z+s);
	drawAugmentedPoint(x+s, y+s, z);

	drawAugmentedPoint(x+s, y, z);
	drawAugmentedPoint(x+s, y+s, z);
	drawAugmentedPoint(x+s, y, z);
	drawAugmentedPoint(x+s, y, z+s);

	drawAugmentedPoint(x, y+s, z);
	drawAugmentedPoint(x+s, y+s, z);
	drawAugmentedPoint(z, y+s, z);
	drawAugmentedPoint(x, y+s, z+s);

	drawAugmentedPoint(x, y, z+s);
	drawAugmentedPoint(x+s, y, z+s);
	drawAugmentedPoint(x, y, z+s);
	drawAugmentedPoint(x, y+s, z+s);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}

void drawAugmentedCorners() {
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numCorners; i++) {
		float tmpX = wallCorners[(i*6)+0];
		float tmpZ = wallCorners[(i*6)+1];
		float cornerType = wallCorners[(i*6)+2];
		if (cornerType == 1) { //false
			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		} else {
			glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		}
		drawTopDownViewPoint(tmpX,0,tmpZ);
		drawTopDownViewPoint(tmpX,300,tmpZ);
	}
	glEnd();

	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < numCorners; i++) {
		float tmpX = wallCorners[(i*6)+0];
		float tmpZ = wallCorners[(i*6)+1];
		float cornerConnectivity = wallCorners[(i*6)+3];
		if (cornerConnectivity == 0) { //Not Connected
			glEnd();
			glBegin(GL_LINE_STRIP);
			drawTopDownViewPoint(tmpX,150.0f,tmpZ);
		} else {
			drawTopDownViewPoint(tmpX,150.0f,tmpZ);
		}		
	}
	glEnd();
	glLineWidth(1.0f);
}