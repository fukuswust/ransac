#include "hud.h"
#include "globals.h"
#include "basicShapes.h"

void drawHud() {
	char printBuff[256];

	// Draw Crosshair (in 2D)
	drawCrosshair(viewWidth, viewHeight);

	//Draw Frame Count (in 2D)
	drawFps(HUD_FPS_X, viewHeight - HUD_FPS_Y);

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

	// Draw helper window
	if (showHelp) {
		drawKeyboardHelp(10, viewHeight-30);
	}

	// Draw Top Down Map
	topDownMap.draw();

	if (editPlacing) {
		drawPlacingHelp(viewWidth-10,10);
		drawModelInfo(viewWidth-10, viewHeight-30);
	}
}

void drawPlacingHelp(int topRx, int topRy) {
	int dPHHeight = 70;
	int dPHWidth = 100;
	// Draw Big White Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f); // White
	glRectf(topRx-dPHWidth, topRy, topRx, topRy+dPHHeight);
	// Draw Big White Background Border
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawRectBorder(topRx-dPHWidth, topRy, topRx, topRy+dPHHeight);
	
	// Header
	orthoPrint(topRx-dPHWidth+7, topRy+15, "-     Keys     +");
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(topRx-dPHWidth, topRy+20);
	glVertex2f(topRx, topRy+20);
	glEnd();

	// Text
	orthoPrint(topRx-dPHWidth+7, topRy+34, "Q    Size       E");
	orthoPrint(topRx-dPHWidth+7, topRy+49, "S    Height    W");
	orthoPrint(topRx-dPHWidth+7, topRy+64, "A    Rotation  D");
}

void drawKeyboardHelp(int botLx, int botLy) {
	int dKHeight = 160;
	int dKWidth = 210;
	int col1X = botLx + 7;
	int col2X = botLx + 60;
	// Draw Big White Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f); // White
	glRectf(botLx, botLy-dKHeight, botLx+dKWidth, botLy);
	// Draw Big White Background Border
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawRectBorder(botLx, botLy-dKHeight, botLx+dKWidth, botLy);
	
	// Header
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(botLx, botLy-dKHeight+20);
	glVertex2f(botLx+dKWidth, botLy-dKHeight+20);
	glEnd();

	// Text - Column 1
	orthoPrint(col1X, botLy-dKHeight+15, "Key");
	orthoPrint(col1X, botLy-dKHeight+34, "ESC");
	orthoPrint(col1X, botLy-dKHeight+49, "SPACE");
	orthoPrint(col1X, botLy-dKHeight+64, "1");
	orthoPrint(col1X, botLy-dKHeight+79, "2");
	orthoPrint(col1X, botLy-dKHeight+94, "F");
	orthoPrint(col1X, botLy-dKHeight+109, "H");
	orthoPrint(col1X, botLy-dKHeight+124, "M");
	orthoPrint(col1X, botLy-dKHeight+139, "O");
	orthoPrint(col1X, botLy-dKHeight+154, "T");

	// Text - Column 2
	orthoPrint(col2X, botLy-dKHeight+15, "Action");
	orthoPrint(col2X, botLy-dKHeight+34, "Close Program");
	orthoPrint(col2X, botLy-dKHeight+49, "Toggle Recording");
	orthoPrint(col2X, botLy-dKHeight+64, "Toggle Show Floor Points");
	orthoPrint(col2X, botLy-dKHeight+79, "Toggle Show Wall Points");
	orthoPrint(col2X, botLy-dKHeight+94, "Toggle Fullscreen");
	orthoPrint(col2X, botLy-dKHeight+109, "Toggle Help");
	orthoPrint(col2X, botLy-dKHeight+124, "Level Motor Position");
	orthoPrint(col2X, botLy-dKHeight+139, "Toggle Overhead Display");
	orthoPrint(col2X, botLy-dKHeight+154, "Toggle Top Down Mode");
}

void drawModelInfo(int botRx, int botRy) {
	int dMIHeight = 50;
	int dMIWidth = 100;
	int col1X = botRx - dMIWidth + 7;
	int col2X = botRx - dMIWidth + 55;

	// Draw Big White Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f); // White
	glRectf(botRx-dMIWidth, botRy-dMIHeight, botRx, botRy);
	// Draw Big White Background Border
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawRectBorder(botRx-dMIWidth, botRy-dMIHeight, botRx, botRy);
	char buffer[20];
	orthoPrint(col1X, botRy-dMIHeight+15, "Height:");
	sprintf(buffer, "%icm", int(editModelHeight));
	orthoPrint(col2X, botRy-dMIHeight+15, buffer);
	
	orthoPrint(col1X, botRy-dMIHeight+30, "Width:");
	sprintf(buffer, "%icm", int(editModelWidth));
	orthoPrint(col2X, botRy-dMIHeight+30, buffer);

	orthoPrint(col1X, botRy-dMIHeight+45, "Length:");
	sprintf(buffer, "%icm", int(editModelLength));
	orthoPrint(col2X, botRy-dMIHeight+45, buffer);
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

void drawFps(int cx, int cy) {
	static float avgFrameTime = 0.0f;
	static CStopWatch fpsStopWatch;
	char printBuff[256];
	glColor3f(1.0f, 1.0f, 1.0f);
	if (avgFrameTime == 0.0f) {
		fpsStopWatch.startTimer();
	}
	fpsStopWatch.stopTimer();
	avgFrameTime = (0.1*(float)(fpsStopWatch.getElapsedTime()))+(0.9f*avgFrameTime);
	sprintf(printBuff, "FPS: %u", (unsigned int)(1.0f/avgFrameTime));
	orthoPrint(cx, cy, printBuff);
	fpsStopWatch.startTimer();
}