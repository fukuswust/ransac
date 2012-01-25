#include "globals.h"
#include "algorithm.h"

#include <math.h>
#include <stdio.h>
#include <conio.h> // for _kbhit and _getch 
#include <fstream>
#include "record.h"
#include "gui.h"
#include "input.h"

int initKinect() {
	if (KF.GetKinectCount() < 1)
	{
		printf("Unable to find Kinect devices... Is one connected?\n");
		return 0;
	}

	K = KF.GetKinect();
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

	// SetMotorPosition accepts 0 to 1 range
	//K->SetMotorPosition(1);
	
	// Led mode ranges from 0 to 7, see the header for possible values
	K->SetLedMode(Kinect::Led_Red);
	
	// Grab 10 accelerometer values from the kinect
	float x,y,z;
	for (int i =0 ;i<10;i++)
	{
		if (K->GetAcceleroData(&x,&y,&z))
		{
			printf("accelerometer reports: %f,%f,%f\n", x,y,z);
		}
		Sleep(5);
	};
	return -1; //Do not exit
}

int main(int argc, char **argv) {
	int ret = initKinect();
	if (ret != -1) {return ret;}
	initGui(argc, argv); // Contains GUI loop
};
