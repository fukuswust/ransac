#include "augModel.h"
#include "globals.h"

AugModel::AugModel(char file[]) {
	path = file;
	next = NULL;
	prev = NULL;
	placing = true;
	m.Load(file);
	autoScaleModel(100.0f);
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

AugModel::~AugModel() {
	if (prev == NULL) {
		if (next == NULL) { //Only one
			modelHead = NULL;
			modelTail = NULL;
		} else { //Some after but none before
			next->prev = NULL;
			modelHead = next;
		}
	} else {
		if (next == NULL) { //Some before but none after
			prev->next = NULL;
			modelTail = prev;
		} else { //Some in both directions
			next->prev = prev;
			prev->next = next;
		}
	}
}

void AugModel::autoScaleModel(float amount) {
	minX = minY = minZ = 999999.0f;
	maxX = maxY = maxZ = -999999.0f; 
	for (int obj = 0; obj < m.numObjects; obj++) {
		for (int v = 0; v < m.Objects[obj].numVerts; v++) {
			float xVal = m.Objects[obj].Vertexes[(v*3)+0];
			if (xVal < minX) {minX = xVal;}
			if (xVal > maxX) {maxX = xVal;}
			float yVal = m.Objects[obj].Vertexes[(v*3)+1];
			if (yVal < minY) {minY = yVal;}
			if (yVal > maxY) {maxY = yVal;}
			float zVal = m.Objects[obj].Vertexes[(v*3)+2];
			if (zVal < minZ) {minZ = zVal;}
			if (zVal > maxZ) {maxZ = zVal;}
		}
	}
	float sX = maxX-minX;
	float sZ = maxZ-minZ;
	if (sX > sZ) {
		scale = amount/sX;
	} else {
		scale = amount/sZ;
	}
	minX *= scale;
	maxX *= scale;
	minY *= scale;
	maxY *= scale;
	minZ *= scale;
	maxZ *= scale;
}

void AugModel::drawTopDown(float cx, float cy, float r) {
	// Determine position if placing or moving
	if (placing) {
		x = (mouseX-cx-viewXOffset)*(MAX_ALLOWED_DIS/r);
		z = (mouseY-cy-viewYOffset)*(MAX_ALLOWED_DIS/r);
	}

	// Determine if visible
	float pt1X = x - xValue + minX;
	float pt1Z = z - zValue + minZ;
	float pt2X = x - xValue + maxX;
	float pt2Z = z - zValue + maxZ;
	if        (sqrt(pow(pt1X,2)+pow(pt1Z,2)) > MAX_ALLOWED_DIS) {
		tdVisible = false;
	} else if (sqrt(pow(pt1X,2)+pow(pt2Z,2)) > MAX_ALLOWED_DIS) {
		tdVisible = false;
	} else if (sqrt(pow(pt2X,2)+pow(pt2Z,2)) > MAX_ALLOWED_DIS) {
		tdVisible = false;
	} else if (sqrt(pow(pt2X,2)+pow(pt1Z,2)) > MAX_ALLOWED_DIS) {
		tdVisible = false;
	} else {
		tdVisible = true;
	}

	// Draw Top Down View
	if (tdVisible) {
		float pt1Xtd = cx + (pt1X/(MAX_ALLOWED_DIS/r));
		float pt1Ztd = cy + (pt1Z/(MAX_ALLOWED_DIS/r));
		float pt2Xtd = cx + (pt2X/(MAX_ALLOWED_DIS/r));
		float pt2Ztd = cy + (pt2Z/(MAX_ALLOWED_DIS/r));
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);				
		glVertex2f(pt1Xtd, pt1Ztd);		
		glVertex2f(pt1Xtd, pt2Ztd);		
		glVertex2f(pt2Xtd, pt2Ztd);		
		glVertex2f(pt2Xtd, pt1Ztd);		
		glEnd();
	}
	if (next != NULL) {
		next->drawTopDown(cx, cy, r);
	}
}

void AugModel::drawAugmentation() {
	if (!placing || (showHud && tdVisible)) {
		glEnable(GL_LIGHTING);
		m.scale = scale;
		m.pos.x = x;
		m.pos.y = y-minY;
		m.pos.z = z;
		m.Draw();
		//glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D); // Disable texturing so the colors don't bleed into the osd
	}
	if (next != NULL) {
		next->drawAugmentation();
	}
}

void AugModel::addNewModel(char file[]) {
	next = new AugModel(file);
	modelTail = next;
	if (placing) {
		delete this;
	}
}

void AugModel::cancelMovement() {
	if (next == NULL) {
		if (placing) {
			delete this;
		}
	} else {
		next->cancelMovement();
	}
}

void AugModel::mouseLeftPress() {
	// Goto tail first
	if (next != NULL) {
		next->mouseLeftPress();
	}
	if (!editPlacing) {

	} else if (placing) { // This model is being placed
		if (tdVisible) {
			placing = false; //Place in the room
			next = new AugModel(path);
			next->prev = this;
			modelTail = next;
		}
	}
}