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
	orthogonalStart (viewWidth, viewHeight);
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
}

void drawCrosshair(int viewWidth, int viewHeight){
	orthogonalStart(viewWidth, viewHeight);
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
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f); // White
	drawCircleSolid(cx, cy, r, 16);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 16);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	drawCenteredTiltedLine(cx, cy, r-6, roll);
	// Draw hash marks
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleHash(cx, cy, r, 2, 8);
	// Draw center point
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleSolid(cx, cy, 3.5, 8);
}

void drawPitchHud(int cx, int cy, int r, float pitch)
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f); // White
	drawCircleSolid(cx, cy, r, 16);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 16);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	drawCenteredTiltedLine(cx, cy, r-6, pitch);
	// Draw hash marks
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleHash(cx, cy, r, 2, 8);
	// Draw center point
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleSolid(cx, cy, 3.5, 8);
}

void drawTopDownMap(int cx, int cy, int r, float heightSlices[], float heightSliceColors[], int numSlices){
	//Draw Local Top Down Map Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // White
	drawCircleSolid(cx, cy, r, 32);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 32);
	//Draw Local Top Down Map 
	float delX = 15.0f*cos((30.0f*PI)/180.0f);
	float delY = 15.0f*sin((30.0f*PI)/180.0f);
	//Draw Origin
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(cx, cy);
	glVertex2f(cx+delX, cy-delY);
	glVertex2f(cx, cy);
	glVertex2f(cx-delX, cy-delY);
	glEnd();
	//Draw Slice
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	for (int i=0, iIm=0; i < numSlices*2; ) {
		if (heightSlices[i] != 999999.0f) {
			float tmpX = heightSlices[i++]/8.0f;
			float tmpY = heightSlices[i++]/8.0f;
			if (sqrt((tmpX*tmpX)+(tmpY*tmpY)) < r) {
				float tmpColor = heightSliceColors[iIm++];
				if (tmpColor == 999999.0f) {
					glColor3f(1.0f, 0.0f, 0.0f);
				} else {
					glColor3f(tmpColor, tmpColor, tmpColor);
				}
				glVertex2f(cx+tmpX, cy+tmpY);
			}
		} else {
			i+=2;
		}
	}
	glEnd();
	glPointSize(1.0f);
}