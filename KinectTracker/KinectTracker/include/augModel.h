#ifndef H_AUGMODEL
#define H_AUGMODEL

#include "Model_3DS.h"

class AugModel
{
private:
	Model_3DS m;
	int x,y,z;
public:
	AugModel(char file[]);
	void drawTopDown();
	void drawAugmentation();
};

#endif