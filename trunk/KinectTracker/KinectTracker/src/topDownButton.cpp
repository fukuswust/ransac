#include "topDownButton.h"
#include "globals.h"
#include "basicShapes.h"

TopDownButton::TopDownButton() {
	state = 0;
	selected = false;
}

void TopDownButton::draw() {
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
}

bool TopDownButton::mouseIsInside() {
	return (sqrt(pow((float)x-mouseX+viewXOffset,2)+pow((float)y-mouseY+viewYOffset,2))<radius);
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
					topDownMap.gotoLevel(0);
				}
			} else { // Other button
				if (levelOn == 0) {
					topDownMap.gotoLevel(id);
				} else {
					topDownMap.setSelected(id);
					state = 2;
					selected = true;
				}
			}
		} else {
			state = 0;
		}
	}
}