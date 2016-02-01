# -*- coding: utf-8 -*-
"""
main3.py
Main code for running the Preisach Model on the raspberry pi B+.
It is called by PreisachModelParameters.py.

Created by Joe Hart
jhart12@umd.edu
"""

import time, picamera, sys, select, io
import numpy as np
import src.PreisachCode as P
import src.cameraSettings as cameraSettings
import src.calibrate as cal


def main(N,refractoryTime,downProbability,inputFilename,threshold,T):
    #N is the number of hysterons in the top row
    #T is the amount of time to run for, in seconds
    a0=1 #maximum of alpha/beta in Preisach Model

    #camera information
    camXRes = 1312
    camYRes = 976

    #calibrate the camera to room lighting
    print "calibrating..."
    cal.calibrate(threshold,N)

    #initialization
    P.initializeGlobals(camXRes,camYRes,N,refractoryTime,downProbability, inputFilename)
    #initialize framebuffer
    P.initialize()
    P.checkerboard(N+1,N+1)

    P.set_alphabeta(a0)
    #initialize RPi camera
    cam=picamera.PiCamera()

    #import camera settings
    cameraSettings.setCamera(cam,camXRes,camYRes)
    time.sleep(1)
    #run main part of code
    print "Start run."
    P.allOFF()
    cam.capture_sequence(outputs(camXRes,camYRes,T), 'yuv', use_video_port=True)

    print "Press Enter to exit"
    while True:
      time.sleep(1)
      if sys.stdin in select.select([sys.stdin],[],[],0)[0]:
        line=raw_input()
        break

    
    P.cleanup()

def outputs(camXRes,camYRes,totalTime):
   framespersecond = 4
   RANGE = totalTime*framespersecond*5
   for i in range(RANGE):
       stream=io.BytesIO()
       # This returns the stream for the camera to capture to
       yield stream
       # Once the capture is complete, the loop continues here
       if ((i>10) and ((i%5)==1)):
       # This is where we process the image.
        stream.truncate(camXRes*camYRes)
       #convert stream into numpy array
        data=np.fromstring(stream.getvalue(),dtype=np.uint8)
       #iterate Preisach Model
        P.Preisach_Main(data)
       stream.close()
