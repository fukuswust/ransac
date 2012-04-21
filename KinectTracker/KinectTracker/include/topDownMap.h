#ifndef H_TOPDOWNMAP
#define H_TOPDOWNMAP

#include "types.h"
#include "topDownButton.h"

//Top Down Map - Locked to Top Right
#define HUD_MAP_RADIUS 150
#define HUD_MAP_X (5 + HUD_MAP_RADIUS)
#define HUD_MAP_Y (5 + HUD_MAP_RADIUS)
#define HUD_MAP_BTN_RADIUS 35

class TopDownMap {
private:
	int radius;
	int cx;
	int cy;
	int btnRadius;
	bool editMode;
	bool mousePressed;
	TopDownButton topDownButton[8];
	int levelOn;
	int selected;

	bool drawPoint(float x, float z);
	void drawLineSeg(LineSeg lineSeg, bool offset);
	bool mouseIsInside();

public:
	TopDownMap();
	void draw();
	void mouseLeftPress();
	void mouseLeftRelease();
	void mouseRightPress();
	void mouseRightRelease();
	void gotoLevel(int level);
	void setSelected(int value);

};

#endif