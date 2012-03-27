#include "topDownMap.h"
#include "globals.h"
#include "basicShapes.h"

bool TopDownMap::drawPoint(float x, float z) {
	float tmpX = x - xValue;
	float tmpZ = z - zValue;
	float tmpDis = sqrt((tmpX*tmpX)+(tmpZ*tmpZ));
	if (tmpDis < MAX_ALLOWED_DIS) {
		float tmpDir = atan2(tmpZ,tmpX) - yawValue;
		tmpX = (tmpDis*cos(tmpDir))/(MAX_ALLOWED_DIS/radius);
		tmpZ = (tmpDis*sin(tmpDir))/(MAX_ALLOWED_DIS/radius);
		glVertex2f(cx+tmpX, cy+tmpZ);
		return true;
	} else { // Point out of range
		return false;
	}
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

	// Draw Wall Slice Points
	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < NUM_SLICES; i++) {
		float tmpDis = wallSlicePoints[i].dis;
		float tmpDir = wallSlicePoints[i].dir;
		if (wallSlicePoints[i].dis != -999999.0) {
			float tmpX = (tmpDis*cos(tmpDir))/(MAX_ALLOWED_DIS/radius);
			float tmpZ = (tmpDis*sin(tmpDir))/(MAX_ALLOWED_DIS/radius);
			glVertex2f(cx+tmpX, cy+tmpZ);
		}
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
	float z1 = (AUG_CUBE_SIZE*sin(augCubeYaw))/(0.25*MAX_ALLOWED_DIS/radius);
	float x2 = (AUG_CUBE_SIZE*cos(augCubeYaw+PI/2))/(0.25*MAX_ALLOWED_DIS/radius);
	float z2 = (AUG_CUBE_SIZE*sin(augCubeYaw+PI/2))/(0.25*MAX_ALLOWED_DIS/radius);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	drawPoint(augCubeX,augCubeZ);
	drawPoint(augCubeX+x1,augCubeZ+z1);
	glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	drawPoint(augCubeX,augCubeZ);
	drawPoint(augCubeX+x2,augCubeZ+z2);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
	drawPoint(augCubeX+x1+x2,augCubeZ+z1+z2);
	drawPoint(augCubeX+x1,augCubeZ+z1);
	drawPoint(augCubeX+x1+x2,augCubeZ+z1+z2);
	drawPoint(augCubeX+x2,augCubeZ+z2);
	drawPoint(0.0f, 0.0f);
	glEnd();
	glLineWidth(1.0f);
}