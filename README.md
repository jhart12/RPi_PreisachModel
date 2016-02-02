# RPi_PreisachModel

This python and c software performs the Preisach Model experiment with the Raspberry Pi B+, Raspberry Pi camera, and Texas Instruments DLP Lightcrafter 3000 projector. While some of the details software may be daunting, you don't need any knowledge of C and only minimal familiarity with the Raspberry Pi/Linux command line and Python. You should, however, have a basic understanding of the Preisach Model of Hysteresis. For good descriptions of the model, see our paper on the subject (soon the be published) or this website from University College, Cork http://euclid.ucc.ie/hysteresis/.

##Hardware
The hardware necessary to perform this experiment is all commercially available. One needs:

1. Raspberry Pi B+ (see https://www.raspberrypi.org/)

2. Raspberry Pi Camera Module (see https://www.raspberrypi.org/)

3. Texas Instruments DLP LightCrafter 3000 projector (see http://www.ti.com/tool/dlplightcrafter)

4. Another computer, either desktop or laptop works fine.

5. A ethernet cable to connect the computer to the Raspberry Pi.

####Basic setup
The basic experimental setup is the following. The LightCrafter consists of 608x684 individually addressible digital micromirrors. Each micromirror can be turned on or off. This is done by setting the Raspberry Pi video output to also have a resolution 608x684 pixels, and feeding the HDMI output of the Raspberry Pi to the HDMI input of the LightCrafter. The LightCrafter then projects the desired image on to a screen. This screen is imaged by the Raspberry Pi camera. This image is processed by the Raspberry Pi, the Preisach Model is iterated, and the next step is output by the Raspberry Pi to the LightCrafter. The results are visible on the projector screen and are also stored in memory on the Raspberry Pi.

In order to control the experiment through Raspberry Pi, we recommend that you SSH into the Raspberry Pi from a different computer. This allows you to view the Raspberry Pi command line on a traditional monitor instead of hurting your eyes trying to read small, low-resolution green print on a projector screen.

A more detailed description of the experiment will soon be provided. 

####Setting up the hardware
#####Raspberry Pi B+ running Raspbian Wheezy
######Framebuffer
This project relies on the Linux framebuffer on the Raspberry Pi. By interacting with the framebuffer, we can directly control the video output of the Raspberry Pi. In turn, this allows us to individually address the micromirrors on the LightCrafter, since each pixel on the Raspberry Pi will correspond to one micromirror on the LightCrafter. For more details on the frame buffer, see for example http://raspberrycompote.blogspot.com/2012/12/low-level-graphics-on-raspberry-pi-part_9509.html.

We want the Raspberry Pi to control each mirror on the LightCrafter individually. We must change the framebuffer settings to allow this. One can do this as follows:

1. Log onto you Raspberry Pi.
2. Navigate to the boot directory
```
cd /boot
```
Copy the config.txt file in case you want to go back to default settings later.
```
sudo cp config.txt config.normal
```
Now add the following lines to the config.txt file:
```
hdmi_cvt=608 684 60 3 0 0 0
disable_camera_led=1
disable_overscan=1
framebuffer_width=608
framebuffer_height=684
framebuffer_depth=24
hdmi_group=2
hdmi_mode=87
```

Reboot your Raspberry Pi so the changes can take effect.
```
sudo reboot
```

In order to return to default settings, simply save the new config file as something else and put the old config file back in place:
```
sudo cp config.txt config.lightcrafter
sudo cp config.normal config.txt
```

Then reboot.

######Raspberry Pi Camera Module
We use the Picamera python module to control the Raspberry Pi Camera Module. Instructions for installation and downloads can be found at https://www.raspberrypi.org/documentation/usage/camera/python/README.md.

######TI LightCrafter 3000
Use the TI LightCrafter GUI from the DLP LightCrafter Firmware and Software Bundle, available for free from TI at http://www.ti.com/tool/dlplightcrafter. 
See the DLP LightCrafter User's Guide from the same website for instructions on how to install the GUI and connect to your LightCrafter from your computer (not necessarily Raspberry Pi).

Once you are connected, we need to change the LightCrafter settings to allow us to control each micromirror individually from the Raspberry Pi HDMI output.
The settings that allow this are the following:
First, choose the Display Mode as HDMI Port
Then, in the HDMI tab, choose External Streaming Pattern Sequence. Change the LED Select to Green, Color Bit Depth to 8, and Frame Rate to 15 Hz.
On the top of the screen, you should change the LED Current to 50 mA for each of the colors so that your LEDs don't burn out. Also change the image orientation to East/West Flip (Short Axis).
Once you have done this, be sure to save these settings by going to the Solutions tab. Name it whatever you want, but be sure to make it the Default, so that everytime you restart your Lightcrafter, these settings will be automatically chosen.

##Software Dependencies
This software depends on you having the aforementioned hardware and Picamera module. It is also assumed you have python2.7 on your Raspberry Pi.

Additionally, you need the following free software:
####SWIG--Simplified Wrapper and Interface Generator
SWIG is a software development tool that connects programs written in C and C++ with a variety of high-level programming languages (for us, python). It allows us to use c code to do all the image processing much faster than we could do in python, and still let's us use the python Picamera module to control the camera.
You can read more about SWIG at http://www.swig.org. You should install this on your Raspberry Pi:
```
sudo apt-get install swig
```

####SSH client
You need a way to log into your Raspberry Pi from your computer so you can tell your Raspberry Pi what you want it to do via command line prompts. Alternatively, you could plug a USB keyboard directly into your Raspberry Pi; however, then you would have to try to read small green print on a projector screen, which is hard on the eyes and not recommended. The SSH client you should use depends on the OS of the computer you are using to communicate with the Raspberry Pi.

######Windows
Use PUTTY, as discussed here http://www.math.umn.edu/systems_guide/putty_xforwarding.shtml. In order for matplotlib to be able to display the results of your Preisach Model experiment on your non-Raspberry Pi Windows computer screen, you need to download and launch Xming on your Windows machine. For an explanation of how to do this for free, please see http://www.math.umn.edu/systems_guide/putty_xforwarding.shtml.

######Linux or Mac
Use SSH as described here https://www.raspberrypi.org/documentation/remote-access/ssh/unix.md.

##Install
Once you have all the other software you need, you are ready to download and install this software. First download:
```
git clone https://github.com/jhart12/RPi_PreisachModel.git
```

Once you have downloaded the library, you should navigate to the directory you just downloaded. For example, if you downloaded to your desktop, you would type

```
cd ~/Desktop/RPi_PreisachModel
```
Once in the RPi_PreisachModel directory, you need to compile the software. The library contains a Makefile which will do this for you if you simply type
```
make
```

Now you should be all installed. If you happen to make any changes to the software written in C, you can compile your changes by again typing "make". However, you shouldn't have to change any of the C code to perform the basic experiment. In fact, you don't need any understanding of the C language and minimal understanding of the Python language in order to perform this experiment.

##Use and Functionality
Because it seemed more user-friendly to me, I have written some Python scripts you can use to run two different versions of the Preisach Model. These scripts call functions that I wrote which control the camera and implement the Preisach Model. I won't describe all of these functions in detail here; however, in general they should be commented quite clearly so if you are interested, you can look them up.

I'll now describe the use and functionality of the three major scripts:
####PreisachModelTutorial.py
Once you have an understanding of the theory of the Preisach Model (for a detailed description, see our paper or http://euclid.ucc.ie/hysteresis/), this is the recommended script for familiarizing yourself with the Preisach Model experiment. Running this script requires no knowledge of Python.

Set up the experiment as described in our paper. Then navigate to the RPi_PreisachModel directory and type
```sudo python PreisachModelTutorial.py```
The "sudo" gives you administrator access, which is necessary to control the framebuffer. The "python" tells Linux that you want it to run a python script.

Once you have entered this, you will be prompted for the number of hysterons you'd like to use, up to a maximum of 465 (we found that the camera resolution causes problems for numbers larger than this). Enter a number. Since in our simple formulation, the number of hysterons is quantized [N=n(n+1)/2], the number of hysterons you input will be rounded to the nearest allowed value. The actual number of hysterons that will be used is output to the screen for your information.

You will then be asked to wait a few seconds while the camera is calibrated to the lighting in the room. The details of the lighting in the room is not important; however, it is important that the lighting remain constant once the camera is calibrated.

Once the camera is calibrated, the geometrical interpretation of the Preisach Model with an initial input of 1 will apper on your projector screen. You will be prompted to enter an input. As described in our paper, the model is normalized so that the inputs are in the interval [-1,1]. It's fine if you enter an input outside this range; however, the result will be the same as if you entered either 1 or -1 since the hysteresis will be saturated at input values outside this interval.

When you enter an input, the Raspberry Pi will update the Preisach Model. The updated state of the Preisach Model will appear on the screen. You can continue entering different input values as long as you like. You will notice that the output will depend not only on the input value you entered most recently, but also on the history of input values entered. When you are done, press "q" then "ENTER" to quit the script.

Again, this is just a toy script to help you understand how the Preisach Model experiment works. We recommend that you play with it a few times, using different numbers of hysterons and different patterns of inputs. Once you are confident that you understand both the theory and experiment, you are ready to move on to the next script, which controls the main experiment.

####PreisachModelMain.py
This is the script to run the main experiment. It allows the user a lot of freedom as to the inputs and parameters involved in the Preisach Model. To use this script, you do need to know a tiny amount of the Python language as you will want to edit the script to set different parameters, but it's really very simple.

To edit this script, open it in your favorite text editor. For example,
```nano PreisachModelMain.py```

You can now see the entire Python script. The first option you have is the input file: inputFilename. I have provided several different files, which are located in the inputfiles directory. Each file contains a function of time that you can use as the input to the Preisach Model. You can also make your own input function of time by using the writeInput.py script as described below.

The next option you have is the amount of time you'd like to run the experiment for, in seconds. This is pretty explanatory.

We then come to the Preisach Model Parameters. The first is n, the number of hysterons per row. As noted in the script, the total number of hysterons will be n(n+1)/2. The next option is the refractoryTime. This is the amount of time (in seconds) that a hysteron, once turned "off" will remain "off," regardless of the input. The final option is the downProbability. This is the probability that a hysteron that is "up" will randomly turn "down," even though the input itself would not be low enough to cause the hysteron to turn "down."

When you have decided the input function and model parameters you want to use, you should save your edits to the script and exit your text editor. Then type in the Linux command line
```
sudo python PreisachModelParameters.py
```
Again, the camera will be calibrated, then the experiment will run for the time you chose. Once it is finished, the hysteresis curve (output vs input) will be displayed on you computer screen. These results are also saved as a text file PMoutput.txt (format: Time, Input, Output) in the RPi_PreisachModel directory.

I'd like to point out that, as any computer controlled experiment, this experiment actually runs in discrete time. So if you choose a function of time that varies too quickly, you will notice discrete-time effects. If you want to approximate continuous time, you should choose slowly changing input functions.

####writeInput.py
This script allows the user to create any desired function of time as in input to the PreisachModelMain.py script.

First, open the script in your favorite text editor. Then edit the function f to be whatever function of time you desire. You will need to use correct Python syntax here, so you can always use a search engine to figure out the right syntax. When you are ready, save and exit. Then run the script
```python writeInput.py```
Once you have done this (and selected input.bin as the inputFilename in PreisachModelMain.py), you are ready to run the experiment:
```sudo python PreisachModelMain.py```
