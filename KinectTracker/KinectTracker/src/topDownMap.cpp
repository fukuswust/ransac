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

void TopDownMap::drawLineSeg(LineSeg lineSeg, bool offset) {
	float x1, x2, y1, y2;
	// Get x and y coordinates
	if (lineSeg.isTypeX) {
		x1 = lineSeg.start;
		x2 = lineSeg.stop;
		y1 = y2 = lineSeg.loc;
	} else {
		y1 = lineSeg.start;
		y2 = lineSeg.stop;
		x1 = x2 = lineSeg.loc;
	}

	// Some values need to be offset - Ask John
	if (offset) {
		if (lineSeg.isTypeX) {
			x1 = x1 - xValue;
			x2 = x2 - xValue;
			y1 = y2 = y1 - zValue;
		} else {
			y1 = y1 - zValue;
			y2 = y2 - zValue;
			x1 = x2 = x1 - xValue;
		}
	}

	// Transferring into proper coordinates to plot in hud
	x1 /= (MAX_ALLOWED_DIS/radius);
	x2 /= (MAX_ALLOWED_DIS/radius);
	y1 /= (MAX_ALLOWED_DIS/radius);
	y2 /= (MAX_ALLOWED_DIS/radius);

	// Make sure lines are within bounding box of circle
	if (lineSeg.isTypeX) {
		if ( fabs(y1) >= radius ) { return; }
	} else {
		if ( fabs(x1) >= radius ) { return; }
	}
	
	float radiusSqd = pow(radius, 2.0f);

	// Determine if line intersects the circle.
	if (lineSeg.isTypeX) {
		// Get distance to circle edge from that point.
		float edge = sqrt(radiusSqd - pow(y1, 2));
		// Check if line is outside circle, but within bounding box
		if ( x2 <= -edge ) { return; } 
		if ( x1 >= edge ) { return; }

		// Set point to edge of circle if outside
		if ( x1 <= -edge ) {
			x1 = -edge;
		}
		if ( x2 >= edge ) {
			x2 = edge;
		}
	} else {
		// Get distance to circle edge from that point.
		float edge = sqrt(radiusSqd - pow(x1, 2));
		// Check if line is outside circle, but within bounding box
		if ( y2 <= -edge ) { return; }
		if ( y1 >= edge ) { return; }

		// Set point to edge of circle if outside
		if ( y1 <= -edge ) {
			y1 = -edge;
		}
		if ( y2 >= edge ) {
			y2 = edge;
		}
	}
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
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

	// Make all points relative to center of map
	glPushMatrix();
	glTranslatef(cx,cy,0.0f);

	//Draw Local Top Down Map Background
	if ((mouseIsInside() || mousePressed) && !editMode && !mapRecord) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.9f); // White
	} else {
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // Clear White
	}
	drawCircleSolid(0.0f, 0.0f, radius, 64);
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	drawCircle(0.0f, 0.0f, radius, 64);

	// Rotate if not editing and option chosen
	if (!editMode && tdDisplayTracking) {
		glRotatef(yawValue*180.0f/PI,0.0f,0.0f,1.0f);
	}

	//Draw Local Top Down Map 
	float cam1X = 15.0f*cos(-yawValue - ((50.0f*PI)/180.0f) - (PI/2));
	float cam1Y = 15.0f*sin(-yawValue - ((50.0f*PI)/180.0f) - (PI/2));
	float cam2X = 15.0f*cos(-yawValue + ((50.0f*PI)/180.0f) - (PI/2));
	float cam2Y = 15.0f*sin(-yawValue + ((50.0f*PI)/180.0f) - (PI/2));
	//Draw Camera
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cam1X, cam1Y);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(cam2X, cam2Y);
	glEnd();

	// Draw Top Down Wall
	glPointSize(5.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < drawNumTdWallPts; i++) {
		if (abs(drawTdWall[i].x) != 999999.0) {
			float tmpX = drawTdWall[i].x/(MAX_ALLOWED_DIS/radius);
			float tmpZ = drawTdWall[i].z/(MAX_ALLOWED_DIS/radius);
			glVertex2f(tmpX, tmpZ);
		}
	}
	glEnd();

	// Draw Line Segments
	glLineWidth(3.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
	glBegin(GL_LINES);
	for (int i = 0; i < numLineSegX; i++) {
		drawLineSeg(tdLineSegX[i], false);
	}
	for (int i = 0; i < numLineSegZ; i++) {
		drawLineSeg(tdLineSegZ[i], false);
	}
	glEnd();

	// Draw Map Line Segments
	glLineWidth(3.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < numLineMapX; i++) {
		drawLineSeg(lineMapX[i], true);
	}
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < numLineMapZ; i++) {
		drawLineSeg(lineMapZ[i], true);
	}
	glEnd();
	glLineWidth(1.0f);

	// Draw All Augmentation Top Down Views
	if (modelHead != NULL) {
		modelHead->drawTopDown(cx,cy,radius);
	}

	glPopMatrix(); // Revert to before translation and rotation
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
		if (mouseIsInside() && !mapRecord) {
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
		if (mouseIsInside() && mousePressed) {
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
	// Nothing for now
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