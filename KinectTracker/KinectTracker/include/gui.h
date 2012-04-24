//Include OpenGL header files, so that we can use OpenGL
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <string>
using namespace std;

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
string** getAllFiles(string path, string searchPattern);
void checkKinect(int value);
void drawFps(int cx, int cy);

#endif