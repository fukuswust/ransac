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

void update(int value);
void initGui(int argc, char **argv);
void initRendering();
void handleResize(int w, int h);
void drawScene();
void orthogonalStart (int viewWidth, int viewHeight);
void orthogonalEnd (void);
void setGlTransformation();

#endif