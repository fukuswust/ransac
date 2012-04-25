#include "globals.h"
#include "algorithm.h"

#include <math.h>
#include <stdio.h>
#include <conio.h> // for _kbhit and _getch 
#include <fstream>
#include "gui.h"
#include "input.h"

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <iostream>

int initKinect() {
	KF = new Kinect::KinectFinder;
	while (KF->GetKinectCount() < 1)
	{
		printf("Unable to find Kinect devices... Is one connected?\n");
		delete KF;
		Sleep(500);
		KF = new Kinect::KinectFinder;
		//return 0;
	}

	K = KF->GetKinect();
	if (K == 0)
	{
		printf("error getting Kinect...\n");
		return 0;
	};
	
	// create a new Listener instance
	L = new Listener();
	
	// register the listener with the kinect. Make sure you remove the 
	// listener before deleting the instance! A good place to unregister 
	// would be your listener destructor.
	K->AddListener(L);
	
	// Led mode ranges from 0 to 7, see the header for possible values
	K->SetLedMode(Kinect::Led_Red);
	
	// Grab 10 accelerometer values from the kinect
	/*float x,y,z;
	for (int i =0 ;i<10;i++)
	{
		if (K->GetAcceleroData(&x,&y,&z))
		{
			printf("accelerometer reports: %f,%f,%f\n", x,y,z);
		}
		Sleep(5);
	};*/
	kinectConnected = true;
	return -1; //Do not exit
}

int main(int argc, char **argv) {
	#ifdef USE_KINECT
		int ret = initKinect();
		if (ret != -1) {return ret;}
	#endif
	if (!SHOW_COMMAND_LINE) {
		HWND hWnd = GetConsoleWindow();
		ShowWindow( hWnd, SW_HIDE );
	}
	initGui(argc, argv); // Contains GUI loop
};
