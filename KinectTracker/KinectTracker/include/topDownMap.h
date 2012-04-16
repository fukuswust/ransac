#ifndef H_TOPDOWNMAP
#define H_TOPDOWNMAP

#include "types.h"

class TopDownMap {
private:
	int radius;
	int cx;
	int cy;
	int miniRadius;
	bool drawPoint(float x, float z);
	void drawLine(Line tdLine);
	void drawLineSeg(LineSeg lineSeg);
	void drawLineSegBounded(LineSeg lineSeg);
	void drawMiniCircles();
	void drawMiniCircle(float angle);

public:
	void draw();
	void setRadius(int r) {radius = r;}
	void setX(int x) {cx = x;}
	void setY(int y) {cy = y;}
	void setMiniRadius(int r) {miniRadius = r;}
};

#endif