#include "hud.h"
#include "globals.h"
#include "basicShapes.h"

void drawHud() {
	char printBuff[256];

	// Draw Crosshair (in 2D)
	drawCrosshair(viewWidth, viewHeight);

	//Draw Frame Count (in 2D)
	glColor3f(1.0f, 1.0f, 1.0f);
	fpsStopWatch->stopTimer();
	avgFrameTime = (0.1*(float)(fpsStopWatch->getElapsedTime()))+(0.9f*avgFrameTime);
	sprintf(printBuff, "FPS: %u", (unsigned int)(1.0f/avgFrameTime));
	orthoPrint(HUD_FPS_X, viewHeight - HUD_FPS_Y, printBuff);
	fpsStopWatch->startTimer();

	#ifdef HUD_DEBUG
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
	#endif

	// Draw Height Measurement Bar
	drawHeightHud(HUD_HEIGHT_BAR_X, HUD_HEIGHT_BAR_Y, heightValue);

	// Draw Roll
	drawRollHud(HUD_ROLL_X, HUD_ROLL_Y, HUD_ROLL_RADIUS, rollValue);
	
	// Draw Pitch
	drawPitchHud(HUD_PITCH_X, HUD_PITCH_Y, HUD_PITCH_RADIUS, pitchValue);

	// Draw Top Down Map
	topDownMap.setX(viewWidth-HUD_MAP_X);
	topDownMap.setY(HUD_MAP_Y);
	topDownMap.setRadius(HUD_MAP_RADIUS);
	topDownMap.draw();
}

void drawHeightHud(int topx, int topy, float height) {
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

void drawRollHud(int cx, int cy, int r, float roll) {
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

void drawPitchHud(int cx, int cy, int r, float pitch) {
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