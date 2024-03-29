// ImageCapturingUsingDirectShow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"WebCamSampleGrabber.h"

//Captured images are saved at "CapturedImages\"

int NoOfFrames = 100;

int main()
{
	WebCamSampleGrabber* WebCamSampleGrabberObj = new WebCamSampleGrabber();
	WebCamSampleGrabberObj->InitializeAndStartCapturing();
	int count = 0;
	while (NoOfFrames > count)
	{
		WaitForSingleObject(WebCamSampleGrabberObj->CB->FrameCaptured, INFINITE);
		count++;
	}

	return 0;
}
