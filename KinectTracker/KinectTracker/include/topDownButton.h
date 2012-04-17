#ifndef H_TOPDOWNBUTTON
#define H_TOPDOWNBUTTON

#include "types.h"

class TopDownButton {
private:
	float x,y,radius;
	int id;
	int levelOn;
	int state;
		// 0 - not clicked
		// 1 - pressed
		// 2 - down

public:
	TopDownButton();
	void setX(float cx) {x = cx;}
	void setY(float cy) {y = cy;}
	void setRadius(float cr) {radius = cr;}
	void setId(int cId) {id = cId;}
	void setLevelOn(int lo) {levelOn = lo; state = 0;}
	void draw();
	bool mouseIsInside();
	void mouseLeftPress();
	void mouseLeftRelease();
};

#endif

