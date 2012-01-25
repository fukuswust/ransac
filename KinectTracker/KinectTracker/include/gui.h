//Include OpenGL header files, so that we can use OpenGL
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

#ifndef GUI_H
#define GUI_H

#define DRAW_HEIGHT_SLICE

void update(int value);
void initGui(int argc, char **argv);
void initRendering();
void handleResize(int w, int h);
void drawScene();
void orthogonalStart (int viewWidth, int viewHeight);
void orthogonalEnd (void);
void orthoPrint(int x, int y, char *string);
void drawColorBackground(int viewWidth, int viewHeight, GLuint texID);
void drawCrosshair(int viewWidth, int viewHeight);
void drawCircle(float cx, float cy, float r, int num_segments);
void drawCircleSolid(float cx, float cy, float r, int num_segments);
void drawCenteredTiltedLine(float cx, float cy, float r, float angle);
void drawCircleHash(float cx, float cy, float r, float l, int num_segments);
void drawHeightHud(int topx, int topy, float height);
void drawRollHud(int cx, int cy, int r, float roll);
void drawPitchHud(int cx, int cy, int r, float pitch);
void drawTopDownMap(int cx, int cy, int r);
void drawHeightLine(float heightSlices[], int heightSliceIJ[], int numSlices);
void drawFloorPoints(int floorIJ[], int numPoints);
void drawWallPoints(int wallIJ[], int numPoints);
void drawAugmentedPoint(float x, float y, float z);
void drawTopDownViewPoint(float x,float y, float z);
void drawAugmentedCube(float x, float y, float z, float s);
void drawAugmentedCorners();

#endif