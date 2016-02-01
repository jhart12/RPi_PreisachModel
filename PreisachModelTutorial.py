# -*- coding: utf-8 -*-
"""
PreisachModelTutorial.py
Allows user to enter input values by hand, and the Preisach Model is
implemented on command.

Created by Joe Hart
jhart12@umd.edu
"""
import src.PMuserinput as PMuserinput
import src.calibrate as cal
import src.cameraSettings as cameraSettings
import picamera
import numpy as np
import io, time
#max number of hysterons in a single row
#corresponds to a total of 30*31/2=465 hysterons
RANGE = 30
#camera threshold
threshold = 25
#camera resolution
camXRes = 1312
camYRes = 976
Nvec=[]
for i in range(RANGE):
    j=RANGE-i
    Nvec.append(j*(j+1)/2);
    
n = input("How many hysterons do you want to use? (No more than "+str(RANGE*(RANGE+1)/2)+") ")
#find the nearest allowed number of hysterons
for i in range(RANGE):
    if (n>=Nvec[i]):
        N=Nvec[i]
        break
print "Using %d hysterons.\n" %N
print "calibrating..."    
cal.calibrate(threshold,RANGE-i)
#initialize everything
PMuserinput.initializeGlobals(camXRes,camYRes, RANGE-i, 0,0)
PMuserinput.set_alphabeta(1)
PMuserinput.initialize()
#calibrate camera
cam = picamera.PiCamera()
PMuserinput.checkerboard(RANGE-i+1,RANGE-i+1)
cameraSettings.setCamera(cam,camXRes,camYRes)
time.sleep(1)
PMuserinput.allOFF()

stream=io.BytesIO()
cam.capture(stream,'yuv',use_video_port=True)
stream.truncate(camXRes*camYRes)
data=np.fromstring(stream.getvalue(),dtype=np.uint8)
stream.close()
PMuserinput.Preisach_Main(data,1)

#run Preisach Model, asking for user input at each time step
flag = True
while (flag):
    INPUT = raw_input("Enter the input, or enter q to quit: ")
    if (INPUT=="q"):
        flag = False
    else:
        INPUT = float(INPUT)
        stream=io.BytesIO()
        cam.capture(stream,'yuv',use_video_port=True)
        stream.truncate(camXRes*camYRes)
        data=np.fromstring(stream.getvalue(),dtype=np.uint8)
        stream.close()
        PMuserinput.Preisach_Main(data,INPUT)
    del INPUT    
PMuserinput.cleanup()
    
