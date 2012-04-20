#include "topDownMap.h"
#include "globals.h"
#include "basicShapes.h"
#include "topDownButton.h"

TopDownMap::TopDownMap() {
	editMode = false;
	btnRadius = HUD_MAP_BTN_RADIUS;
	for (int i = 0; i < 8; i++) {
		topDownButton[i].setRadius(btnRadius);
		topDownButton[i].setId(i);
		topDownButton[i].setLevelOn(0);
	}
}

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
	// Get x and y coordinates
	if (lineSeg.isTypeX) {
		if (lineSeg.start > lineSeg.stop) { // Switch start and stop so lower value is to the left.
			x1 = lineSeg.stop;
			x2 = lineSeg.start;
		} else {
			x1 = lineSeg.start;
			x2 = lineSeg.stop;
		}
		y1 = y2 = lineSeg.loc;
	} else {
		if (lineSeg.start > lineSeg.stop) { // Switch so lower value is on bottom.
			y1 = lineSeg.stop;
			y2 = lineSeg.start;
		} else {
			y1 = lineSeg.start;
			y2 = lineSeg.stop;
		}
		x1 = x2 = lineSeg.loc;
	}
	// Transferring into proper coordinates to plot in hud
	x1 /= (MAX_ALLOWED_DIS/radius);
	x2 /= (MAX_ALLOWED_DIS/radius);
	y1 /= (MAX_ALLOWED_DIS/radius);
	y2 /= (MAX_ALLOWED_DIS/radius);

	x1 = x1 + cx;
	x2 = x2 + cx;
	y1 = y1 + cy;
	y2 = y2 + cy;

	// Make sure lines are within circle
	if (lineSeg.isTypeX) {
		if ( fabs(y2-y1) >= radius ) { return; }
	} else {
		if ( fabs(x2-x1) >= radius ) { return; }
	}
	
	float radiusSqd = pow(radius, 2.0f);

	// Determine if line intersects the circle.
	if (lineSeg.isTypeX) {
		// Get distance to circle edge from that point.
		float distC = pow(y1-float(cy), 2);
		float mod = sqrt(radiusSqd - distC);
		float xLeft = cx - mod;
		float xRight = cx + mod;
		if ( x1 <= xLeft ) { // Draw on circle if outside
			x1 = xLeft;
		}
		if ( x2 >= xRight ) { // Draw on circle if outside
			x2 = xRight;
		}
	} else {
		float distC = pow(x1-float(cx), 2);
		float mod = sqrt(radiusSqd - distC);
		float yBot = cy - mod;
		float yTop = cy + mod;
		if ( y1 <= yBot ) { // Draw on circle if outside
			y1 = yBot;
		}
		if ( y2 >= yTop ) { // Draw on circle if outside
			y2 = yTop;
		}
	}
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
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
	//Set Location
	if (editMode) {
		cx = viewWidth/2;
		cy = viewHeight/2;
		if (viewWidth > viewHeight) {
			radius = (viewHeight/2)-(2*btnRadius)-30;
		} else {
			radius = (viewWidth/2)-(2*btnRadius)-30;
		}
		btnRadius = HUD_MAP_BTN_RADIUS;
	} else {
		cx = viewWidth-HUD_MAP_X;
		cy = HUD_MAP_Y;
		radius = HUD_MAP_RADIUS;
		btnRadius = HUD_MAP_BTN_RADIUS;
	}

	//Draw Local Top Down Map Background
	if ((mouseIsInside() || mousePressed) && !editMode) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.9f); // White
	} else {
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // Clear White
	}
	drawCircleSolid(cx, cy, radius, 64);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(cx, cy, radius, 64);
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

	// MANUALLY ADDED LINE FOR DEBUGGING ////
	//numLineSegX = 1;
	//tdLineSegX[0].isTypeX = true;
	//tdLineSegX[0].loc = 200.0f;
	//tdLineSegX[0].start = -10.0f;
	//tdLineSegX[0].stop = 10090.0f;
	for (int i = 0; i < numLineSegX; i++) {
		drawLineSegBounded(tdLineSegX[i]);
	}
	for (int i = 0; i < numLineSegZ; i++) {
		drawLineSegBounded(tdLineSegZ[i]);
	}
	glEnd();

	// Draw Map Line Segments
	glLineWidth(3.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numLineMapX; i++) {
		drawLineSegBounded(lineMapX[i]);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < numLineMapZ; i++) {
		drawLineSegBounded(lineMapZ[i]);
	}
	glEnd();
	glLineWidth(1.0f);

	// Draw All Augmentation Top Down Views
	if (modelHead != NULL) {
		modelHead->drawTopDown(cx,cy,radius);
	}

	// Set Button Locations and Draw
	if (editMode) {
		float dTheta = 2 * atan2((float)btnRadius, (float)(radius + btnRadius));
		for (int i = 0; i < 8; i++) {
			float theta = (i-3.5f)*dTheta;
			topDownButton[i].setX(cx + (radius + btnRadius)*cos(theta));
			topDownButton[i].setY(cy + (radius + btnRadius)*sin(theta));
			topDownButton[i].draw();
		}	
	}
}

bool TopDownMap::mouseIsInside() {
	return (sqrt(pow((float)cx-mouseX+viewXOffset,2)+pow((float)cy-mouseY+viewYOffset,2))<radius);
}

void TopDownMap::mouseLeftPress() {
	if (!editMode) {
		if (mouseIsInside()) {
			mousePressed = true;
		}
	} else {
		for (int i = 0; i < 8; i++) {
			topDownButton[i].mouseLeftPress();
		}
		if (modelTail != NULL) {
			modelTail->mouseLeftPress();
		}
	}
}

void TopDownMap::mouseLeftRelease() {
	if (!editMode) {
		if (mouseIsInside()) {
			editMode = true;
			editPlacing = false;
		}
	} else {
		for (int i = 0; i < 8; i++) {
			topDownButton[i].mouseLeftRelease();
		}
		if (modelTail != NULL) {
			modelTail->mouseLeftRelease();
		}
	}
	mousePressed = false;
}

void TopDownMap::mouseRightPress() {
	if (editPlacing) {
		topDownMap.gotoLevel(levelOn);
		modelHead->cancelMovement();
		editPlacing = false;
	} else {
		modelTail->mouseRightPress();
	}
}

void TopDownMap::mouseRightRelease() {

}

void TopDownMap::gotoLevel(int level) {
	for (int i = 0; i < 8; i++) {
		if (level == -1) {
			topDownButton[i].setLevelOn(0);
			editMode = false;
			levelOn = level;
		} else {
			topDownButton[i].setLevelOn(level);
			levelOn = level;
		}
	}
}

void TopDownMap::setSelected(int value) {
	for (int i = 0; i < 8; i++) {
		topDownButton[i].setLevelOn(levelOn);
		selected = value;
	}
}