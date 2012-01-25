#include "augment.h"
#include "globals.h"
#include "basicShapes.h"

void drawColorBackground(int viewWidth, int viewHeight, GLuint texID){
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

void drawHeightLine(float heightSlices[], int heightSliceIJ[], int numSlices) {
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (int i=0, ij=0; i < numSlices*2; i+=2) {
		if (heightSlices[i] != 999999.0f) {
			int tmpI = heightSliceIJ[ij++]*xViewFactor;
			int tmpJ = heightSliceIJ[ij++]*yViewFactor;
			glVertex2f(tmpI, tmpJ);
		} else {
			ij+=2;
		}
	}
	glEnd();
	glLineWidth(1.0f);
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

	float fi = ((( fx - 1.8f) / 0.0023f)/ (-fz - 10)) + 320.0f - 1.0f;
	float fj = (((-fy - 2.4f) / 0.0023f)/ (-fz - 10)) + 240.0f - 1.0f;

	glVertex2f(fi*xViewFactor, fj*yViewFactor);
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

void drawAugmentedCube(float x, float y, float z, float s) {
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x+s, y, z);
	glColor3f(0.0f, 1.0f, 0.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x, y+s, z);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawAugmentedPoint(x, y, z);
	drawAugmentedPoint(x, y, z+s);

	glColor3f(0.0f, 0.0f, 0.0f);
	drawAugmentedPoint(x+s, y+s, z+s);
	drawAugmentedPoint(x, y+s, z+s);
	drawAugmentedPoint(x+s, y+s, z+s);
	drawAugmentedPoint(x+s, y, z+s);
	drawAugmentedPoint(x+s, y+s, z+s);
	drawAugmentedPoint(x+s, y+s, z);

	drawAugmentedPoint(x+s, y, z);
	drawAugmentedPoint(x+s, y+s, z);
	drawAugmentedPoint(x+s, y, z);
	drawAugmentedPoint(x+s, y, z+s);

	drawAugmentedPoint(x, y+s, z);
	drawAugmentedPoint(x+s, y+s, z);
	drawAugmentedPoint(z, y+s, z);
	drawAugmentedPoint(x, y+s, z+s);

	drawAugmentedPoint(x, y, z+s);
	drawAugmentedPoint(x+s, y, z+s);
	drawAugmentedPoint(x, y, z+s);
	drawAugmentedPoint(x, y+s, z+s);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}

void drawAugmentedCorners() {
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numCorners; i++) {
		float tmpX = wallCorners[(i*6)+0];
		float tmpZ = wallCorners[(i*6)+1];
		float cornerType = wallCorners[(i*6)+2];
		if (cornerType == 1) { //false
			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		} else {
			glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		}
		drawTopDownViewPoint(tmpX,0,tmpZ);
		drawTopDownViewPoint(tmpX,300,tmpZ);
	}
	glEnd();

	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < numCorners; i++) {
		float tmpX = wallCorners[(i*6)+0];
		float tmpZ = wallCorners[(i*6)+1];
		float cornerConnectivity = wallCorners[(i*6)+3];
		if (cornerConnectivity == 0) { //Not Connected
			glEnd();
			glBegin(GL_LINE_STRIP);
			drawTopDownViewPoint(tmpX,150.0f,tmpZ);
		} else {
			drawTopDownViewPoint(tmpX,150.0f,tmpZ);
		}		
	}
	glEnd();
	glLineWidth(1.0f);
}