# RPi_PreisachModel

This python and c software performs the Preisach Model experiment with the Raspberry Pi B+, Raspberry Pi camera, and Texas Instruments DLP Lightcrafter 3000 projector.

##Hardware
The hardware necessary to perform this experiment is all commercially available. One needs:
1. Raspberry Pi B+ (see https://www.raspberrypi.org/)
2. Raspberry Pi Camera Module (see https://www.raspberrypi.org/)
3. Texas Instruments DLP LightCrafter 3000 projector (see http://www.ti.com/tool/dlplightcrafter)
4. Another computer, either desktop or laptop works fine.
5. A ethernet cable to connect the computer to the Raspberry Pi.

####Setting up the hardware
#####Raspberry Pi B+ running Raspbian Wheezy
######Framebuffer
This project relies on the Linux framebuffer. For more details on the frame buffer, see for example http://raspberrycompote.blogspot.com/2012/12/low-level-graphics-on-raspberry-pi-part_9509.html.

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

####Xming
######Only if your non-Raspberry Pi computer runs Windows
In order for matplotlib to be able to display the results of your Preisach Model experiment on your non-Raspberry Pi Windows computer screen, you need to download and launch Xming on your windows machine. For an explanation of how to do this for free, please see http://www.math.umn.edu/systems_guide/putty_xforwarding.shtml.

####SSH client
You need a way to log into your Raspberry Pi from your computer so you can tell your Raspberry Pi what you want it to do via command line prompts.

######Windows
Use PUTTY, as discussed here http://www.math.umn.edu/systems_guide/putty_xforwarding.shtml.

######Linux or Mac
Use SSH as described here https://www.raspberrypi.org/documentation/remote-access/ssh/unix.md.

##Install
