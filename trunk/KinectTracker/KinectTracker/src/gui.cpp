#include "gui.h"
#include <string>
#include "globals.h"
#include "input.h"
#include "algorithm.h"
#include "basicShapes.h"
#include "hud.h"
#include "augment.h"
#include "augModel.h"
#include "NeheTGATexture.h"

// For finding filenames for files
#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

void initGui(int argc, char **argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480); //Set the window size

	//Create the window
	glutCreateWindow("Kinect Tracker");
	initRendering(); //Initialize rendering

	//Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutPassiveMotionFunc(handleMouseMove);
	glutMotionFunc(handleMouseMove);
	glutMouseFunc(handleMouseButtons);
	glutReshapeFunc(handleResize);
	glutTimerFunc(10, update, 0);

	// Determine all models in directories
	modelPaths = getAllFiles("models/", "*.3ds");

	// Load Icon Textures
	NeHe::imageSampler g_textureSampler;
	g_textureSampler = NeHe::imageSampler(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT);
	editIcon[0] = NeHe::imageTGA("icons/bed.tga",g_textureSampler);
	editIcon[1] = NeHe::imageTGA("icons/storage.tga",g_textureSampler);
	editIcon[2] = NeHe::imageTGA("icons/chair.tga",g_textureSampler);
	editIcon[3] = NeHe::imageTGA("icons/table.tga",g_textureSampler);
	editIcon[4] = NeHe::imageTGA("icons/light.tga",g_textureSampler);
	editIcon[5] = NeHe::imageTGA("icons/tv.tga",g_textureSampler);

	// Start loop
	update(0);
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

// Returns all files (first seven) in the given directory in an array
string** getAllFiles(string path, string searchPattern) {
#define numFolders 7
#define maxFiles 7
	string folders[numFolders] = {"beds/", "storage/", "chairs/", "tables/", "lights/", "electronics/", "misc/"};
	string** files = 0;
	files = new string*[numFolders];

	string filePath;
	for (int i=0; i<numFolders; i++) {
		files[i] = new string[maxFiles];
		int iterator = 0;
		string fullSearchPath = path + folders[i] + searchPattern;
		WIN32_FIND_DATA FindData;
		HANDLE hFind;
		hFind = FindFirstFile( fullSearchPath.c_str(), &FindData );

		if( hFind == INVALID_HANDLE_VALUE ) {
			//cout << "Error searching directory - Most likely and empty directory\n";
			// Set to blank string if occurs - otherwise weird things happen...
		} else {
			do {
				filePath = path + folders[i] + FindData.cFileName;
				files[i][iterator] = filePath;
				iterator++;
			} while( (FindNextFile(hFind, &FindData) > 0) && (iterator < maxFiles) );
		}
	}
	return files;
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
	// Enable Lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

// Main Update Loop
void update(int value) {
	if (USE_KINECT) {
		glBindTexture( GL_TEXTURE_2D, texID );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, K->mColorBuffer );
	}
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
	gluPerspective(43.0,                  //The camera angle
				   (double)viewWidth / (double)viewHeight, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   1000.0);                //The far z clipping coordinate
}

void drawScene() {
//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw 2D Scene in Background
	orthogonalStart (viewWidth, viewHeight);
	// Draw RGB Camera Output in background (in 2D)
	drawColorBackground(viewWidth, viewHeight, texID);
	orthogonalEnd();

	//Draw 3D Scene
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective
	setGlTransformation(); //Transform all points for augmentation
	if (modelHead != NULL) {
		modelHead->drawAugmentation();
	}

	//Draw 2D Overlay
	orthogonalStart(viewWidth, viewHeight);
	// Draw Floor Points
	if (showFloorPoints) {
		drawFloorPoints(floorIJ, numFloorPoints);
	}
	// Draw Wall Points
	if (showWallPoints) {
		drawWallPoints(wallIJ, numWallPoints);
	}
	//Draw the HUD
	if (showHud) {
		drawHud();
	}
	//Draw Warnings
	if (numFloorPoints <= MIN_FLOOR_POINTS) {
		glColor3f(1.0f, 0.0f, 0.0f);
		#ifdef HUD_DEBUG
			orthoPrint(250, viewHeight - 30.0f, "WARNING: CANNOT SEE FLOOR");
		#else
			orthoPrint(250, viewHeight - 7.0f, "WARNING: CANNOT SEE FLOOR");
		#endif
		glColor3f(0.0f, 0.0f, 0.0f);
	}
	if (mapRecord) {
		orthoPrint(200, viewHeight - 60.0f, "RECORDING");
	}
	orthogonalEnd();

	//Send the scene to the screen
	glutSwapBuffers();
}

void orthogonalStart (int viewWidth, int viewHeight) {
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);	// Sets the current stack to push and pop stuff onto
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, viewWidth, 0, viewHeight);	// Sets the viewing region
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

void setGlTransformation() {
	// Apply tranform from depth to rgb sensor - http://nicolas.burrus.name/index.php/Research/KinectCalibration
	GLfloat drbpMatrx[] = { 0.99984628826577793f, -0.0014779096108364480f, 0.017470421412464927f, 0.0f,
						    0.0012635359098409581f, 0.99992385683542895f, 0.012275341476520762f, 0.0f,
						   -0.017487233004436643f, -0.012251380107679535f, 0.99977202419716948f, 0.0f,
						   1.9985242312092553f, -0.074423738761617583f, -1.0916736334336222f, 1.0f}; // Column major form
	//glMultMatrixf(drbpMatrx);

	// Apply pictch / roll matrix
	GLfloat prMatrix[] = { pitchRollMatrix[0], pitchRollMatrix[3], pitchRollMatrix[6], 0.0f,
						   pitchRollMatrix[1], pitchRollMatrix[4], pitchRollMatrix[7], 0.0f,
						   pitchRollMatrix[2], pitchRollMatrix[5], pitchRollMatrix[8], 0.0f,
						   0.0f, 0.0f, 0.0f, 1.0f}; // Column major form
	glMultMatrixf(prMatrix);

	// Apply yMatrix
	GLfloat yMatrix[] = { yawMatrix[0], yawMatrix[3], yawMatrix[6], 0.0f,
						   yawMatrix[1], yawMatrix[4], yawMatrix[7], 0.0f,
						   yawMatrix[2], yawMatrix[5], yawMatrix[8], 0.0f,
						   0.0f, 0.0f, 0.0f, 1.0f}; // Column major form
	glMultMatrixf(yMatrix);

	// Apply Translation Matrix
	GLfloat tsMatrix[] = { 1.0f,					0.0f,					 0.0f,					  0.0f,
						   0.0f,					1.0f,					 0.0f,					  0.0f,
						   0.0f,					0.0f,					 1.0f,					  0.0f,
						   -translationMatrix[0],	-translationMatrix[1],	 -translationMatrix[2],	  1.0f}; // Column major form
	glMultMatrixf(tsMatrix);
}
