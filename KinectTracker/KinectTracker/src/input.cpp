#include <stdlib.h> //Needed for "exit" function
#include "globals.h"

#define ESC_KEY 27

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
					int x, int y) {    //The current mouse coordinates
	switch (key) {
		case ESC_KEY: { //Escape key
			// remove and delete the listener instance
			K->RemoveListener(L);
			delete L;
	
			//turn the led off
			K->SetLedMode(Kinect::Led_Off);
	
			// when the KinectFinder instance is destroyed, it will tear down and free all kinects.
			exit(0); //Exit the program
			break;
		}
		case 'c': {
			yawValue = 999999.0f;
			break;
		}
		case 'f': {
			// Toggle Fullscreen
			isFullscreen = !isFullscreen;
			if (isFullscreen) {
				glutFullScreen();
			} else {
				glutReshapeWindow(640, 480);
				glutPositionWindow(200, 200);
			}
			break;
		}
		case 'm': {
			K->SetMotorPosition(1);
			break;
		}
		case 'o': {
			// Toggle onscreen display
			showHud = !showHud;
			// The rest of this is handled in gui.cpp
			break;
		}
		case '1': {
			showHeightSlice = !showHeightSlice;
			break;
		}
		case '2': {
			showFloorPoints = !showFloorPoints;
			break;
		}
		case '3': {
			showWallPoints = !showWallPoints;
			break;
		}

		case 'w': {
			augCubeZ -= 10.0f;
			break;
		}
		case 's': {
			augCubeZ += 10.0f;
			break;
		}
		case 'a': {
			augCubeX -= 10.0f;
			break;
		}
		case 'd': {
			augCubeX += 10.0f;
			break;
		}
	}
}