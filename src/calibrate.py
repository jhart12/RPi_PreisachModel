'''
calibrate.py

Calibrates camera so that it should work in many different light settings,
so long as the ambient lighting is consistent throughout the experiment.
The experiment recalibrates upon each run.

Created by Joe Hart
jhart12@umd.edu
'''
import src.camCalibrate as cal
import picamera
import time
import cameraSettings as settings

def calibrate(threshold, N):

    #camera information
    camXRes = 1312
    camYRes = 976

    imageFilename = "helperfiles/image0.yuv"

    cal.initialize(camXRes, camYRes,N)
    cal.checkerboard(N+1,N+1)
    #cal.allON(maxBlocks)
    cam=picamera.PiCamera()
    settings.setCamera(cam,camXRes,camYRes)    
    cam.capture(imageFilename,'yuv',use_video_port=True)
        
    cal.calibrateCamera(threshold, imageFilename, 
                                "helperfiles/cameracalibrations.bin",N)
    #calibrate Intensity of each block
    cal.allON(N)
    time.sleep(1)
    cam.capture("helperfiles/Icalibrate.yuv",'yuv',use_video_port=True)
    blockNum = 0
    #cycle through each block
    for yndx in range (0,N):
        for xndx in range (0,N-yndx):
            #find and store the average intensity
            cal.findAvgIntensity(xndx,yndx,blockNum)
            blockNum = blockNum + 1
    cal.cleanup("helperfiles/cameracalibrations.bin",N)

    leftoffset=cal.getleftoffset()
    rightoffset=cal.getrightoffset()
    topoffset=cal.gettopoffset()
    bottomoffset=cal.getbottomoffset()
    #print "left offset = %d\n" % leftoffset
    #print "right offset = %d\n" % rightoffset
    #print "top offset = %d\n" % topoffset
    #print "bottom offset = %d\n" % bottomoffset
    cal.printthresholds(N)
    cam.close()
