#include <stdlib.h> //Needed for "exit" function
#include "globals.h"

#define ESC_KEY 27

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
					int x, int y) {    //The current mouse coordinates
	switch (key) {
		case ESC_KEY: { //Escape key
			if (USE_KINECT) {
				// remove and delete the listener instance
				K->RemoveListener(L);
				delete L;
	
				//turn the led off
				K->SetLedMode(Kinect::Led_Off);
			}
	
			// when the KinectFinder instance is destroyed, it will tear down and free all kinects.
			exit(0); //Exit the program
			break;
		}
		case 'c': {
			yawValue = 0.0f;
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
			showFloorPoints = !showFloorPoints;
			break;
		}
		case '2': {
			showWallPoints = !showWallPoints;
			break;
		}
	}
}

//Called when the mouse is moved
void handleMouseMove(int x, int y) {
	mouseX = x;
	mouseY = y;
}

//Called when mouse button pressed or released
void handleMouseButtons(int button, int state, int x, int y) {
	if (showHud) {
		if (button == GLUT_LEFT_BUTTON) { // Left Button
			if (state == GLUT_DOWN) {
				topDownMap.mouseLeftPress();
			} else {
				topDownMap.mouseLeftRelease();
			}
		} else if (button == GLUT_RIGHT_BUTTON) { // Right Button
			if (state == GLUT_DOWN) {
				topDownMap.mouseRightPress();
			} else  {
				topDownMap.mouseRightRelease();
			}
		}
	}
}