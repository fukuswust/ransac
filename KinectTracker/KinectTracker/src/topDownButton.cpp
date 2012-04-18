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
		float col[3];
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
			glColor3f(0.75f, 0.0f, 0.0f); // Red
			orthoPrintLarge(x-8,y+7,"X");
			glColor3f(0.0f, 0.0f, 0.0f); // Black
		} else if (levelOn == 0) { // Icon

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
						modelHead = new AugModel((char*)modelPaths[levelOn-1][id-1].c_str(),-1.0f,0.0f);
						modelTail = modelHead;
					} else {
						modelTail->addNewModel((char*)modelPaths[levelOn-1][id-1].c_str());
					}
					editPlacing = true;
				}
			}
		} else {
			state = 0;
		}
	}
}