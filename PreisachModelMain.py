# -*- coding: utf-8 -*-
"""
PreisachModelParameters.py
This allows the user to enter parameters and input functions for the 
Raspberry Pi Preisach Model Experiment.

Created on Fri Jun 19 14:59:50 2015

@author: Joe Hart
jhart12@umd.edu
"""

import src.main3 as main3
import pylab
import numpy as np
#inputFilename options:
#constant.bin is a constant input of 0.75 (100s)
#periodic.bin is a sinusoidal function (100s)
#doubleperiodic.bin is two beating sinusoidal functions (100s)
#input.bin is your own input, created by using writeInput.py
inputFilename = "inputfiles/input.bin"

T = 20 #amount of time you want to run for, in seconds

#model parameters
N=15 #number of hysterons = N*(N+1)/2
refractoryTime = 0.0 #refractory time in seconds
downProbability=0.0 #probability that an up hysteron will randomly turn down

#camera threshold (I recommend not playing with this too much)
threshold = 20 

#call to Preisach Model "main" file
main3.main(N,refractoryTime,downProbability,inputFilename,threshold,T)


data = np.loadtxt("PMoutput.txt",delimiter=',')
pylab.plot(data[:,1],data[:,2])
pylab.xlabel('Input')
pylab.ylabel('Output')
pylab.title('Preisach Model Hysteresis Curve')
pylab.show()
