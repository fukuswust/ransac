#include "listener.h"
#include "globals.h"

void Listener::KinectDisconnected(Kinect::Kinect *K) 
{
	printf("Kinect disconnected!\n");
};
		
		// Depth frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseDepthBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
void Listener::DepthReceived(Kinect::Kinect *K) 
{
	mDepthFrameOn++;
	// K->mDepthBuffer is now valid and usable!
	// see Kinect-Demo.cpp for a more complete example on what to do with this buffer
};
		
		// Color frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseColorBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
void Listener::ColorReceived(Kinect::Kinect *K) 
{
	mColorFrameOn++;
	// K->mColorBuffer is now valid and usable!
	// see Kinect-Demo.cpp for a more complete example on what to do with this buffer
};
		
		// not functional yet:
void Listener::AudioReceived(Kinect::Kinect *K) {};