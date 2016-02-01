//PreisachCode.i
//swig interface file for PreisachCode.c
%module PreisachCode

%{
#define SWIG_FILE_WITH_INIT
#include <linux/fb.h>
#include "PreisachCode.h"
#include "globals.h"
%}
%include "globals.h"
%include "numpy.i"

%init %{
import_array();
%}

typedef char BOOL;
#define FALSE 0
#define TRUE 1

typedef unsigned char uint8;

%apply (int DIM1, uint8* IN_ARRAY1){(int length, uint8 *input)};

void initializeGlobals(int camXRes, int camYRes, int maxBlocks, 
double refractoryTime, double probDown, char * inputFilename);
void set_alphabeta(int a0);
void allOFF();
void checkerboard(int numXBlocks, int numYBlocks);
BOOL initialize();
void cleanup();
void invertBlocks(uint8 *input);
int Preisach_Main(int length,uint8 *input);

