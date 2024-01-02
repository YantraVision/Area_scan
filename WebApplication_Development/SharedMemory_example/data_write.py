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

def multiImg_test():
    #frames = glob.glob('./*.jpeg')
    #f_num = len(glob.glob('./*.jpeg'))
    frames = ["multi_image.jpeg","bubble.jpeg","building.jpeg","river.jpeg","camera.jpeg"]
    while 1:
        for f in frames:
            print(f)
            f_img = cv2.imread(f)
            # image -> array
            numpy_data = np.asarray(f_img)
            h,w,c = numpy_data.shape
            print(h,w,c)
    
            cv2.imshow("input data",numpy_data)
            cv2.waitKey(2000)
            #print(type(numpy_data), numpy_data.shape)
        
            # storing image to shared memory
            b_arr =np.ndarray((h,w,c), dtype=np.uint8, buffer=shm_a.buf)
            b_arr[:] = numpy_data[:]
            #print(type(shm_a.buf), len(shm_a.buf))
            #print(type(b_arr), b_arr.shape)
            #cv2.imshow("b data ",b_arr)
        
    
if __name__ == "__main__":
    multiImg_test()
    shm_a.close()
    shm_a.unlink()

