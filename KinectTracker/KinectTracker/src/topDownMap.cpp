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

	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	// Draw Wall Slice Points
	for (int i = 0; i < NUM_SLICES; i++) {
		float tmpDis = wallSlicePoints[i].dis;
		if (tmpDis != -999999.0) {
			float tmpDir = wallSlicePoints[i].dir;
			float tmpX = (tmpDis*cos(tmpDir))/(MAX_ALLOWED_DIS/radius);
			float tmpZ = (tmpDis*sin(tmpDir))/(MAX_ALLOWED_DIS/radius);
			glVertex2f(cx+tmpX, cy+tmpZ);
		}
	}
	glEnd();

	//Draw Global Map
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_POINTS);
	for (int i=0; i < numGlobalPoints; i++) {
		drawPoint(globalMapPoints[i].x, globalMapPoints[i].z);
	}
	glEnd();

	//Draw Local Map
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i=0; i < NUM_SLICES; i++) {
		float tmpDis = localMapPoints[i].dis;
		if (tmpDis != -999999.0) {
			float tmpDir = localMapPoints[i].dir;
			float tmpX = (tmpDis*cos(tmpDir))/(MAX_ALLOWED_DIS/radius);
			float tmpZ = (tmpDis*sin(tmpDir))/(MAX_ALLOWED_DIS/radius);
			glVertex2f(cx+tmpX, cy+tmpZ);
		}
	}
	glEnd();
	glPointSize(1.0f);

	//Draw Augmented Cube
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	drawPoint(augCubeX,augCubeZ);
	drawPoint(augCubeX+AUG_CUBE_SIZE,augCubeZ);
	glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	drawPoint(augCubeX,augCubeZ);
	drawPoint(augCubeX,augCubeZ+AUG_CUBE_SIZE);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	drawPoint(augCubeX+AUG_CUBE_SIZE,augCubeZ+AUG_CUBE_SIZE);
	drawPoint(augCubeX+AUG_CUBE_SIZE,augCubeZ);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	drawPoint(augCubeX+AUG_CUBE_SIZE,augCubeZ+AUG_CUBE_SIZE);
	drawPoint(augCubeX,augCubeZ+AUG_CUBE_SIZE);
	drawPoint(0.0f, 0.0f);
	glEnd();
	glLineWidth(1.0f);
}