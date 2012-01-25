#include <stdlib.h> //Needed for "exit" function
#include "globals.h"

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
					int x, int y) {    //The current mouse coordinates
	switch (key) {

		case 27: { //Escape key
			// remove and delete the listener instance
			K->RemoveListener(L);
			delete L;
	
			//turn the led off
			K->SetLedMode(Kinect::Led_Off);
	
			// when the KinectFinder instance is destroyed, it will tear down and free all kinects.
			exit(0); //Exit the program
		}
	}
}