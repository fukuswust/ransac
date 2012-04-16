#include "topDownMap.h"
#include "globals.h"
#include "basicShapes.h"

bool TopDownMap::drawPoint(float x, float z) {
	float tmpX = x;// - xValue;
	float tmpZ = z;// - zValue;
	float tmpDis = sqrt((tmpX*tmpX)+(tmpZ*tmpZ));
	if (tmpDis < MAX_ALLOWED_DIS) {
		float tmpDir = atan2(tmpZ,tmpX) + yawValue;
		tmpX = (tmpDis*cos(tmpDir))/(MAX_ALLOWED_DIS/radius);
		tmpZ = (tmpDis*sin(tmpDir))/(MAX_ALLOWED_DIS/radius);
		glVertex2f(cx+tmpX, cy+tmpZ);
		return true;
	} else { // Point out of range
		return false;
	}
}

void TopDownMap::drawLineSeg(LineSeg lineSeg) {
	float x1, x2, y1, y2;
	if (lineSeg.isTypeX) {
		x1 = lineSeg.start;
		x2 = lineSeg.stop;
		y1 = y2 = lineSeg.loc;
	} else {
		y1 = lineSeg.start;
		y2 = lineSeg.stop;
		x1 = x2 = lineSeg.loc;
	}
	x1 /= (MAX_ALLOWED_DIS/radius);
	x2 /= (MAX_ALLOWED_DIS/radius);
	y1 /= (MAX_ALLOWED_DIS/radius);
	y2 /= (MAX_ALLOWED_DIS/radius);
	glVertex2f(cx+x1, cy+y1);
	glVertex2f(cx+x2, cy+y2);	
}

void TopDownMap::drawLineSegBounded(LineSeg lineSeg) {
	float x1, x2, y1, y2;
	if (lineSeg.isTypeX) {
		x1 = lineSeg.start;
		x2 = lineSeg.stop;
		y1 = y2 = lineSeg.loc;
	} else {
		y1 = lineSeg.start;
		y2 = lineSeg.stop;
		x1 = x2 = lineSeg.loc;
	}
	x1 /= (MAX_ALLOWED_DIS/radius);
	x2 /= (MAX_ALLOWED_DIS/radius);
	y1 /= (MAX_ALLOWED_DIS/radius);
	y2 /= (MAX_ALLOWED_DIS/radius);

	float dx = x2-x1;
	float dy = y2-y1;
	float dr = sqrt((dx*dx)+(dy*dy));
	float D = (x1*y2)-(x2*y1);
	float sgn = 0;
	float r = radius;
		if (dy < 0) {sgn = -1;} else {sgn = 1;}
	float ci1x = (((D*dy) + (sgn*dx*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	float ci2x = (((D*dy) - (sgn*dx*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	float ci1y = (((-D*dx) + (abs(dy)*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	float ci2y = (((-D*dx) - (abs(dy)*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));

	//glVertex2f(cx+pt1x, cy+pt1y);
	//glVertex2f(cx+pt2x, cy+pt2y);	
}

void TopDownMap::drawLine(Line tdLine) {
	float m = tdLine.m;
	float b = tdLine.b/(MAX_ALLOWED_DIS/radius);
	float x1 = 0;
	float x2 = 1;
	float y1 = b;
	float y2 = m + b;
	float dx = x2-x1;
	float dy = y2-y1;
	float dr = sqrt((dx*dx)+(dy*dy));
	float D = (x1*y2)-(x2*y1);
	float sgn = 0;
	float r = radius;
		if (dy < 0) {sgn = -1;} else {sgn = 1;}
	float pt1x = (((D*dy) + (sgn*dx*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	float pt2x = (((D*dy) - (sgn*dx*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	float pt1y = (((-D*dx) + (abs(dy)*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	float pt2y = (((-D*dx) - (abs(dy)*sqrt((r*r*dr*dr)-(D*D))))/(dr*dr));
	glVertex2f(cx+pt1x, cy+pt1y);
	glVertex2f(cx+pt2x, cy+pt2y);
}

void TopDownMap::draw() {
	//Draw Local Top Down Map Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // White
	drawCircleSolid(cx, cy, radius, 32);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, radius, 32);
	//Draw Local Top Down Map 
	float cam1X = 15.0f*cos(-yawValue - ((50.0f*PI)/180.0f) - (PI/2));
	float cam1Y = 15.0f*sin(-yawValue - ((50.0f*PI)/180.0f) - (PI/2));
	float cam2X = 15.0f*cos(-yawValue + ((50.0f*PI)/180.0f) - (PI/2));
	float cam2Y = 15.0f*sin(-yawValue + ((50.0f*PI)/180.0f) - (PI/2));
	//Draw Camera
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(cx, cy);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cx+cam1X, cy+cam1Y);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cx+cam2X, cy+cam2Y);
	glEnd();

	// Draw Top Down Wall
	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < numTdWallPts; i++) {
		if (abs(tdWall[i].x) != 999999.0) {
			float tmpX = tdWall[i].x/(MAX_ALLOWED_DIS/radius);
			float tmpZ = tdWall[i].z/(MAX_ALLOWED_DIS/radius);
			glVertex2f(cx+tmpX, cy+tmpZ);
		}
	}
	glEnd();

	// Draw Line Segments
	glLineWidth(3.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numLineSegX; i++) {
		drawLineSeg(tdLineSegX[i]);
	}
	for (int i = 0; i < numLineSegZ; i++) {
		drawLineSeg(tdLineSegZ[i]);
	}
	glEnd();

	// Draw Map Line Segments
	glLineWidth(3.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numLineMapX; i++) {
		drawLineSeg(lineMapX[i]);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < numLineMapZ; i++) {
		drawLineSeg(lineMapZ[i]);
	}
	glEnd();

	// Draw Augmentations
	model->setX(augCubeX);
	model->setY(augCubeY);
	model->setZ(augCubeZ);
	model->drawTopDown(cx,cy,radius);

	// Draw small circles with mini models
	drawMiniCircles();
}

void TopDownMap::drawMiniCircles() {
	double theta = 2 * atan2((float)miniRadius, (float)(radius + miniRadius));
	drawMiniCircle(0);
	drawMiniCircle(theta);
	drawMiniCircle(-theta);
	drawMiniCircle(2*theta);
	drawMiniCircle(-2*theta);
	drawMiniCircle(3*theta);
	drawMiniCircle(-3*theta);
}

// Draws a circle at a specified angle around the large circle (0 being at bottom of large circle)
void TopDownMap::drawMiniCircle(float angle) {
	float theta = angle + PI/2;
	int miniX = cx + (radius + miniRadius)*cos(theta);
	int miniY = cy + (radius + miniRadius)*sin(theta);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // White
	drawCircleSolid(miniX, miniY, miniRadius, 20);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(miniX, miniY, miniRadius, 20);
}