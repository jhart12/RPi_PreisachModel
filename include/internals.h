//internals.h
//header file for internals.c
//holds functions for Preisach Model not called by python main.py

#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"


unsigned int TIMER_GetSysTick();
void TIMER_Init();
void exit_on_error (const char *s);
int calculate_voltage_output(double input);
int calculate_voltage_input(double input);
void setvoltage(int value2, int value1);
int max_array(int a[], int num_elements);
void Preisach(BOOL* cur_state, unsigned int currentTime);
double calc_Preisach_output(BOOL *cur_state);
double readInput(unsigned int currTime);
void yuv2bool(uint8* input,BOOL *output);
void bool2output(BOOL *input,BOOL *output);
