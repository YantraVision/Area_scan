from pypylon import pylon
from pypylon import genicam
import cv2
import numpy as np
import matplotlib.pyplot as plt
tl_factory = pylon.TlFactory.GetInstance()
#print(type(tl_factory))
devices = tl_factory.EnumerateDevices()
#print(type(devices), devices)
#for device in devices:
#    print(device.GetFriendlyName())
imageWindow = pylon.PylonImageWindow()
imageWindow.Create(1)
camera = pylon.InstantCamera()
camera.Attach(tl_factory.CreateFirstDevice())
camera.Open()
camera.PixelFormat.Value = "RGB8"
#camera.StartGrabbing(1)
camera.StartGrabbing(pylon.GrabStrategy_OneByOne)
i=1
while camera.IsGrabbing():
    i+=1
    grab = camera.RetrieveResult(2000, pylon.TimeoutHandling_Return)
    if grab.GrabSucceeded():
        #print(type(grab))
        img = grab.GetArray()
        print(img.shape)
        imageWindow.SetImage(grab)
        imageWindow.Show()
        #print(f'Size of image: {img.shape}')
        #print(camera.Gain.GetValue())
        #cv2.imshow("image",img)

    if i==100:
        break
camera.Close()
