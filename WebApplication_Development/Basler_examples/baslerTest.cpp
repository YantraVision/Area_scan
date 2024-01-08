#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include <pylon/PylonIncludes.h>

using namespace std;

//using namespace cv;
using namespace Pylon;

int c_countOfImagesToGrab = 1;
int fd = shm_open("my_shared_memory", O_CREAT | O_RDWR, 0666);
int fd1 = shm_open("my_shared_memory", O_CREAT | O_RDWR, 0666);
    

    // Map the shared memory object into our address space
    uint8_t *shared_memory = (uint8_t *)mmap(NULL, 1073741824, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    uint8_t *shared_out = (uint8_t *)mmap(NULL, 1073741824, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    

int main()
{
	/*
	 //int arr[]={10,20,40};
	//cout<<sizeof(arr)<<endl;
	// ftok to generate unique key
	//key_t key = ftok("shmfile", 65);
	key_t key1 = ftok("shmfile", 66);
	key_t key2 = ftok("shmfile", 67);

	// shmget returns an identifier in shmid
	//int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
	int shmid1 = shmget(key1, 1024, 0666 | IPC_CREAT);
	int shmid2 = shmget(key2, 1024, 0666 | IPC_CREAT);
	int shmid_out = shmget(key1, 1024, 0666 | IPC_CREAT);

	// shmat to attach to shared memory
	//char* str = (char*)shmat(shmid, (void*)0, 0);
	uint8_t* str_data1 = (uint8_t*)shmat(shmid1, (void*)0, 0);
	uint8_t* str_data2 = (uint8_t*)shmat(shmid2, (void*)0, 0);
	uint8_t* str_out = (uint8_t*)shmat(shmid_out, (void*)0, 0);

	//cout << "Write Data : ";
	//cin.getline(str, 1024);
	//cout << "Data written in memory: " << str << endl;
	*/

	if (fd == -1) {
        perror("shm_open failed");
        return -1;
    }

    // Set the size of the shared memory object
    if (ftruncate(fd, 1073741824) == -1) {  // Size in bytes
        perror("ftruncate failed");
        return -1;
    }

    if (shared_memory == MAP_FAILED) {
        perror("mmap failed");
        return -1;
    }
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
	uint8_t* pImageBuffer;
	unsigned int buf_size ;
	while (camera.IsGrabbing())
	{
	    // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
	    camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException );
	    
	    // Image grabbed successfully?
	    if (ptrGrabResult->GrabSucceeded())
	    {
	        // Access the image data.
	        cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
	        cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
	        cout << "component: " << ptrGrabResult->GetDataComponentCount() << endl;
		
	        pImageBuffer = (uint8_t*) ptrGrabResult->GetBuffer();
		buf_size = ptrGrabResult->GetBufferSize();
	        cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl<< ptrGrabResult->GetImageSize() <<" " << ptrGrabResult->GetBufferSize() << endl;

		cout << (uint32_t)pImageBuffer[0] << " " << (uint32_t)*(pImageBuffer+1) << " " << (uint32_t)*(pImageBuffer+2) << " " << (uint32_t)*(pImageBuffer+3) << " " << (uint32_t)*(pImageBuffer+4)<< " " << (uint32_t)pImageBuffer[(buf_size/8)-1] <<endl;
		//imshow("image",pImageBuffer);
    		// Display the image.
		#ifdef PYLON_WIN_BUILD
		    Pylon::DisplayImage(1, pImageBuffer);
		#endif
	    }
	    else
	    {
	        cout << "Error: " << std::hex << ptrGrabResult->GetErrorCode() << std::dec << " " << ptrGrabResult->GetErrorDescription() << endl;
	    }
	    cout<< "data : "<<  (uint32_t)pImageBuffer[0]<<endl;
	memcpy(shared_memory,pImageBuffer,1073741824);
	cout<<shared_memory[0]<<endl;
	cout<<shared_out[0]<<endl;
//	memcpy(str_data1,pImageBuffer,(buf_size/256));
//	memcpy(str_data2,(pImageBuffer+(buf_size/256)),(buf_size/256));
	/*for (int i=0; i<(buf_size/2); i++) {
		str_data[i] = pImageBuffer[i];
	}*/
	 /*   str_data[0] = pImageBuffer[0];
	    //cout << "str data: "<<(uint32_t)str_data[0]<<endl;
	    str_data[1] =pImageBuffer[1];
	    str_data[2] =pImageBuffer[2];
	    str_data[3] =pImageBuffer[3];
	    str_data[4] =pImageBuffer[4];
	    */
/*	cout << (uint32_t)str_data1[0] <<" " << (uint32_t)str_data1[1] <<" " << (uint32_t)str_data1[2] <<" " << (uint32_t)str_data1[3] <<" " << (uint32_t)str_data1[4] <<" " << endl;
	cout << (uint32_t)str_data2[0] <<" " << (uint32_t)str_data2[1] <<" " << (uint32_t)str_data2[2] <<" " << (uint32_t)str_data2[3] <<" " << (uint32_t)str_data2[4] <<" " << endl;
	cout << (uint32_t)str_out[0] <<" " << (uint32_t)str_out[1] <<" " << (uint32_t)str_out[2] <<" " << (uint32_t)str_out[3] <<" " << (uint32_t)str_out[4] <<" " << endl;
	//cout << "Data written in memory: " << *str_data <<" " << *(str_data+1)<< " " << *(str_data+2)<< " " << *(str_data+3)<< endl;

	shmdt(str_data1);
	shmdt(str_data2);
	shmdt(str_out);
	*/
	}
	// detach from shared memory
	if (shm_unlink("my_shared_memory") == -1) {
        perror("shm_unlink failed");
        return -1;
    }

	return 0;
}

