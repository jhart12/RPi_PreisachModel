//PMuserinput.i
//swig interface file for PreisachCode2.c
%module PMuserinput

%{
#define SWIG_FILE_WITH_INIT
#include <linux/fb.h>
#include "PreisachCode2.h"
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
 double refractoryTime, double probDown);
void set_alphabeta(int a0);
void allOFF();
void allON();
void checkerboard(int numXBlocks, int numYBlocks);
BOOL initialize();
void cleanup();
void invertBlocks(uint8 *input);
int Preisach_Main(int length,uint8 *input, double scalarInput);

