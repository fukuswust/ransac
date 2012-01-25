#include "hud.h"
#include "globals.h"
#include "basicShapes.h"

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
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
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