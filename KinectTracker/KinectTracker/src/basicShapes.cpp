#include "basicShapes.h"
#include "gui.h"

void drawRectBorder(int x1, int y1, int x2, int y2){
	// Draws the border for a rectangle with the four corners, similar to glRectd
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}