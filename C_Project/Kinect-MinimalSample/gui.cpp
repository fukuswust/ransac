#include "gui.h"

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
	glColor3f(1.0f, 1.0f, 1.0f); // White
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
	glColor3f(1.0f, 1.0f, 1.0f); // White
	drawCircleSolid(cx, cy, r, 16);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 16);
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	drawCenteredTiltedLine(cx, cy, r-6, roll);
	// Draw hash marks
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleHash(cx, cy, r, 2, 8);
}

void drawPitchHud(int cx, int cy, int r, float pitch)
{
	glColor3f(1.0f, 1.0f, 1.0f); // White
	drawCircleSolid(cx, cy, r, 16);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, r, 16);
	drawCenteredTiltedLine(cx, cy, r-6, pitch);
	// Draw hash marks
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircleHash(cx, cy, r, 2, 8);
}