#include "listener.h"
#include "globals.h"
#include "algorithm.h"

void Listener::KinectDisconnected(Kinect::Kinect *K) 
{
	printf("Kinect disconnected!\n");
};
		
		// Depth frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseDepthBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
void Listener::DepthReceived(Kinect::Kinect *K) 
{
	K->ParseDepthBuffer(); 

	// GET ACCEL DATA
	K->GetAcceleroData(&accelVector[0], &accelVector[1], &accelVector[2]);

	runAlgorithm();
};
		
		// Color frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseColorBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
void Listener::ColorReceived(Kinect::Kinect *K) 
{
	K->ParseColorBuffer();
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, K->mColorBuffer );
};
		
		// not functional yet:
void Listener::AudioReceived(Kinect::Kinect *K) {};