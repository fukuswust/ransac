#include "augModel.h"
#include "globals.h"

AugModel::AugModel(char file[]) {
	next = NULL;
	prev = NULL;
	placing = true;
	m.Load(file);
	autoScaleModel(100.0f);
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
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

	float pt1X = cx + ((x - xValue + minX)/(MAX_ALLOWED_DIS/r));
	float pt1Z = cy + ((z - zValue + minZ)/(MAX_ALLOWED_DIS/r));
	float pt2X = cx + ((x - xValue + maxX)/(MAX_ALLOWED_DIS/r));
	float pt2Z = cy + ((z - zValue + maxZ)/(MAX_ALLOWED_DIS/r));
	glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);				
	glVertex2f(pt1X, pt1Z);		
	glVertex2f(pt1X, pt2Z);		
	glVertex2f(pt2X, pt2Z);		
	glVertex2f(pt2X, pt1Z);		
    glEnd();
}

void AugModel::drawAugmentation() {
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

void AugModel::addNewModel(char file[]) {
	modelTail = new AugModel(file);
	if (placing) { // Replace self
		if (prev == NULL) { // Only entry
			modelHead = modelTail;
			delete this;
		} else {
			prev->next = modelTail;
			delete this;
		}
	} else {
		next = modelTail;
		next->prev = this;
	}
}