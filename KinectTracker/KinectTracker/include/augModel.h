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
	float cx, cy;
	bool placing;
	bool moving;
	float tdPt1X, tdPt1Z, tdPt2X, tdPt2Z;
	AugModel* next;
	AugModel* prev;
	bool tdVisible;
	bool augVisible;
	int id;
	float col[3];
	void autoScaleModel(float amount, float cScale);
	bool mouseIsInside();
	void AugModel::scaleMultiplier(float mult);
public:
	AugModel(char file[], float cScale, float cRot, float cHeight, int cId, float cCol[3]);
	~AugModel();
	void drawTopDown(float ccx, float ccy, float r);
	void drawAugmentation();
	void addNewModel(char file[], float cScale, float cRot, float cHeight, int cId, float cCol[3]);
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