from pypylon import pylon

from multiprocessing import shared_memory
import cv2
import sys
import threading
import os
import termios
from PIL import Image
import glob
import time
import numpy as np
from ctypes import *

# Creating a shared memory buffer named 'test_buf'
shm_a = shared_memory.SharedMemory(create=True, size=100000000, name='test_buf')

tl_factory = pylon.TlFactory.GetInstance()
#print(type(tl_factory))
devices = tl_factory.EnumerateDevices()
#print(type(devices), devices)
#for device in devices:
#    print(device.GetFriendlyName())
camera = pylon.InstantCamera()
camera.Attach(tl_factory.CreateFirstDevice())
camera.Open()
camera.PixelFormat.Value = "RGB8"

def multiImg_test():
    #frames = glob.glob('./*.jpeg')
    #f_num = len(glob.glob('./*.jpeg'))
    #frames = ["multi_image.jpeg","bubble.jpeg","building.jpeg","river.jpeg","camera.jpeg"]
    i = 1
    j=1
    while i == 1:
        i+=1
        #camera.StartGrabbing(1)
        camera.StartGrabbing(pylon.GrabStrategy_LatestImages)
        while camera.IsGrabbing():
            j+=1
            grab = camera.RetrieveResult(200, pylon.TimeoutHandling_ThrowException)
            if grab.GrabSucceeded():
                #print(type(grab))
                img = grab.GetArray()
                grab.Release()
                #print(img.shape) #960,1280,3
                #print(f'Size of image: {img.shape}')
                print(camera.Gain.GetValue())
                print(camera.ExposureTime.GetValue())
                print(camera.AcquisitionFrameRate.GetValue())
                cv2.imshow("image",img)
                h,w,c = img.shape
                #print(h,w,c)
    
                cv2.imshow("input data",img)
                #cv2.waitKey(20)
                #print(type(numpy_data), numpy_data.shape)
            
                # storing image to shared memory
                b_arr =np.ndarray((h,w,c), dtype=np.uint8, buffer=shm_a.buf)
                b_arr[:] = img[:]
                #print(type(shm_a.buf), len(shm_a.buf))
                #print(type(b_arr), b_arr.shape)
                #cv2.imshow("b data ",b_arr)
            if j==1000:
                break
    camera.Close()
        
    
if __name__ == "__main__":
    multiImg_test()
    shm_a.close()
    shm_a.unlink()

