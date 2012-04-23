#ifndef H_TYPES
#define H_TYPES

//#define RECORD_RAW

#define USE_KINECT
#define SHOW_COMMAND_LINE false
#define DEPTH_SCALE_FACTOR 16
#define MAX_FLOOR_POINTS ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MAX_WALL_POINTS ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MAX_ALLOWED_DIS 700.0f
#define NUM_SLICES (640/DEPTH_SCALE_FACTOR)
#define CLOUD_SIZE ((640*480)/(DEPTH_SCALE_FACTOR*DEPTH_SCALE_FACTOR))
#define MIN_FLOOR_POINTS 25
#define AVG_STRENGTH 0.1f
#define EDIT_DEL_HEIGHT 5.0f
#define MAX_ACCEL_ERROR 0.3f

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

struct LineSeg {
	bool  isTypeX;
	float loc;
	float start;
	float stop;
	int   n;
};

#endif