#include "augModel.h"
#include "globals.h"

AugModel::AugModel(char file[], float cScale = -1.0f, float cRot = 0.0f, float cHeight = 0.0f) {
	path = file;
	next = NULL;
	prev = NULL;
	placing = true;
	moving = false;
	m.Load(file);
	autoScaleModel(100.0f, cScale);
	x = 0.0f;
	y = cHeight;
	z = 0.0f;
	tdVisible = false;
	rot = cRot;
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

void AugModel::autoScaleModel(float amount, float cScale) {
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
	float sY = maxY-minY;
	float sZ = maxZ-minZ;
	if (cScale == -1) {
		if (sY/1.5f > sX && sY/1.5f > sZ) {
			scale = amount*1.5f/sY;
		} else if (sX > sZ) {
			scale = amount/sX;
		} else {
			scale = amount/sZ;
		}
	} else {
		scale = cScale;
	}
	minX *= scale;
	maxX *= scale;
	minY *= scale;
	maxY *= scale;
	minZ *= scale;
	maxZ *= scale;
	width = (maxX-minX);
	depth = (maxZ-minZ);
}

void AugModel::scaleMultiplier(float mult) {
	scale *= mult;
	minX *= mult;
	maxX *= mult;
	minY *= mult;
	maxY *= mult;
	minZ *= mult;
	maxZ *= mult;
	width = (maxX-minX);
	depth = (maxZ-minZ);
}

bool AugModel::mouseIsInside() {
	if (tdVisible) {
		return (mouseX-viewXOffset > tdPt1X && mouseX-viewXOffset < tdPt2X && mouseY-viewYOffset > tdPt1Z && mouseY-viewYOffset < tdPt2Z);
	} else {
		return false;
	}
}

void AugModel::drawTopDown(float cx, float cy, float r) {
	// Determine position if placing or moving
	if (placing || moving) {
		x = ((mouseX-cx-viewXOffset)*(MAX_ALLOWED_DIS/r))+xValue;
		z = ((mouseY-cy-viewYOffset)*(MAX_ALLOWED_DIS/r))+zValue;
		editModelHeight = y;
	}

	// Determine if visible
	float pt1X, pt1Z, pt2X, pt2Z;
	if (rot == 0.0f || rot == 180.0f) {
		pt1X = x - xValue - (width/2.0f);
		pt1Z = z - zValue - (depth/2.0f);
		pt2X = x - xValue + (width/2.0f);
		pt2Z = z - zValue + (depth/2.0f);
	} else {
		pt1X = x - xValue - (depth/2.0f);
		pt1Z = z - zValue - (width/2.0f);
		pt2X = x - xValue + (depth/2.0f);
		pt2Z = z - zValue + (width/2.0f);
	}
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
		tdPt1X = cx + (pt1X/(MAX_ALLOWED_DIS/r));
		tdPt1Z = cy + (pt1Z/(MAX_ALLOWED_DIS/r));
		tdPt2X = cx + (pt2X/(MAX_ALLOWED_DIS/r));
		tdPt2Z = cy + (pt2Z/(MAX_ALLOWED_DIS/r));
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);				
		glVertex2f(tdPt1X, tdPt1Z);		
		glVertex2f(tdPt1X, tdPt2Z);		
		glVertex2f(tdPt2X, tdPt2Z);		
		glVertex2f(tdPt2X, tdPt1Z);		
		glEnd();
	}
	if (next != NULL) {
		next->drawTopDown(cx, cy, r);
	}
}

void AugModel::drawAugmentation() {
	if ((!placing && !moving) || (showHud && tdVisible)) {
		glEnable(GL_LIGHTING);
		m.scale = scale;
		m.rot.y = rot;
		m.pos.x = x+minX+(width/2.0f);
		m.pos.y = y-minY;
		m.pos.z = z+minZ+(depth/2.0f);
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
	modelTail->prev = this;
	if (placing) {
		delete this;
	}
}

void AugModel::cancelMovement() {
	if (placing) {
		delete this;
	} else if (moving) {
		moving = false;
		x = prevX;
		z = prevZ;
	} else if (next != NULL) {
		next->cancelMovement();
	}
}

void AugModel::mouseLeftPress() {
	// Goto tail first
	if (!editPlacing && mouseIsInside()) {
		moving = true;
		editPlacing = true;
		prevX = x;
		prevZ = z;
	} else if (placing) { // This model is being placed
		if (tdVisible) {
			placing = false; //Place in the room
			next = new AugModel(path, scale, rot, y);
			next->prev = this;
			modelTail = next;
		}
	} else if (prev != NULL) {
		prev->mouseLeftPress();
	}
}

void AugModel::mouseLeftRelease() {
	if (moving) {
		if (tdVisible) {
			moving = false;
			editPlacing = false;
		} else {
			moving = false;
			editPlacing = false;
			x = prevX;
			z = prevZ;
		}
	} else if (prev != NULL) {
		prev->mouseLeftRelease();
	}
}

void AugModel::mouseRightPress() {
	// Goto tail first
	if (mouseIsInside()) {
		delete this;
	} else if (prev != NULL) {
		prev->mouseRightPress();
	}
}

void AugModel::keyPressW() {
	if (moving || placing) {
		y += EDIT_DEL_HEIGHT;
	} else if (next != NULL) {
		next->keyPressW();
	}
}

void AugModel::keyPressA() {
	if (moving || placing) {
		rot -= 90.0f;
		if (rot < 0.0f) {
			rot += 360.0f;
		}
	} else if (next != NULL) {
		next->keyPressA();
	}
}

void AugModel::keyPressS() {
	if (moving || placing) {
		y -= EDIT_DEL_HEIGHT;
	} else if (next != NULL) {
		next->keyPressS();
	}
}

void AugModel::keyPressD() {
	if (moving || placing) {
		rot += 90.0f;
		if (rot >= 360.0f) {
			rot -= 360.0f;
		}
	} else if (next != NULL) {
		next->keyPressD();
	}
}

void AugModel::keyPressQ() {
	if (moving || placing) {
		scaleMultiplier(1.0f/1.1f);
	} else if (next != NULL) {
		next->keyPressQ();
	}
}

void AugModel::keyPressE() {
	if (moving || placing) {
		scaleMultiplier(1.1f);
	} else if (next != NULL) {
		next->keyPressE();
	}
}