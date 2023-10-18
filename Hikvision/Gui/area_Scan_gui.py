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
from PIL import Image
from multiprocessing import shared_memory

class VideoThread(QThread):
    change_pixmap_signal = pyqtSignal(np.ndarray)

    def run(self):
        existing_shm = shared_memory.SharedMemory(name='test1')
        dataBufferFromCapture = np.ndarray((480000,), dtype=np.uint8, buffer=existing_shm.buf)
        #print(dataBufferFromCapture,"datafrmcapture")
        while True:
            dataBufferNpArray = np.array(dataBufferFromCapture)
            dataBufReshape = dataBufferNpArray.reshape(400, 400, 3)
            #sleep for 1000 ms
            time.sleep(1)
            self.change_pixmap_signal.emit(dataBufReshape)
            #print("updating",dataBufReshape)
            #cv_img = Image.open("/home/vinoth/Pictures/Screenshot from 2023-05-19 18-46-50.png")
            #nw_arry=np.array(cv_img)
            #self.change_pixmap_signal.emit(nw_arry)
        existing_shm.close()
        existing_shm.unlink()

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("Area_Scan_YV")
        MainWindow.resize(800, 600)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(40, 30, 400, 400))
        self.label.setObjectName("label")
        self.textBrowser = QtWidgets.QTextBrowser(self.centralwidget)
        self.textBrowser.setGeometry(QtCore.QRect(640, 100, 111, 61))
        self.textBrowser.setObjectName("textBrowser")
        self.pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(90, 500, 51, 31))
        self.pushButton.setObjectName("pushButton")
        self.pushButton_3 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_3.setGeometry(QtCore.QRect(500, 110, 111, 41))
        self.pushButton_3.setObjectName("pushButton_3")
        self.pushButton_2 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_2.setGeometry(QtCore.QRect(170, 500, 51, 31))
        self.pushButton_2.setObjectName("pushButton_2")
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 20))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.pushButton_3.clicked.connect(self.encoder)
        self.pushButton_2.clicked.connect(self.reverse)
        self.pushButton.clicked.connect(self.forward)
        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)
        self.thread = VideoThread()
        self.thread.change_pixmap_signal.connect(self.update_image)
        self.thread.start()
    
    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("AREA_SCAN_YV", "MainWindow"))
        self.label.setText(_translate("YV", "TextLabel"))
        self.pushButton.setText(_translate("MainWindow", "+"))
        self.pushButton_3.setText(_translate("MainWindow", "Encoder"))
        self.pushButton_2.setText(_translate("MainWindow", "-"))

    def encoder(self):
        print("started")
           
    def forward(self):
        print(" #### forward pressed ####") 
    


    def reverse(self):
        print("***** reverse pressed ******") 
    
    def update_image(self, cv_img):
        """Updates the image_label with a new opencv image"""
        qt_img = self.convert_cv_qt(cv_img)
        self.label.setPixmap(qt_img)
    
    def convert_cv_qt(self, cv_img):
        """Convert from an opencv image to QPixmap"""
        self.disply_width = 640
        self.display_height = 480
        rgb_image = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
        h, w, ch = rgb_image.shape
        bytes_per_line = ch * w
        convert_to_Qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)
        #convert_to_Qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_Indexed8)
        p = convert_to_Qt_format.scaled(self.disply_width, self.display_height, Qt.KeepAspectRatio)
        return QPixmap.fromImage(p)

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
