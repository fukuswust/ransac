#ifndef H_TOPDOWNMAP
#define H_TOPDOWNMAP

class TopDownMap {
private:
	int radius;
	int cx;
	int cy;
	bool drawPoint(float x, float z);

public:
	void draw();
	void setRadius(int r) {radius = r;}
	void setX(int x) {cx = x;}
	void setY(int y) {cy = y;}
};

#endif