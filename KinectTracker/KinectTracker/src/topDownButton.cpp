#include "topDownButton.h"
#include "globals.h"
#include "basicShapes.h"
#include "augModel.h"

TopDownButton::TopDownButton() {
	state = 0;
}

void TopDownButton::draw() {
	if (levelOn == 0) {
		visible = true;
	} else {
		if (id != 0) {
			visible = (modelPaths[levelOn-1][id-1] != "");
		} else {
			visible = true;
		}
	}

	if (visible) {
		if (id!=0) {
			if (levelOn == 0) {
				col[0] = btnColors[id][0];
				col[1] = btnColors[id][1];
				col[2] = btnColors[id][2];
			} else {
				col[0] = btnColors[levelOn][0];
				col[1] = btnColors[levelOn][1];
				col[2] = btnColors[levelOn][2];
			}
		} else {
			col[0] = btnColors[0][0];
			col[1] = btnColors[0][1];
			col[2] = btnColors[0][2];
		}

		float alpha;
		if (state == 2) {
			alpha = 0.95f;
		} else if (mouseIsInside() || state == 1) {
			alpha = 0.8f;
		} else {
			alpha = 0.5f;
		}

		glColor4f(col[0], col[1], col[2], alpha);
		drawCircleSolid(x, y, radius, 20);

		glColor3f(0.0f, 0.0f, 0.0f); // Black
		drawCircle(x, y, radius, 20);

		// Draw Number/Icon in middle
		if (id == 0) { // X
			orthoPrintLarge(x-8,y+7,"X");
		} else if (levelOn == 0) {
			if (id == 7) {
				orthoPrintLarge(x-5,y+8,"?");
			} else {// Icon
				float diag = radius/sqrt(2.0f);
				glColor3f(col[0], col[1], col[2]);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,editIcon[id-1].getId());
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x-diag, y+diag);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x-diag, y-diag);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x+diag, y-diag);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x+diag, y+diag);
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		} else { // Number
			char tmp[2];
			sprintf(tmp,"%i",id);
			orthoPrintLarge(x-4,y+7,tmp);
		}
	}
}

bool TopDownButton::mouseIsInside() {
	return (sqrt(pow((float)x-mouseX+viewXOffset,2)+pow((float)y-mouseY+viewYOffset,2))<radius && visible);
}

void TopDownButton::mouseLeftPress() {
	if (mouseIsInside()) {
		if (state == 0) {
			state = 1;
		}
	}
}

void TopDownButton::mouseLeftRelease() {
	if (state == 1) {
		if (mouseIsInside()) {
			if (id == 0) { // Is 'x'
				if (levelOn == 0) {
					topDownMap.gotoLevel(-1);
				} else {
					if (modelHead != NULL) {
						editPlacing = false;
						modelHead->cancelMovement();
					}
					topDownMap.gotoLevel(0);
				}
			} else { // Other button
				if (levelOn == 0) {
					topDownMap.gotoLevel(id);
				} else {
					topDownMap.setSelected(id);
					state = 2;
					// Create new model to be placed
					if (modelHead != NULL) {
						editPlacing = false;
						modelHead->cancelMovement();
					}

					if (modelHead == NULL) {
						modelHead = new AugModel((char*)modelPaths[levelOn-1][id-1].c_str(),-1.0f,0.0f,0.0f,id,col);
						modelTail = modelHead;
					} else {
						modelTail->addNewModel((char*)modelPaths[levelOn-1][id-1].c_str(),-1.0f,0.0f,0.0f,id,col);
					}
					editPlacing = true;
				}
			}
		} else {
			state = 0;
		}
	}
}