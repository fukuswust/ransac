#include "augment.h"
#include "globals.h"
#include "basicShapes.h"

void drawColorBackground(int viewWidth, int viewHeight, GLuint texID){
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
}

void drawFloorPoints(int floorIJ[], int numPoints) {
	glColor3f(0.0f, 1.0f, 0.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (int ij=0; ij < numPoints*2; ) {
		int tmpI = floorIJ[ij++]*xViewFactor;
		int tmpJ = floorIJ[ij++]*yViewFactor;
		glVertex2f(tmpI, tmpJ);
	}
	glEnd();
	glLineWidth(1.0f);
}

void drawWallPoints(int wallIJ[], int numPoints) {
	glColor3f(0.0f, 0.0f, 1.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (int ij=0; ij < numPoints*2; ) {
		int tmpI = wallIJ[ij++]*xViewFactor;
		int tmpJ = wallIJ[ij++]*yViewFactor;
		glVertex2f(tmpI, tmpJ);
	}
	glEnd();
	glLineWidth(1.0f);
}

void drawAugmentedPoint(float x, float y, float z) {
	// Get X,Y,Z Coordinates
	float transX = translationMatrix[0] - x;
	float transY = translationMatrix[1] - y;
	float transZ = translationMatrix[2] - z;

	// Apply Yaw Rotation
	float yawTmpX = (yawMatrix[0]*transX) + (yawMatrix[1]*transY) + (yawMatrix[2]*transZ);
	float yawTmpY = (yawMatrix[3]*transX) + (yawMatrix[4]*transY) + (yawMatrix[5]*transZ);
	float yawTmpZ = (yawMatrix[6]*transX) + (yawMatrix[7]*transY) + (yawMatrix[8]*transZ);

	// Apply Pitch and Roll
	float fx = (pitchRollMatrix[0]*yawTmpX) + (pitchRollMatrix[1]*yawTmpY) + (pitchRollMatrix[2]*yawTmpZ);
	float fy = (pitchRollMatrix[3]*yawTmpX) + (pitchRollMatrix[4]*yawTmpY) + (pitchRollMatrix[5]*yawTmpZ);
	float fz = (pitchRollMatrix[6]*yawTmpX) + (pitchRollMatrix[7]*yawTmpY) + (pitchRollMatrix[8]*yawTmpZ);

	if (fz >= 0) {
		float fi = ((( fx - 1.8f) / 0.0023f)/ (-fz - 10)) + 320.0f - 1.0f;
		float fj = (((-fy - 2.4f) / 0.0023f)/ (-fz - 10)) + 240.0f - 1.0f;

		glVertex2f(fi*xViewFactor, fj*yViewFactor);
	}
}

void drawTopDownViewPoint(float x, float y, float z) {
	// Get X,Y,Z Coordinates
	float transX = -x;
	float transY = translationMatrix[1] - y;
	float transZ = -z;

	// Apply Pitch and Roll
	float fx = (pitchRollMatrix[0]*transX) + (pitchRollMatrix[1]*transY) + (pitchRollMatrix[2]*transZ);
	float fy = (pitchRollMatrix[3]*transX) + (pitchRollMatrix[4]*transY) + (pitchRollMatrix[5]*transZ);
	float fz = (pitchRollMatrix[6]*transX) + (pitchRollMatrix[7]*transY) + (pitchRollMatrix[8]*transZ);

	float fi = ((( fx - 1.8f) / 0.0023f)/ (-fz - 10)) + 320.0f - 1.0f;
	float fj = (((-fy - 2.4f) / 0.0023f)/ (-fz - 10)) + 240.0f - 1.0f;

	glVertex2f(fi*xViewFactor, fj*yViewFactor);
}

void drawAugmentedCube(float x, float y, float z, float yaw, float s) {
	glLineWidth(4.0f);
	glBegin(GL_LINES);

	float x1 = s*cos(yaw);
	float z1 = s*sin(yaw);
	float x2 = s*cos(yaw+PI/2);
	float z2 = s*sin(yaw+PI/2);

	glColor3f(0.0f, 0.0f, 1.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x+x1, y, z+z1);
	glColor3f(0.0f, 1.0f, 0.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x, y+s, z);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x+x2, y, z+z2);

	glColor3f(0.2f, 0.2f, 0.2f);
	drawAugmentedPoint(x+x1+x2, y+s, z+z1+z2);
	drawAugmentedPoint(x+x2,   y+s, z+z2);
	drawAugmentedPoint(x+x1+x2, y+s, z+z1+z2);
	drawAugmentedPoint(x+x1+x2, y, z+z1+z2);
	drawAugmentedPoint(x+x1+x2, y+s, z+z1+z2);
	drawAugmentedPoint(x+x1, y+s, z+z1);
	
	drawAugmentedPoint(x+x1, y,   z+z1);
	drawAugmentedPoint(x+x1, y+s, z+z1);
	drawAugmentedPoint(x+x1, y,   z+z1);
	drawAugmentedPoint(x+x1+x2, y,  z+z1+z2);
	
	drawAugmentedPoint(x,   y+s, z);
	drawAugmentedPoint(x+x1, y+s, z+z1);
	drawAugmentedPoint(x,   y+s, z);
	drawAugmentedPoint(x+x2,   y+s, z+z2);
	
	drawAugmentedPoint(x+x2,   y, z+z2);
	drawAugmentedPoint(x+x1+x2, y, z+z1+z2);
	drawAugmentedPoint(x+x2,   y, z+z2);
	drawAugmentedPoint(x+x2,   y+s, z+z2);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}
