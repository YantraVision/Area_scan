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
import numpy as np
from ctypes import *
from PIL import Image as im
import time
from PyQt5.QtWidgets import QWidget, QApplication, QLabel, QVBoxLayout
from PyQt5.QtGui import QPixmap
import sys
import cv2
from PyQt5.QtCore import pyqtSignal, pyqtSlot, Qt, QThread
import numpy as np
from PIL import Image
from multiprocessing import shared_memory


class VideoThread(QThread):
    change_pixmap_signal = pyqtSignal(np.ndarray)
    #print(change_pixmap_signal)
    #print(type(change_pixmap_signal))

    def run(self):
        # capture from web cam
        #cap = cv2.VideoCapture(0)
        #cap = cv2./home/vinoth/Pictures/Screenshot from 2023-05-19 18-46-50.png
        existing_shm = shared_memory.SharedMemory(name='test1')
        dataBufferFromCapture = np.ndarray((160000,), dtype=np.uint8, buffer=existing_shm.buf)
        #dataBufferFromCapture = np.array((921600,), dtype=np.uint8, buffer=existing_shm.buf)
        print(dataBufferFromCapture,"datafrmcapture")
        while True:
            dataBufferNpArray = np.array(dataBufferFromCapture)
            dataBufReshape = dataBufferNpArray.reshape(400, 400, 1)
            cv_img = Image.open("/home/vinoth/Pictures/Screenshot from 2023-05-19 18-46-50.png")
            nw_arry=np.array(cv_img)
            print(" shape of predefined ", nw_arry.shape)
            print(" shape of nparray ", dataBufferNpArray.shape)
            print(" shape ndarr ", dataBufferFromCapture.shape)
            print(" shape ndarr ", dataBufReshape.shape)
            self.change_pixmap_signal.emit(dataBufReshape)
            #self.change_pixmap_signal.emit(dataBufferFromCapture)
            #self.change_pixmap_signal.emit(nw_arry)
            print(" data .. ")
        existing_shm.close()


class App(QWidget):
    def __init__(self):
        super().__init__()
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
        self.thread.start()



    @pyqtSlot(np.ndarray)
    def update_image(self, cv_img):
        """Updates the image_label with a new opencv image"""
        qt_img = self.convert_cv_qt(cv_img)
        self.image_label.setPixmap(qt_img)
    
    def convert_cv_qt(self, cv_img):
        """Convert from an opencv image to QPixmap"""
        #print(cv_img)
        #print("convert")
        #print(type(cv_img))
        rgb_image = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
        h, w, ch = rgb_image.shape
        bytes_per_line = ch * w
        convert_to_Qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)
        p = convert_to_Qt_format.scaled(self.disply_width, self.display_height, Qt.KeepAspectRatio)
        return QPixmap.fromImage(p)
    
if __name__=="__main__":
    app = QApplication(sys.argv)
    a = App()
    a.show()
    sys.exit(app.exec_())
