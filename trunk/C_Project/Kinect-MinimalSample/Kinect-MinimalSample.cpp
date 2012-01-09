#define DISPLAY_CAPTURE_COMMANDS
//#define RECORD_COLORS
//#define RECORD_DEPTH
//#define RECORD_ACCEL
#define RGB_USE_SMOOTHING
#define COMPUTE
#define DEPTH_SCALE_FACTOR 16

#define PI 3.14159265

#include <math.h>
#include <stdio.h>
#include <conio.h> // for _kbhit and _getch 
#include <stdlib.h> //Needed for "exit" function
#include "../Kinect-win32.h"
#include <fstream>
#include "hr_time.h"

// The "Kinect" Project has been added to the project dependencies of this project. 

//Include OpenGL header files, so that we can use OpenGL
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int outFileOn = 0;
int mDepthFrameOn = 0;
int mPrevDepthFrameOn = 0;
int mColorFrameOn = 0;
int mPrevColorFrameOn = 0;
int viewWidth = 640;
int viewHeight = 480;
int viewXOffset = 0;
int viewYOffset = 0;
float xAccel = 0.0f;
float yAccel = 0.0f;
float zAccel = 0.0f;
float xAccelAvg = 0.0f;
float yAccelAvg = 0.0f;
float zAccelAvg = 0.0f;
float sensorHeight = 0.0f;
float avgTime = 0.0f;
float avgFrameTime = 0.0f;

CStopWatch* myStopWatch;
CStopWatch* fpsStopWatch;

float heightSlices[(640/DEPTH_SCALE_FACTOR)*2];
float dirRange = 0.0f;

struct Vector {
	float x;
	float y;
	float z;
};

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

void orthogonalStart (void) {
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
			delete myStopWatch;
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

float _angle = 0.0f;

//Draws the 3D scene


void glRectBorder(int x1, int y1, int x2, int y2){
	// Draws the border for a rectangle with the four corners, similar to glRectd
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}

// From http://slabode.exofire.net/circle_draw.shtml - In the public domain
void DrawCircle(float cx, float cy, float r, int num_segments) 
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
void DrawCircleSolid(float cx, float cy, float r, int num_segments) 
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

void DrawCircleHash(float cx, float cy, float r, float l, int num_segments) 
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



void drawScene() {
	char printBuff[256];
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw RGB Camera (in 2D)
	orthogonalStart();
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
	orthogonalEnd();

	//Draw 3D Scene
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	/*
	glPushMatrix();

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(_angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_QUADS); //Begin quadrilateral coordinates

	//Trapezoid
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.7f, -1.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.7f, -1.5f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.4f, -0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.4f, -0.5f, 0.0f);

	glEnd(); //End quadrilateral coordinates

	glBegin(GL_TRIANGLES); //Begin triangle coordinates

	//Pentagon
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(1.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 1.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 1.0f, 0.0f);
	glVertex3f(1.5f, 0.5f, 0.0f);
	glVertex3f(1.5f, 1.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 1.0f, 0.0f);
	glVertex3f(1.5f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.5f, 0.0f);

	//Triangle
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.5f, 0.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1.5f, 0.5f, 0.0f);

	glEnd(); //End triangle coordinates

	glPopMatrix();
	*/

	/*//Accel Coordinates (3D)
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glTranslatef(0.0f, 0.0f, -((float)centerDepth)/25.0f);
	glLineWidth(4.0);
	glBegin(GL_LINES);
	// Z Axis
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, zAccelAvg/3.0f, -yAccelAvg/3.0f);
	glVertex3f(0, -zAccelAvg/3.0f, yAccelAvg/3.0f);
	// Y Axis
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-xAccelAvg/3.0f, yAccelAvg/3.0f, zAccelAvg/3.0f);
	// X Axis
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(yAccelAvg/3.0f, xAccelAvg/3.0f, 0);
	glVertex3f(-yAccelAvg/3.0f, -xAccelAvg/3.0f, 0);
	glEnd();
	glLineWidth(1.0);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();*/

	// Draw Crosshair (in 2D)
	orthogonalStart();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f((viewWidth/2)-6, (viewHeight/2));
	glVertex2f((viewWidth/2)+5, (viewHeight/2));
	glVertex2f((viewWidth/2), (viewHeight/2)-5);
	glVertex2f((viewWidth/2), (viewHeight/2)+6);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	//Draw Frame Count (in 2D)
	fpsStopWatch->stopTimer();
	avgFrameTime = (0.1*(float)(fpsStopWatch->getElapsedTime()))+(0.9f*avgFrameTime);
	sprintf(printBuff, "FPS: %u", (unsigned int)(1.0f/avgFrameTime));
	orthoPrint(10, 20, printBuff);
	fpsStopWatch->startTimer(); 

	/*
	//Draw Height
	float heightValue = sensorHeight;
	sprintf(printBuff, "Height: %d", (int)heightValue);
	orthoPrint(120, 20, printBuff);
	*/

	//// Draw Height Measurement Bar - this code is painfull to read...
#define HEIGHT_MEAUSUREMENT_BAR_TOPX 0
#define HEIGHT_MEAUSUREMENT_BAR_TOPY 0
	// Draw Background behind all
	glColor3f(1.0f, 1.0f, 1.0f); // White
	glRectf(HEIGHT_MEAUSUREMENT_BAR_TOPX, HEIGHT_MEAUSUREMENT_BAR_TOPY, 45.0f, 195.0f);
	// Black border
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	glRectBorder(HEIGHT_MEAUSUREMENT_BAR_TOPX, HEIGHT_MEAUSUREMENT_BAR_TOPY, 45.0f, 195.0f);
	// Draw Background - white to red gradiant
	glColor3f(1.0f, 1.0f, 1.0f); // White
	//glRectf(5, 5, 20, 185);
	glBegin(GL_POLYGON);
	glVertex2f(5, 5);
	glVertex2f(20, 5);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	glVertex2f(20, 185);
	glVertex2f(5, 185);
	glEnd();
	// Draw Border - black
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	glRectBorder(5, 5, 20, 185);
	// Calculate and place height bar correctly
	float heightValue = sensorHeight;
	float barLocation;
	barLocation = ((300-heightValue)/300)*185;
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(8, (int)barLocation);
	glVertex2f(17, (int)barLocation);
	glEnd();
	// Draw values next to bar
	orthoPrint(25, 5+9, "3m");
	orthoPrint(25, 65+6, "2m");
	orthoPrint(25, 125+3, "1m");
	orthoPrint(25, 185, "0m");

	// Enable antialiasing.  Do we want to do this?
	glEnable(GL_LINE_SMOOTH);

	// Draw Roll
	float xRollLbl = (viewWidth/3);
	float yRollLbl = 30;
	float radius = 20;
	float rollValue = atan2(xAccelAvg, yAccelAvg);
	glColor3f(1.0f, 1.0f, 1.0f); // White
	DrawCircleSolid(xRollLbl, yRollLbl, radius, 16);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	DrawCircle(xRollLbl, yRollLbl, radius, 16);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	drawCenteredTiltedLine(xRollLbl, yRollLbl, 14, rollValue);
	// Draw hash marks
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	DrawCircleHash(xRollLbl, yRollLbl, radius, 2, 8);

	/*//Draw Roll
	float rollValue = atan2(xAccelAvg, yAccelAvg);
	sprintf(printBuff, "Roll: %d", (int)((rollValue*180.0f)/PI));
	orthoPrint(300, 20, printBuff);*/

	// Draw Pitch
	float xPitchLbl = (2*viewWidth/3);
	float yPitchLbl = 30;
	float pitchValue = atan2(zAccelAvg, yAccelAvg);
	glColor3f(1.0f, 1.0f, 1.0f); // White
	DrawCircleSolid(xPitchLbl, yPitchLbl, 20, 16);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	DrawCircle(xPitchLbl, yPitchLbl, 20, 16);
	drawCenteredTiltedLine(xPitchLbl, yPitchLbl, 15, pitchValue);

	/*//Draw Pitch
	float pitchValue = atan2(zAccelAvg, yAccelAvg);
	sprintf(printBuff, "Pitch: %d", (int)((pitchValue*180.0f)/PI));
	orthoPrint(480, 20, printBuff);*/

	// Draw Local Top Down Map Background
#define MAP_BACK_X 100
#define MAP_BACK_Y 100
	glColor3f(1.0f, 1.0f, 1.0f); // White
	DrawCircleSolid(MAP_BACK_X, MAP_BACK_Y, 100, 32);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	DrawCircle(MAP_BACK_X, MAP_BACK_Y, 100, 32);

	//Draw Local Top Down Map (in 2D, upper right)
	float delX = 15.0f*cos(dirRange/2.0f);
	float delY = 15.0f*sin(dirRange/2.0f);
	//Draw Origin
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(viewWidth-150, 150);
	glVertex2f(viewWidth-150+delX, 150-delY);
	glVertex2f(viewWidth-150, 150);
	glVertex2f(viewWidth-150-delX, 150-delY);
	glEnd();
	//Draw Slice
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	for (int i=0; i < (640/DEPTH_SCALE_FACTOR)*2; ) {
		if (heightSlices[i] != 999999.0f) {
			glVertex2f(viewWidth-150+heightSlices[i++]/5.0f, 150+heightSlices[i++]/5.0f);
		} else {
			i+=2;
		}
	}
	glEnd();
	glPointSize(1.0f);

	//Return to Default
	glColor3f(1.0f, 1.0f, 1.0f);



	orthogonalEnd();

	//Send the scene to the screen
	glutSwapBuffers();
}

#define FX_D 5.9421434211923247e+02
#define FY_D 5.9104053696870778e+02
#define CX_D 3.3930780975300314e+02
#define CY_D 2.4273913761751615e+02

void update(int value) {
	if (mDepthFrameOn != mPrevDepthFrameOn) {
		mPrevDepthFrameOn = mDepthFrameOn;
		K->ParseDepthBuffer();	

		int avgAmount = 1;
		K->GetAcceleroData(&xAccel, &yAccel, &zAccel);
		xAccelAvg = ((xAccelAvg*(avgAmount-1))+xAccel)/(avgAmount);
		yAccelAvg = ((yAccelAvg*(avgAmount-1))+yAccel)/(avgAmount);
		zAccelAvg = ((zAccelAvg*(avgAmount-1))+zAccel)/(avgAmount);

		char bufferFileName[128];
#ifdef DISPLAY_CAPTURE_COMMANDS
		printf("Center Distance: %f\n",100.0f/(-0.00307f * (float)(K->mDepthBuffer[153920]) + 3.33f));
#endif
#ifdef RECORD_COLORS
		if (mColorFrameOn > 0) {
			sprintf(bufferFileName, "colorData/%.6u_R.csv", outFileOn);
			FILE * redFile = fopen(bufferFileName,"w");
			int i = 0;
			while (i < 640*480) {
				for (int x = 0; x < 640-1; x++) {
					fprintf(redFile,"%u,",K->mColorBuffer[((i++)*3)+0]);
				}
				fprintf(redFile,"%u\n",K->mColorBuffer[((i++)*3)+0]);
			}
			fclose(redFile);
			
			sprintf(bufferFileName, "colorData/%.6u_G.csv", outFileOn);
			FILE * greenFile = fopen(bufferFileName,"w");
			i = 0;
			while (i < 640*480) {
				for (int x = 0; x < 640-1; x++) {
					fprintf(greenFile,"%u,",K->mColorBuffer[((i++)*3)+1]);
				}
				fprintf(greenFile,"%u\n",K->mColorBuffer[((i++)*3)+1]);
			}
			fclose(greenFile);

			sprintf(bufferFileName, "colorData/%.6u_B.csv", outFileOn);
			FILE * blueFile = fopen(bufferFileName,"w");
			i = 0;
			while (i < 640*480) {
				for (int x = 0; x < 640-1; x++) {
					fprintf(blueFile,"%u,",K->mColorBuffer[((i++)*3)+1]);
				}
				fprintf(blueFile,"%u\n",K->mColorBuffer[((i++)*3)+1]);
			}
			fclose(blueFile);
		}
#endif
#ifdef RECORD_DEPTH
		if (mColorFrameOn > 0) {
			sprintf(bufferFileName, "depthData/%.6u.csv", outFileOn);
			FILE * depthFile = fopen(bufferFileName,"w");
			int i = 0;
			while (i < 640*480) {
				for (int x = 0; x < 640-1; x++) {
					fprintf(depthFile,"%u,",K->mDepthBuffer[i++]);
				}
				fprintf(depthFile,"%u\n",K->mDepthBuffer[i++]);
			}
			fclose(depthFile);
		}
#endif
#ifdef RECORD_ACCEL
		if (mColorFrameOn > 0) {
			sprintf(bufferFileName, "accelData/%.6u.csv", outFileOn);
			FILE * accelFile = fopen(bufferFileName,"w");
			fprintf(accelFile,"%f,",xAccel);
			fprintf(accelFile,"%f,",yAccel);
			fprintf(accelFile,"%f",zAccel);
			fclose(accelFile);
		}
#endif
		outFileOn++;

#ifdef COMPUTE
		myStopWatch->startTimer();

		// GET GRAVITY ROTATION MATRIX
		// Get Unit Vectors
		//magA = sqrt(sum(A.*A));
		//uA = A./magA;
		float gravMag = sqrt((xAccelAvg*xAccelAvg)+(yAccelAvg*yAccelAvg)+(zAccelAvg*zAccelAvg)); //Quality = diff from 819/512
		float uGravX = xAccelAvg/gravMag;
		float uGravY = yAccelAvg/gravMag;
		float uGravZ = zAccelAvg/gravMag;
		//c = dot(uA,uB);
		//c = uGravY
		//s = sqrt(1-(c*c));
		float s = sqrt(1-(uGravY*uGravY));
		//t = 1 - c;
		float t = 1-uGravY;
		//RotAxis = cross(uA,uB);
		float RotAxisX = -uGravZ;
		//    RotAxisY = 0;
		float RotAxisZ = uGravX;
		float magRotAxis = sqrt((RotAxisX*RotAxisX)+(RotAxisZ*RotAxisZ));
		//uRotAxis = RotAxis./magRotAxis;
		float uX = RotAxisX/magRotAxis;
		//    uY = 0;
		float uZ = RotAxisZ/magRotAxis;
		
		float xz = uX*uZ;
		float sx = s*uX;
		float sz = s*uZ;
		// Calculate Matrix Elements
		float R11 = uGravY + (t*uX*uX);
		float R12 = -sz;
		float R13 = t*xz;
		float R21 = sz;
		float R22 = uGravY;
		float R23 = -sx;
		float R31 = t*xz;
		float R32 = sx;
		float R33 = uGravY + (t*uZ*uZ);

		printf("\n");
		printf("%f, %f, %f\n", R11, R12, R13);
		printf("%f, %f, %f\n", R21, R22, R23);
		printf("%f, %f, %f\n\n", R31, R32, R33);

		// Move Depth Data into Local Array of Floats (40x30x3 on Stack)
		float depthPointCloud[((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))*3]; // Stored in the order of [z,y,x]
		int offset = 0;
		float currentMinHeight = 999999.0f;
		float currentMinDir = 999999.0f;
		float currentMaxDir = -999999.0f;

		for (int j = 0; j < 480; j+=DEPTH_SCALE_FACTOR) {
			for (int i = 0; i < 640; i+=DEPTH_SCALE_FACTOR) {
				//Acquire Raw Depth Value
				float z;
				z = (float)(K->mDepthBuffer[(j*640)+i]);
				
				//Check Sensor Data for Error
				if (z == 2047.0f) {
					depthPointCloud[offset] = 999999.0f;
					offset += 3;
				} else {
					// Depth to Z
					depthPointCloud[offset++] = z = -100.0f/((-0.00307f * z) + 3.33f); //z -> y
					// Z to Point Cloud
					//depthPointCloud[offset++] = (((j*16) - CY_D) * z) / FY_D;  //y-> x
					//depthPointCloud[offset] = (((i*16) - CX_D) * z) / -FX_D; //x
					depthPointCloud[offset++] = (float)(j - 240) * (z - 10.0f) * 0.0021f ; //y -> x
					depthPointCloud[offset] = (float)(i - 320) * (z - 10.0f) * -0.0021f ;  //x
					 
					// Reorient Y-Axis to Gravity
					float tmpX = depthPointCloud[offset--]; // x -> y
					float tmpY = depthPointCloud[offset--]; // y -> z
					float tmpZ = depthPointCloud[offset];   // z
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

		// Determine Direction Range and Midpoint
		dirRange = currentMaxDir - currentMinDir;
		float dirMid = currentMinDir + (dirRange);
		
		// Determine height slice (polar coordinates)
		float heightDiffList[640/DEPTH_SCALE_FACTOR];
		for (int i = 0; i < 640/DEPTH_SCALE_FACTOR; i++) {
			heightSlices[i*2] = 999999.0f;
			heightDiffList[i] = 999999.0f;
		}
		currentMinHeight += 150.0;
		offset = 0;
		float factor = (640.0f/DEPTH_SCALE_FACTOR)/(currentMaxDir - currentMinDir + 0.000001f);
		for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*(480/DEPTH_SCALE_FACTOR); i++) {
			if (depthPointCloud[offset++] == 999999.0f) { //height
				offset += 2;
			} else {
				if (abs(depthPointCloud[offset])>50) {
					factor++;
				}

				int dirIndex = (int)floor((depthPointCloud[offset--]-currentMinDir)*factor); //dir
				float heightDiff = abs(depthPointCloud[offset++]-currentMinHeight); //height
				if ((heightDiff < 10) && (heightDiff < heightDiffList[dirIndex])) {
					if (abs(dirIndex) > 40) {
						dirIndex++;
					}
					heightDiffList[dirIndex] = heightDiff;
					heightSlices[dirIndex*2] = depthPointCloud[offset++]; //dir
					heightSlices[(dirIndex*2)+1] = depthPointCloud[offset++]; //dis
				} else {
					offset += 2;
				}
			}
		}

		// Reorient to 90 degrees
		float dirChange = (PI/2)-dirMid;
		for (int i = 0; i < (640/DEPTH_SCALE_FACTOR)*2; i+=2) {
			if (heightSlices[i] != 999999.0f) {
				heightSlices[i] = heightSlices[i] - (3.0f*PI/2.0f);
			}
		}
		
		// Convert Slices to Cartesian
		for (int i=0; i < (640/DEPTH_SCALE_FACTOR)*2; ) {
			if (heightSlices[i] != 999999.0f) {
				float tmpDir = heightSlices[i++]; //Dir
				float tmpDis = heightSlices[i--]; //Dis
				heightSlices[i++] = -tmpDis*cos(tmpDir); //x
				heightSlices[i++] = tmpDis*sin(tmpDir); //y
			} else {
				i+=2;
			}
		}

		currentMinHeight -= 150;
		sensorHeight = -currentMinHeight;

		myStopWatch->stopTimer();
		avgTime = (0.1*(float)(myStopWatch->getElapsedTime()))+(0.9f*avgTime);

#endif

	}
	if (mColorFrameOn != mPrevColorFrameOn) { 
		mPrevColorFrameOn = mColorFrameOn;
		K->ParseColorBuffer();

		glBindTexture( GL_TEXTURE_2D, texID );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 640, 480,
						  GL_RGB, GL_UNSIGNED_BYTE, K->mColorBuffer );

#ifdef DISPLAY_CAPTURE_COMMANDS
		printf(" Got Color  ");
#endif
	}

	_angle += 5.0;
	if (_angle > 360) {_angle -= 360;}

	glutPostRedisplay();
	glutTimerFunc(10, update, 0);
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
	myStopWatch = new CStopWatch();
	fpsStopWatch = new CStopWatch();
	fpsStopWatch->startTimer(); 
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.

	return 0;
};
