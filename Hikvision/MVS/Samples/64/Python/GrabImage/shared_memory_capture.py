from multiprocessing import shared_memory
from PyQt5 import QtGui
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import cv2
import sys
import threading
import os
import termios
from ctypes import *
from PIL import Image as im
import time
from PyQt5.QtWidgets import QWidget, QApplication, QLabel, QVBoxLayout
from PyQt5.QtGui import QPixmap
import sys
import cv2
from PyQt5.QtCore import pyqtSignal, pyqtSlot, Qt, QThread
from PIL import Image
import cv2
import sys
import threading
import os
import termios
import numpy as np
from ctypes import *
sys.path.append("../MvImport")
from MvCameraControl_class import *

shm_a = shared_memory.SharedMemory(create=True, size=100000000, name='test1')
b_arr = np.ndarray((400,400), dtype=np.uint8, buffer=shm_a.buf)
g_bExit = False

def work_thread(cam=0, pData=0, nDataSize=0):
        stFrameInfo = MV_FRAME_OUT_INFO_EX()
        memset(byref(stFrameInfo), 0, sizeof(stFrameInfo))
        pData_ref = ctypes.cast(pData,ctypes.POINTER(ctypes.c_uint8))
        print ("pdata_ref",pData_ref) 

        while True:
                ret= cam.MV_CC_GetOneFrameTimeout(pData, nDataSize, stFrameInfo, 1000)
                if ret == 0:
                    time.sleep(1)
                    print ("get one frame: Width[%d], Height[%d], PixelType[0x%x], nFrameNum[%d]"  % (stFrameInfo.nWidth, stFrameInfo.nHeight, stFrameInfo.enPixelType,stFrameInfo.nFrameNum))
                    #print ("pdata",pData) 
                    grabbedArr = np.ctypeslib.as_array(pData_ref, shape=(stFrameInfo.nHeight, stFrameInfo.nWidth))
                    size=(400,400)
                    resized_arr=cv2.resize(grabbedArr,size)
                    arr=resized_arr 
                    print ("array dim is ", resized_arr.shape)
                    print ("grabaddr dim is ", grabbedArr.shape)
                    b_arr[:] = arr[:]  # Copy the original data into shared memory
                    cv2.imshow("shared_Array_capture",b_arr)
                    cv2.waitKey(10)
                    print(b_arr,"shared arr")
                    print("frame captured")
                else:
                    print ("no data[0x%x]" % ret)
                if g_bExit == True:
                    break


def press_any_key_exit():
        fd = sys.stdin.fileno()
        old_ttyinfo = termios.tcgetattr(fd)
        new_ttyinfo = old_ttyinfo[:]
        new_ttyinfo[3] &= ~termios.ICANON
        new_ttyinfo[3] &= ~termios.ECHO
        print ("starts 0")
        #sys.stdout.write(msg)
        #sys.stdout.flush()
        termios.tcsetattr(fd, termios.TCSANOW, new_ttyinfo)
        try:
                os.read(fd, 7)
        except:
                print ("except")
                pass
        finally:
                print ("final")
                termios.tcsetattr(fd, termios.TCSANOW, old_ttyinfo)
        
        print ("starts 1")



class VideoThread(QThread):
    change_pixmap_signal = pyqtSignal(np.ndarray)
    print("*iiiii***** video thread *********")

    def run(self):
        while True:
            #cv_img = cv2.imread("/home/vinoth/Pictures/Screenshot from 2023-05-19 18-46-50.png",mode='RGB')
            for i in range(0,5):
                img=Image.open('/opt/MVS/Samples/64/Python/GrabImage/image_'+str(i)+'.png')
                nw_arry=np.array(img)
                self.change_pixmap_signal.emit(nw_arry)

class App(QWidget):
    def __init__(self):
        super().__init__()
        print("****** gui entered*********")
        self.setWindowTitle("Qt live label demo")
        self.disply_width = 640
        self.display_height = 480
        # create the label that holds the image
        self.image_label = QLabel(self)
        self.image_label.resize(self.disply_width, self.display_height)
        # create a text label
        self.textLabel = QLabel('Webcam')

        # create a vertical box layout and add the two labels
        vbox = QVBoxLayout()
        vbox.addWidget(self.image_label)
        vbox.addWidget(self.textLabel)
        # set the vbox layout as the widgets layout
        self.setLayout(vbox)

        # create the video capture thread
        self.thread = VideoThread()
        # connect its signal to the update_image slot
        self.thread.change_pixmap_signal.connect(self.update_image)
        # start the thread
        print("****** thread *********")
        self.thread.start()



    @pyqtSlot(np.ndarray)
    def update_image(self, cv_img):
        """Updates the image_label with a new opencv image"""
        #print("****** update *********")
        qt_img = self.convert_cv_qt(cv_img)
        self.image_label.setPixmap(qt_img)
    
    def convert_cv_qt(self, cv_img):
        """Convert from an opencv image to QPixmap"""
        #print(cv_img)
        #rgb_image = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
        #h, w, ch = rgb_image.shape
        h, w = cv_img.shape
        #bytes_per_line = ch * w
        #convert_to_Qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)
        convert_to_Qt_format = QtGui.QImage(cv_img.data, w, h, QtGui.QImage.Format_Indexed8)
        p = convert_to_Qt_format.scaled(self.disply_width, self.display_height, Qt.KeepAspectRatio)
        return QPixmap.fromImage(p)
    
if __name__=="__main__":
    
    #shm_a = shared_memory.SharedMemory(create=True, size=100000000)
    #app = QApplication(sys.argv)
    SDKVersion = MvCamera.MV_CC_GetSDKVersion()
    print ("SDKVersion[0x%x]" % SDKVersion)

    deviceList = MV_CC_DEVICE_INFO_LIST()
    tlayerType = MV_GIGE_DEVICE | MV_USB_DEVICE
    
    # ch:枚举设备 | en:Enum device
    ret = MvCamera.MV_CC_EnumDevices(tlayerType, deviceList)
    if ret != 0:
            print ("enum devices fail! ret[0x%x]" % ret)
            sys.exit()

    if deviceList.nDeviceNum == 0:
            print ("find no device!")
            sys.exit()

    print ("Find %d devices!" % deviceList.nDeviceNum)

    for i in range(0, deviceList.nDeviceNum):
            mvcc_dev_info = cast(deviceList.pDeviceInfo[i], POINTER(MV_CC_DEVICE_INFO)).contents
            if mvcc_dev_info.nTLayerType == MV_GIGE_DEVICE:
                    print ("\ngige device: [%d]" % i)
                    strModeName = ""
                    for per in mvcc_dev_info.SpecialInfo.stGigEInfo.chModelName:
                            strModeName = strModeName + chr(per)
                    print ("device model name: %s" % strModeName)

                    nip1 = ((mvcc_dev_info.SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24)
                    nip2 = ((mvcc_dev_info.SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16)
                    nip3 = ((mvcc_dev_info.SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8)
                    nip4 = (mvcc_dev_info.SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff)
                    print ("current ip: %d.%d.%d.%d\n" % (nip1, nip2, nip3, nip4))
            elif mvcc_dev_info.nTLayerType == MV_USB_DEVICE:
                    print ("\nu3v device: [%d]" % i)
                    strModeName = ""
                    for per in mvcc_dev_info.SpecialInfo.stUsb3VInfo.chModelName:
                            if per == 0:
                                    break
                            strModeName = strModeName + chr(per)
                    print ("device model name: %s" % strModeName)

                    strSerialNumber = ""
                    for per in mvcc_dev_info.SpecialInfo.stUsb3VInfo.chSerialNumber:
                            if per == 0:
                                    break
                            strSerialNumber = strSerialNumber + chr(per)
                    print ("user serial number: %s" % strSerialNumber)

    if sys.version >= '3':
            #nConnectionNum = input("please input the number of the device to connect:")
            nConnectionNum = 0
    else:
            nConnectionNum = raw_input("please input the number of the device to connect:")

    if int(nConnectionNum) >= deviceList.nDeviceNum:
            print ("intput error!")
            sys.exit()

    # ch:创建相机实例 | en:Creat Camera Object
    cam = MvCamera()
    print ("  starts   11")
    
    # ch:选择设备并创建句柄| en:Select device and create handle
    stDeviceList = cast(deviceList.pDeviceInfo[int(nConnectionNum)], POINTER(MV_CC_DEVICE_INFO)).contents

    ret = cam.MV_CC_CreateHandle(stDeviceList)
    if ret != 0:
            print ("create handle fail! ret[0x%x]" % ret)
            sys.exit()

    # ch:打开设备 | en:Open device
    ret = cam.MV_CC_OpenDevice(MV_ACCESS_Exclusive, 0)
    if ret != 0:
            print ("open device fail! ret[0x%x]" % ret)
            sys.exit()
    
    # ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
    if stDeviceList.nTLayerType == MV_GIGE_DEVICE:
            nPacketSize = cam.MV_CC_GetOptimalPacketSize()
            if int(nPacketSize) > 0:
                    ret = cam.MV_CC_SetIntValue("GevSCPSPacketSize",nPacketSize)
                    if ret != 0:
                            print ("Warning: Set Packet Size fail! ret[0x%x]" % ret)
            else:
                    print ("Warning: Get Packet Size fail! ret[0x%x]" % nPacketSize)

    # ch:设置触发模式为off | en:Set trigger mode as off
    ret = cam.MV_CC_SetEnumValue("TriggerMode", MV_TRIGGER_MODE_OFF)
    #ret = cam.MV_CC_SetEnumValue("TriggerMode", MV_TRIGGER_MODE_ON)
    if ret != 0:
            print ("set trigger mode fail! ret[0x%x]" %ret)
            sys.exit()
    # ch:获取数据包大小 | en:Get payload size
    stParam =  MVCC_INTVALUE()
    memset(byref(stParam), 0, sizeof(MVCC_INTVALUE))
    
    ret = cam.MV_CC_GetIntValue("PayloadSize", stParam)
    if ret != 0:
            print ("get payload size fail! ret[0x%x]" % ret)
            sys.exit()
    nPayloadSize = stParam.nCurValue
    print (" starts 2    ")

    # ch:开始取流 | en:Start grab image
    ret = cam.MV_CC_StartGrabbing()
    if ret != 0:
            print ("start grabbing fail! ret[0x%x]" % ret)
            sys.exit()

    data_buf = (c_ubyte * nPayloadSize)()
    print(data_buf)

    try:
            hThreadHandle = threading.Thread(target=work_thread, args=(cam, byref(data_buf), nPayloadSize))
            hThreadHandle.start()
    except:
            print ("error: unable to start thread")
            
    print ("press a key to stop grabbing.")
    #a = App()
    #time.sleep(2)
    #a.show()
    print ("  starts 3   ")
    #cv2.imshow("shared_Array_capture",b_arr)
    #cv2.waitKey(10)
    #sys.exit(app.exec_())
    press_any_key_exit()

    print ("  starts 4   ")
    g_bExit = True
    hThreadHandle.join()
    print ("  starts 5   ")
    shm_a.close()
    shm_a.unlink()

    # ch:停止取流 | en:Stop grab image
    ret = cam.MV_CC_StopGrabbing()
    if ret != 0:
            print ("stop grabbing fail! ret[0x%x]" % ret)
            del data_buf
            sys.exit()

    # ch:关闭设备 | Close device
    ret = cam.MV_CC_CloseDevice()
    if ret != 0:
            print ("close deivce fail! ret[0x%x]" % ret)
            del data_buf
            sys.exit()

    # ch:销毁句柄 | Destroy handle
    ret = cam.MV_CC_DestroyHandle()
    if ret != 0:
            print ("destroy handle fail! ret[0x%x]" % ret)
            del data_buf
            sys.exit()

    del data_buf
