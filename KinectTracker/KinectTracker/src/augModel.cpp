#include "augModel.h"
#include "globals.h"

AugModel::AugModel(char file[]) {
	m.Load(file);
}

void AugModel::drawTopDown() {

}

void AugModel::drawAugmentation() {
	glEnable(GL_LIGHTING);
	m.Draw();
	m.pos.z = -5000.0f; // Make smaller so we can see it

	//glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D); // Disable texturing so the colors don't bleed into the osd
}