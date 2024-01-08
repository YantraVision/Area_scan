#include <iostream>
#include <pylon/PylonIncludes.h>

#define PYLON_WIN_BUILD
/*
#ifdef PYLON_WIN_BUILD
	#include <pylon/PylonGUI.h>
#endif
*/
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace Pylon;
using namespace std;

int c_countOfImagesToGrab = 10;

int main(int argc, char* argv[]) {

	Pylon::PylonAutoInitTerm autoInitTerm;  // PylonInitialize() will be called now
	// Create an instant camera object with the camera device found first.
	CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice() );
	
	// Print the model name of the camera.
	cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
	
	// Start the grabbing of c_countOfImagesToGrab images.
	// The camera device is parameterized with a default configuration which
	// sets up free-running continuous acquisition.
	camera.StartGrabbing( c_countOfImagesToGrab );
	
	// This smart pointer will receive the grab result data.
	CGrabResultPtr ptrGrabResult;
	
	// Camera.StopGrabbing() is called automatically by the RetrieveResult() method
	// when c_countOfImagesToGrab images have been retrieved.
	while (camera.IsGrabbing())
	{
	    // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
	    camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException );
	    //cout<<ptrGrabResult->IsGrabResultBufferAttached()<<endl;
	
	    // Image grabbed successfully?
	    if (ptrGrabResult->GrabSucceeded())
	    {
	        // Access the image data.
	        cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
	        cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
	        const uint8_t* pImageBuffer = (uint8_t*) ptrGrabResult->GetBuffer();
	        cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl<< ptrGrabResult->GetImageSize() <<" " << ptrGrabResult->GetPixelType() << endl;
		//imshow("image",pImageBuffer);
    		// Display the image.
		/*#ifdef PYLON_WIN_BUILD
		    Pylon::DisplayImage(1, pImageBuffer);
		#endif
		*/
		
	
	    }
	    else
	    {
	        cout << "Error: " << std::hex << ptrGrabResult->GetErrorCode() << std::dec << " " << ptrGrabResult->GetErrorDescription() << endl;
	    }
	}
}
