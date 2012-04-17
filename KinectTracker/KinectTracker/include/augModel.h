#ifndef H_AUGMODEL
#define H_AUGMODEL

#include "Model_3DS.h"

class AugModel
{
private:
	char *path;
	Model_3DS m;
	float x,y,z;
	float minX, maxX, minY, maxY, minZ, maxZ;
	float scale;
	bool placing;
	AugModel* next;
	AugModel* prev;
	bool tdVisible;
	bool augVisible;
	void autoScaleModel(float amount);
public:
	AugModel(char file[]);
	~AugModel();
	void drawTopDown(float cx, float cy, float r);
	void drawAugmentation();
	void addNewModel(char file[]);
	void cancelMovement();
	void mouseLeftPress();
};

#endif