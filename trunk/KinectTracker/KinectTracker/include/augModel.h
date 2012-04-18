#ifndef H_AUGMODEL
#define H_AUGMODEL

#include "Model_3DS.h"

class AugModel
{
private:
	char *path;
	Model_3DS m;
	float x,y,z;
	float prevX, prevZ;
	float minX, maxX, minY, maxY, minZ, maxZ;
	float width, depth;
	float scale, rot;
	bool placing;
	bool moving;
	float tdPt1X, tdPt1Z, tdPt2X, tdPt2Z;
	AugModel* next;
	AugModel* prev;
	bool tdVisible;
	bool augVisible;
	void autoScaleModel(float amount, float cScale);
	bool mouseIsInside();
	void AugModel::scaleMultiplier(float mult);
public:
	AugModel(char file[], float cScale, float cRot, float cHeight);
	~AugModel();
	void drawTopDown(float cx, float cy, float r);
	void drawAugmentation();
	void addNewModel(char file[]);
	void cancelMovement();
	void mouseLeftPress();
	void mouseLeftRelease();
	void mouseRightPress();
	void keyPressW();
	void keyPressA();
	void keyPressS();
	void keyPressD();
	void keyPressQ();
	void keyPressE();
};

#endif