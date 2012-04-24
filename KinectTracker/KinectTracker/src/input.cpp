#include <stdlib.h> //Needed for "exit" function
#include "globals.h"

#define ESC_KEY 27

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
					int x, int y) {    //The current mouse coordinates
	switch (key) {
		case ESC_KEY: { //Escape key
			#ifdef USE_KINECT
				// remove and delete the listener instance
				K->RemoveListener(L);
				delete L;
	
				//turn the led off
				K->SetLedMode(Kinect::Led_Off);
			#endif
	
			// when the KinectFinder instance is destroyed, it will tear down and free all kinects.
			exit(0); //Exit the program
			break;
		}
		case ' ': {
			// Toggle Recording
			if (!topDownMap.inEditMode()) {
				mapRecord = !mapRecord;
			}
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
			// Level motor position
			K->SetMotorPosition(1);
			break;
		}
		case 'h': {
			// Toggle Help
			showHelp = !showHelp;
			break;
		}
		case 'o': {
			// Toggle overhead display
			if (!topDownMap.inEditMode()) {
				showHud = !showHud;
			}
			break;
		}
		case 't': {
			// Toggle td display rotation
			if (!topDownMap.inEditMode()) {
				tdDisplayTracking = !tdDisplayTracking;
			}
			break;
		}
		case '1': {
			// Toggle floor points
			showFloorPoints = !showFloorPoints;
			break;
		}
		case '2': {
			// Toggle wall points
			showWallPoints = !showWallPoints;
			break;
		}

		// Aug model movement
		case 'w': {
			if (modelHead != NULL) {modelHead->keyPressW();}
			break;
		}
		case 'a': {
			if (modelHead != NULL) {modelHead->keyPressA();}
			break;
		}
		case 's': {
			if (modelHead != NULL) {modelHead->keyPressS();}
			break;
		}
		case 'd': {
			if (modelHead != NULL) {modelHead->keyPressD();}
			break;
		}
		case 'q': {
			if (modelHead != NULL) {modelHead->keyPressQ();}
			break;
		}
		case 'e': {
			if (modelHead != NULL) {modelHead->keyPressE();}
			break;
		}

		case '=': {
			maxAllowedDis += 100.0f;
			break;
		}
		case '-': {
			if (maxAllowedDis > 100.0f) {
				maxAllowedDis -= 100.0f;
			}
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