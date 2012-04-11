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

	// Draw Top Down Lines
	glLineWidth(4.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numTdLines; i++) {
		drawLine(tdLine[i]);
	}
	glEnd();

	// Draw Line Segments
	glLineWidth(3.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numLineSeg; i++) {
		drawLineSeg(tdLineSeg[i]);
	}
	glEnd();

	// Draw Wall Points
	if (showWallPoints) {
		glPointSize(5.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_POINTS);
		int offset =  0;
		for (int i = 0; i < numWallPoints; i++) {
			float tmpX = wallPoints[offset++]/(MAX_ALLOWED_DIS/radius);
			offset++;
			float tmpZ = wallPoints[offset++]/(MAX_ALLOWED_DIS/radius);
			glVertex2f(cx+tmpX, cy+tmpZ);
		}
		glEnd();
	}

	//Draw Augmented Cube
	float x1 = (AUG_CUBE_SIZE*cos(augCubeYaw))/(0.25*MAX_ALLOWED_DIS/radius);
	float z1 = -(AUG_CUBE_SIZE*sin(augCubeYaw))/(0.25*MAX_ALLOWED_DIS/radius);
	float x2 = (AUG_CUBE_SIZE*cos(augCubeYaw+PI/2))/(0.25*MAX_ALLOWED_DIS/radius);
	float z2 = -(AUG_CUBE_SIZE*sin(augCubeYaw+PI/2))/(0.25*MAX_ALLOWED_DIS/radius);
	float acX = augCubeX - 50.0f;
	float acZ = augCubeZ + 50.0f;

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	drawPoint(acX,acZ);
	drawPoint(acX+x1,acZ+z1);
	glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	drawPoint(acX,acZ);
	drawPoint(acX+x2,acZ+z2);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
	drawPoint(acX+x1+x2,acZ+z1+z2);
	drawPoint(acX+x1,acZ+z1);
	drawPoint(acX+x1+x2,acZ+z1+z2);
	drawPoint(acX+x2,acZ+z2);
	drawPoint(0.0f, 0.0f);
	glEnd();
	glLineWidth(1.0f);
}