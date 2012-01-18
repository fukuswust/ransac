#ifndef H_LISTENER
#define H_LISTENER

#include "Kinect-win32.h"

// the listener callback object. Implement these methods to do your own processing
class Listener: public Kinect::KinectListener
{
public:
		virtual void KinectDisconnected(Kinect::Kinect *K);
		
		// Depth frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseDepthBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
		virtual void DepthReceived(Kinect::Kinect *K);
		
		// Color frame reception complete. this only means the transfer of 1 frame has succeeded. 
		// No data conversion/parsing will be done until you call "ParseColorBuffer" on the kinect 
		// object. This is to prevent needless processing in the wrong thread.
		virtual void ColorReceived(Kinect::Kinect *K);
		
		// not functional yet:
		virtual void AudioReceived(Kinect::Kinect *K) ;
};

#endif