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
from flask import Flask,render_template

app = Flask(__name__)
img= os.path.join('static','Images')
# Creating a shared memory buffer named 'test_buf'
#shm_a = shared_memory.SharedMemory(create=True, size=100000000, name='test_buf')

def singleImg_test():
    # sample image
    image = Image.open('sample.png')
    #print(image.size)
    
    # image -> array
    numpy_data = np.asarray(image)
    cv2.imshow("input data",numpy_data)
    h,w,c = numpy_data.shape
    print(h,w,c)
    #print(type(numpy_data), numpy_data.shape)
    
    # storing image to shared memory
    b_arr =np.ndarray((h,w,c), dtype=np.uint8, buffer=shm_a.buf)
    b_arr[:] = numpy_data[:]
    #print(type(shm_a.buf), len(shm_a.buf))
    #print(type(b_arr), b_arr.shape)
    #cv2.imshow("b data ",b_arr)
    
    # Reading from shared memory
    shm_b = shared_memory.SharedMemory(create=False, size=100000000, name='test_buf')
    image_arr =np.ndarray(((h*w*c)), dtype=np.uint8, buffer=shm_b.buf)
    inter_data = np.array(image_arr)
    arr_reshape = inter_data.reshape(148,160,3)
    print(type(arr_reshape), arr_reshape.shape)
    #img_data = Image.fromarray(arr_reshape)
    #img_data.save("dummy.png")
    cv2.imshow("output data", arr_reshape)
    cv2.waitKey(5000)

@app.route('/')
def home_page():
    return render_template('index.html')

@app.route('/test')
def multiImg_test():
    #frames = glob.glob('./*.jpeg')
    #f_num = len(glob.glob('./*.jpeg'))
    #frames = [print(f) Image.open(f)for f in frames]
    #for f in frames:
    #    print(f)
    #    f_img = cv2.imread(f)
    #    # image -> array
    #    numpy_data = np.asarray(f_img)
    #    h,w,c = numpy_data.shape
    #    #print(h,w,c)
    #
    #    cv2.imshow("input data",numpy_data)
    #    #print(type(numpy_data), numpy_data.shape)
    #    
    #    # storing image to shared memory
    #    b_arr =np.ndarray((h,w,c), dtype=np.uint8, buffer=shm_a.buf)
    #    b_arr[:] = numpy_data[:]
    #    #print(type(shm_a.buf), len(shm_a.buf))
    #    #print(type(b_arr), b_arr.shape)
    #    #cv2.imshow("b data ",b_arr)
    #    
    #print("length of b array: ",(f_num))
    # Reading from shared memory
    #for i in range(f_num):
    while 1:
        shm_b = shared_memory.SharedMemory(create=False, size=100000000, name='test_buf')
        image_arr =np.ndarray((183*275*3), dtype=np.uint8, buffer=shm_b.buf)
        inter_data = np.array(image_arr)
        arr_reshape = inter_data.reshape(183,275,3)
        print(type(arr_reshape), arr_reshape.shape)
        img_data = Image.fromarray(arr_reshape)
        img_data.save("static/Images/dummy.png")
        file_path= os.path.join(img,'dummy.png')
        #cv2.imshow("output image", arr_reshape)
        #cv2.waitKey(2000)
        print(file_path)
        return render_template('image_render.html',img=file_path)
    

if __name__ == "__main__":
    #singleImg_test()
    #multiImg_test()
    app.run(debug=False,host="172.16.0.90",port=5000)
    shm_a.close()
    shm_a.unlink()
