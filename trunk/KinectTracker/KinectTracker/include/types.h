#ifndef H_TYPES
#define H_TYPES

//#define RECORD_RAW

#define USE_KINECT true
#define DEPTH_SCALE_FACTOR 16
#define MAX_FLOOR_POINTS ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MAX_WALL_POINTS ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MAX_ALLOWED_DIS 700.0f
#define NUM_SLICES (640/DEPTH_SCALE_FACTOR)
#define CLOUD_SIZE ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define CLOUD_AVG_FACTOR 0.0f
#define MIN_FLOOR_POINTS 25
#define AVG_STRENGTH 0.1f

#define PI 3.14159265

union SlicePoint {
	struct {
		float x;
		float z;
	};
	struct {
		float dir;
		float dis;
	};
};

struct Line {
	float m;
	float b;
	float n;
};

struct LineSeg {
	bool  isTypeX;
	float loc;
	float start;
	float stop;
	int   n;
};

struct GlobalMapPoint {
	float x;
	float z;
	float weight;
};

struct GlobalMapCorner {
	float x;
	float z;
	float leftDir;
	float leftDis;
	float leftWeight;
	float rightDir;
	float rightDis;
	float rightWeight;
};

struct LocalMapCorner {
	float x;
	float z;
	bool  truth;
	float leftDir;
	float leftDis;
	float leftWeight;
	float rightDir;
	float rightDis;
	float rightWeight;
	int   matchingGlobal;
};

#endif