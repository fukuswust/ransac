#define RECORD_DEPTH
#define RECORD_COLOR
#define RECORD_ACCEL
#define RECORD_TIMING
//0: No Display
//1: Color Display
//2: Depth Display
#define DISPLAY 1

#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <CLNUIDevice.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CImg.h"
#include "hr_time.h"
#include <direct.h>

using namespace std;
using namespace cimg_library;

HWND    hWnd;
HDC     hDC;
HGLRC   hRC;

CImg<BYTE> colorImage(640,480,1,3);
CImgDisplay main_disp(colorImage,"Color Video Feed");

unsigned short int depthData[640*480];
BYTE colorData[640*480*3];
unsigned int dataWidth = 640U;
unsigned int dataHeight = 480U;

CLNUIMotor motor;
CLNUICamera camera;
CStopWatch* myStopWatch;
unsigned int outFileOn;

char depthPath[256];
char colorPath[256];

void InitKinect() {
	ifstream pathFile("paths.txt");
	pathFile.getline(depthPath,256);
	pathFile.getline(colorPath,256);
	pathFile.close();

	mkdir(depthPath);
	mkdir(colorPath);

	camera = CreateNUICamera();
	StartNUICamera(camera);
	myStopWatch = new CStopWatch();
	myStopWatch->startTimer();
	outFileOn = 0;
}

void DrawGraphics(void) {
	FILE * depthFile;
	char bufferFileName[128];

	GetNUICameraColorFrameRGB24(camera, colorData, 1000);
	GetNUICameraDepthFrameRAW(camera, depthData, 1000);

	sprintf(bufferFileName,"%s/%.6u.csv",depthPath,outFileOn);
	depthFile = fopen (bufferFileName,"w");

	unsigned int y = 0;
	unsigned int x = 0;
	unsigned int totalPoints = 0;
	y = 0;
	for (; y < dataHeight; ++y) {
		x = 0;
		for (; x < dataWidth-1; ++x) {
			colorImage(x,y,0,0) = colorData[(((y*dataWidth)+x)*3)+2];
			colorImage(x,y,0,1) = colorData[(((y*dataWidth)+x)*3)+1];
			colorImage(x,y,0,2) = colorData[(((y*dataWidth)+x)*3)+0];
			fprintf(depthFile,"%u,",depthData[(y*dataWidth)+x]);
		}
		colorImage(x,y,0,0) = colorData[(((y*dataWidth)+x)*3)+2];
		colorImage(x,y,0,1) = colorData[(((y*dataWidth)+x)*3)+1];
		colorImage(x,y,0,2) = colorData[(((y*dataWidth)+x)*3)+0];
		fprintf(depthFile,"%u",depthData[(y*dataWidth)+x]);
		fprintf(depthFile,"\n");
	}

	main_disp.display(colorImage);

	sprintf(bufferFileName,"%s/%.6u.bmp",colorPath,outFileOn++);
	colorImage.save_bmp(bufferFileName); 

	fclose(depthFile);
}

// Handle window events and messages
LONG WINAPI MainWndProc (HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE: 
        DestroyWindow(hWnd);
        break;
 
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
 
    // Default event handler
    default: 
        return DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return 1; 
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const LPCWSTR appname = TEXT("Main Window");

    WNDCLASS wndclass;
    MSG      msg;
 
    // Define the window class
    wndclass.style         = 0;
    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(hInstance, appname);
    wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wndclass.lpszMenuName  = appname;
    wndclass.lpszClassName = appname;
 
    // Register the window class
    if (!RegisterClass(&wndclass)) return FALSE;
 
    // Create the window
    hWnd = CreateWindow(
            appname,
            appname,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            640,
            480,
            NULL,
            NULL,
            hInstance,
            NULL);
 
    if (!hWnd) return FALSE;

	// Initialize Kinect 
	InitKinect();

    // Display the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Event loop
    while (!main_disp.is_closed())
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
        {
            if (!GetMessage(&msg, NULL, 0, 0)) return TRUE;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        DrawGraphics();
    }

	DestroyNUIMotor(motor);
	StopNUICamera(camera);
	DestroyNUICamera(camera);
	wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
	delete myStopWatch;
}