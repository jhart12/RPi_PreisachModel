//globals.h
//global variables header file
//all global variables are stored in a single class
//for simplicity and readability

#ifndef GLOBALS_H
#define GLOBALS_H

#include <linux/fb.h>
#include <stdio.h>

#define BCM2708_ST_BASE 0x20003000 

//spi interface clock speed
#define SPI_SPEED 2500000

//projector resolution
#define XRES 608
#define YRES 684

#define GPIO_IN 0 //input comes from GPIO or internal file?

#ifdef DEFINEGLOBALSHERE
#define GLOBAL
#else
#define GLOBAL extern
#endif

typedef unsigned char uint8;

typedef char BOOL;
#define TRUE 1
#define FALSE 0

typedef struct
{
int fbfd;//framebuffer location
int kbfd;
char *fbp;//pixel location in framebuffer
int page_size;
int cur_page;
long int screensize;
struct fb_var_screeninfo vinfo;
struct fb_var_screeninfo orig_vinfo;
struct fb_fix_screeninfo finfo;
} Framebuffer;

typedef struct {
	char filename[40];
	int camXRes;
	int camYRes;
	double wBlock;//width of bool block in camera pixels
	double hBlock;
	//int numXBlocks;
	//int numYBlocks;
	int maxBlocks;//max number of blocks in a row
	int totalBlocks;
	int leftoffset;
	int rightoffset;
	int topoffset;
	int bottomoffset;
	uint8 *threshold;
} BBI; //boolean block image
typedef struct
{
double *alpha;//up values
double *beta;//down values
double *downTime;//amount of time since "going down"
long int refracTime;//refractory time in us
int pDown;
double input1;//current and previous inputs
double input2;
//timing variables
unsigned int initialTime;
unsigned int prevTime;
int inputLoc;
volatile unsigned *TIMER_registers;
FILE * outputfp;//where scalar PM output is stored as a function of time
int count;
char inputFilename[25];
} Model;


struct Globals
{
	Model model;
	BBI camImage;
	Framebuffer fb;
	int DAC_fd;
};

GLOBAL struct Globals globals;
#endif
