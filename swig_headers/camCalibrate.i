//camCalibrate.i

//swig interface file for camCalibrate.c
%module camCalibrate

%{
#include <linux/fb.h>

typedef char BOOL;
#define FALSE 0
#define TRUE 1

typedef unsigned char uint8;

//*****GLOBAL VARIABLES*****
//framebuffer variables
int fbfd;//framebuffer location
int kbfd;
char *fbp;//pixel location in framebuffer
int page_size;
int cur_page;
long int screensize;
struct fb_var_screeninfo vinfo;
struct fb_var_screeninfo orig_vinfo;
struct fb_fix_screeninfo finfo;

//other globals
int leftoffset;
int topoffset;
int rightoffset;
int bottomoffset;
int hBlock;
int wBlock;
int camXRes;
int camYRes;
uint8 *threshold;

%}

typedef char BOOL;
#define FALSE 0
#define TRUE 1

typedef unsigned char uint8;

//*****GLOBAL VARIABLES*****
//framebuffer variables
int fbfd;//framebuffer location
int kbfd;
char *fbp;//pixel location in framebuffer
int page_size;
int cur_page;
long int screensize;
struct fb_var_screeninfo vinfo;
struct fb_var_screeninfo orig_vinfo;
struct fb_fix_screeninfo finfo;

//other globals
int leftoffset;
int topoffset;
int rightoffset;
int bottomoffset;
int hBlock;
int wBlock;
int camXRes;
int camYRes;
uint8 *threshold;

//*****FUNCTIONS*****
int getleftoffset();
int getrightoffset();
int gettopoffset();
int getbottomoffset();
void printthresholds(int maxBlocks);
void oneBlock(int blockNum, int maxBlocks);
void allON(int maxBlocks);
void checkerboard(int numXBlocks, int numYBlocks);
void findAvgIntensity(int xndx, int yndx, int blockNum);
void calibrateCamera(int thresh, char *imageFile, char 
*outFile,int maxBlocks);
void projectorON(int numXBlocks, int numYBlocks);
BOOL initialize(int cameraXRes, int cameraYRes, int maxBlocks);
void cleanup(char *thresholdFile,int maxBlocks);
