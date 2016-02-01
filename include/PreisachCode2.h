#ifndef PREISACHCODE_H
#define PREISACHCODE_H
//PreisachAll.h
//header for PreisachCode.c


#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/ioctl.h>
#include <math.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "internals.h"

//*****start c code*****/
//writes an file to be used as input for the Preisach Model
//initializes global variables
void initializeGlobals(int camXRes, int camYRes, int maxBlocks,
 double refractoryTime, double probDown);

//determines alpha and beta values for Preisach Model
void set_alphabeta(int a0);

//turns all micromirrors to the off position
void allOFF();

//writes a checkerboard image to the screen (for testing/calibration)
void checkerboard(int numXBlocks, int numYBlocks);

//open the framebuffer, get fb info, set variable fb info
BOOL initialize();

//return to terminal framebuffer
void cleanup();

//practice function that just switches the state of each block
//for troubleshooting/testing
void invertBlocks(uint8 *input);

//main function that controls all internal programming
int Preisach_Main(int length,uint8 *input, double scalarInput);

#endif
