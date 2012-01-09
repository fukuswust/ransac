//Include OpenGL header files, so that we can use OpenGL
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
// Need to include file with orthoPrint!!! Currently in main file.

#ifndef GUI_H
#define GUI_H

void drawRectBorder(int x1, int y1, int x2, int y2);

void drawCircle(float cx, float cy, float r, int num_segments);

void drawCircleSolid(float cx, float cy, float r, int num_segments);

void drawCenteredTiltedLine(float cx, float cy, float r, float angle);

void drawCircleHash(float cx, float cy, float r, float l, int num_segments);

void drawHeightHud(int topx, int topy, float height);

void drawRollHud(int cx, int cy, int r, float roll);

void drawPitchHud(int cx, int cy, int r, float pitch);

#endif