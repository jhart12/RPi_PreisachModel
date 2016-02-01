'''
writeInput.py
This function is used to create a function of time that is
fed into the Preisach Model experiment as the Input. The function
is formatted and written to a file, which is later read by the 
Preisach Model software. 
USE: Enter the desired function of time where indicated. Also enter the 
total time (in seconds) as well as the time step (also in seconds). 0.05 
seconds should be a sufficiently small time step, as the Raspberry Pi 
camera cannot take pictures faster than this. Once the desired function is
entered, save and run this python file BEFORE running the experiment.

Created by Joe Hart
jhart12@umd.edu
'''
import numpy as np
import math
import random
def f(time):
 w = 2*3.1415*0.0005
 w2 = -2*3.1415*.5

 #enter desired function here
 x = (math.cos(w2*time)*math.exp(-w*time)) 

 return x

T=100 #total time (seconds)
dT=0.05 #time step of input (seconds)

arr = np.zeros([T/dT,2],dtype=np.float64)#write as double
for i in range (int(T/dT)):
 arr[i][0]=i*dT
 arr[i][1]=f(i*dT)
 
F=open("inputfiles/input.bin","wb")
arr.tofile(F)
F.close()
