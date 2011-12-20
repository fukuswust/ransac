#define DISPLAY_CAPTURE_COMMANDS
//#define RECORD_COLORS
//#define RECORD_DEPTH
//#define RECORD_ACCEL
#define RGB_USE_SMOOTHING

#include <stdio.h>
#include <conio.h> // for _kbhit and _getch 
#include <stdlib.h> //Needed for "exit" function
#include "../Kinect-win32.h"
#include <fstream>

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
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
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

	unsigned short centerDepth = (unsigned short)(100.0f/(-0.00307f * (float)(K->mDepthBuffer[153920]) + 3.33f));

	//Accel Coordinates (3D)
	if ((K->mDepthBuffer[153920]) != 2047) {
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
		glPopMatrix();
	}

	// Draw Crosshair (in 2D)
	orthogonalStart();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f((viewWidth/2)-6, (viewHeight/2));
	glVertex2f((viewWidth/2)+5, (viewHeight/2));
	glVertex2f((viewWidth/2), (viewHeight/2)-5);
	glVertex2f((viewWidth/2), (viewHeight/2)+6);
	glEnd();

	if ((K->mDepthBuffer[153920]) == 2047) {
		glColor3f(1.0f, 0.0f, 0.0f);
		sprintf(printBuff, "Sensor Error");
	} else {
		glColor3f(1.0f, 1.0f, 1.0f);
		sprintf(printBuff, "Distance: %u cm (%u in)", centerDepth, (unsigned int)((float)centerDepth/2.54f));
	}
	orthoPrint((viewWidth/2)+10, (viewHeight/2)+11, printBuff);

	glColor3f(1.0f, 1.0f, 1.0f);

	//Draw Frame Count (in 2D)
	sprintf(printBuff, "Frame: %u", mDepthFrameOn);
	orthoPrint(10, 20, printBuff);

	//Draw Accel Data (in 2D)
	sprintf(printBuff, "xAccel: %f", (xAccelAvg*512.0f)/819.0f);
	orthoPrint(120, 20, printBuff);
	sprintf(printBuff, "yAccel: %f", (yAccelAvg*512.0f)/819.0f);
	orthoPrint(300, 20, printBuff);
	sprintf(printBuff, "zAccel: %f", (zAccelAvg*512.0f)/819.0f);
	orthoPrint(480, 20, printBuff);

	orthogonalEnd();

	//Send the scene to the screen
	glutSwapBuffers();
}

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
	glutTimerFunc(25, update, 0);
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
	glutTimerFunc(25, update, 0);

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.

	return 0;
};

