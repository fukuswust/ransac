#include "gui.h"
#include <string.h>
#include "globals.h"

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

void drawRectBorder(int x1, int y1, int x2, int y2){
	// Draws the border for a rectangle with the four corners, similar to glRectd
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
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

void drawTopDownMap(int cx, int cy, int r, float heightSlices[], int numSlices){
	//Draw Local Top Down Map Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // White
	drawCircleSolid(cx, cy, r, 32);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 32);
	//Draw Local Top Down Map 
	float delX = 15.0f*cos((50.0f*PI)/180.0f);
	float delY = 15.0f*sin((50.0f*PI)/180.0f);
	//Draw Origin
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(cx, cy);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cx+delX, cy-delY);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cx-delX, cy-delY);
	glEnd();
	//Draw Slice
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	float lastDis = 999999.0;
	float lastAccel = 999999.0;
	float prevError = 999999.0f;
	float prevDelDis = 999999.0;
	for (int i=0, iStat=0; i < numSlices*2; iStat++) {
		char printBuff[128];
		float tmpX = heightSlices[i++]/(MAX_ALLOWED_DIS/r);
		float tmpZ = heightSlices[i++]/(MAX_ALLOWED_DIS/r);
		float dis = sqrt((tmpX*tmpX)+(tmpZ*tmpZ));
		float dir = atan2(tmpZ,tmpX);
		if (wallStatus[iStat] == 1) { //False Corner
			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		} else if (wallStatus[iStat] == 2) { //True Corner
			glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		} else { //Wall
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
		}
		//glVertex2f(cx+tmpX, cy+tmpZ);
	}
	// Draw Local Wall Corner Points
	glPointSize(5.0f);
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