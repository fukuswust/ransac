#ifndef H_AUGMODEL
#define H_AUGMODEL

#include "Model_3DS.h"

class AugModel
{
private:
	Model_3DS m;
	float x,y,z;
	float minX, maxX, minY, maxY, minZ, maxZ;
	float scale;
public:
	AugModel(char file[]);
	void drawTopDown(float cx, float cy, float r);
	void drawAugmentation();
	void setX(float cx) {x = cx;}
	void setY(float cy) {y = cy;}
	void setZ(float cz) {z = cz;}
};

#endif