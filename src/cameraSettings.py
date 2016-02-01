'''
cameraSettings.py
This file contains all the settings for the Raspberry Pi camera.
These should probably not be changed, since the software calibrates
to the camera anyway. Most of what this does is fix the resolution,
frame rate, and gain, which may be variable by default.

See the PiCamera python library for more details.

Created by Joe Hart
jhart12@umd.edu
'''
import time, select, sys
import numpy as np
import src.PreisachCode as P
def setCamera(camera,camXRes,camYRes):
#this function sets camera settings
 time.sleep(1)
 camera.start_preview()
 camera.resolution = (camXRes,camYRes)
 camera.awb_mode='off'
 camera.awb_gains=(3/2,3/2)
 camera.iso = 150
 camera.contrast = 50
 camera.drc_strength='off'
 camera.shutterspeed=10000
 camera.hflip=True
 camera.vflip=True
 camera.video_denoise=False
 camera.image_denoise=False
 while (camera.digital_gain<=0):
  time.sleep(0.1)
  if sys.stdin in select.select([sys.stdin],[],[],0)[0]:
   line=raw_input() 
   break
 camera.exposure_mode='off'
 camera.stop_preview()
 
