//framebuffer.h
//header file for framebuffer functions
//these functions are not called by python main.py
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/ioctl.h>
#include "globals.h"

typedef unsigned char uint8;
typedef char BOOL;
#define TRUE 1
#define FALSE 0

void put_pixel(int x, int y, BOOL c);
void clear_screen(int c);
void draw(BOOL* output);
