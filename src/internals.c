//internals.c
//source code for internal Preisach Code
#include "internals.h"

//*****timing code*****/
unsigned int TIMER_GetSysTick(){
   return (globals.model.TIMER_registers[1]);
}
void TIMER_Init(){
  // open /dev/mem
  int TIMER_memFd;
  if((TIMER_memFd = open("/dev/mem",O_RDWR)) < 0)
  {
	printf("Can't open /dev/mem/ -- need root?\n");
        exit(-1);
  }

  //mmap BCM System Timer
  void *TIMER_map = mmap(0,4096,PROT_READ,                 
      MAP_SHARED, 
      TIMER_memFd, 
      BCM2708_ST_BASE);

  close(TIMER_memFd);

  if (TIMER_map == MAP_FAILED)
  {
     printf("mmap error %d/n",(int)TIMER_map);
     exit(-1);
  }

  globals.model.TIMER_registers = (volatile unsigned *)TIMER_map;
}

//*****DAC Code*****/
void exit_on_error (const char *s)	// Exit and print error code
{ 	perror(s);
		abort();
} 

// turns scalar preisach output into DAC input
int calculate_voltage_output(double input) {
	//2048 = (2^12)/2 = (DAC digitization)/2
	return (2048 * (1 + input / globals.camImage.totalBlocks));
}

//input must be in range [-1,1]
int calculate_voltage_input(double input){
	return (2048 * (1 + input));

}
void setvoltage(int value2, int value1) {
	
	uint16_t tx;    	// RX buffer (16 bit unsigned integer)

	struct spi_ioc_transfer tr = 
	{	.tx_buf = (unsigned long)&tx,         
	.rx_buf = (unsigned long)NULL,        
	.len = 2,             
	.delay_usecs = 0,
	.speed_hz = SPI_SPEED,   
	.bits_per_word = 8,
	.cs_change = 0,
	};
	//write to DAC 2
	value2 = value2 | 0xF000;	
	tx = (value2 << 8) | (value2 >> 8);	// Adjust bits
	// Write data
	if (ioctl(globals.DAC_fd, SPI_IOC_MESSAGE(1), &tr) < 1) exit_on_error ("Can't send SPI message");

	//write to DAC 1
	value2 = (value2 & 0x0FFF) | 0x3000;
	tx = (value2 << 8) | (value2 >> 8);                     
	// Write data
	if (ioctl(globals.DAC_fd, SPI_IOC_MESSAGE(1), &tr) < 1) exit_on_error("Can't send SPI message");

}

//*****miscellaneous code*****/


//iterates Preisach model one time step, returns state in boolean array cur_state
//updates downTime[] and implements refractory time and random decay
//internal function
void Preisach(BOOL* cur_state, unsigned int currentTime){
	/*cur_state is an array holding the current state of each hysteron
	such that cur_state[x+y*xhysterons] gives the state (on or off) of the hysteron at x,y*/
	//currentTime should be in us
	int N = globals.camImage.totalBlocks;//total number of hysterons	
	int i;//index
	currentTime = currentTime - globals.model.prevTime;

	if (globals.model.input1>globals.model.input2){//if input is increasing
		for(i=0;i<N;i++){
			//update down time
			globals.model.downTime[i] += currentTime;
			if ((globals.model.alpha[i]<=globals.model.input1)&&(globals.model.downTime[i]>=globals.model.refracTime)){

				cur_state[i]=TRUE;//turn on
				}
			//randomly decay down
			if((rand() % 100)<globals.model.pDown){
				cur_state[i] = FALSE;
				globals.model.downTime[i] = 0;
			}
		}//end for loop
	}//end if
	else if (globals.model.input1<globals.model.input2){//if input decreases
		for(i=0;i<N;i++){
			globals.model.downTime[i] += currentTime;
			if (globals.model.beta[i]>globals.model.input1){
				cur_state[i]=FALSE;//turn off
				globals.model.downTime[i] = 0;
			}
			//randomly decay down
			if((rand() % 100)<globals.model.pDown){
				cur_state[i] = FALSE;
				globals.model.downTime[i] = 0;
			}
			//turn up if downTime long enough
			if((globals.model.alpha[i]<=globals.model.input1)&&(globals.model.downTime[i]>globals.model.refracTime)){
					cur_state[i]=TRUE;
			}
		}//end for loop
	}//end else if
	else if (globals.model.input1==globals.model.input2){//if input doesn't change
		for(i=0;i<N;i++){
			//update downTime
			globals.model.downTime[i] += currentTime;
			//randomly decay down
			if((rand() % 100)<globals.model.pDown){
				cur_state[i] = FALSE;
				globals.model.downTime[i] = 0;
			}
			//turn up if downTime is long enough
			if((globals.model.input1>=globals.model.alpha[i])&&(globals.model.downTime[i]>globals.model.refracTime))
				cur_state[i] = TRUE;
		}//end for loop
	}//end else if
}//end Preisach

//calculates scalar Preisach output
double calc_Preisach_output(BOOL *cur_state){
	int i;
	double out = 0.0;
	for(i=0;i<globals.camImage.totalBlocks;i++){
		if (cur_state[i]) {out +=1.0;}
		else {out -= 1.0;}}
	return out;
}

//reads input from file (or GPIO later)
//internal function
double readInput(unsigned int currTime){
	double dataPoint[2];
	//double dataPoint[1001];
	FILE * fptr;
	BOOL flag = TRUE;
	long count = 0;
	if(GPIO_IN){
		printf("ERROR: GPIO_IN feature not yet added.\n");}
	else {
		fptr = fopen(globals.model.inputFilename,"rb");
		//set location to start reading
		if(fseek(fptr, 2*sizeof(double)*globals.model.inputLoc, SEEK_SET)){
			//fseek returns 0 if successful
			printf("ERROR: fseek FAILED.");}
		//for loop until we find the right time location
		while(flag){
			if(fread(dataPoint, sizeof(double),2,fptr)!=2){
				//if end of input file, reset to beginning
				globals.model.initialTime=currTime;
				globals.model.inputLoc=0;
				fseek(fptr,0,SEEK_SET);
			}
			fread(dataPoint, sizeof(double),2,fptr);
			if((currTime-globals.model.initialTime)/1000000.0-dataPoint[0] <= 0)
				flag = FALSE;			
			count++;
		}//end while
		globals.model.inputLoc += count;
		//commented out on 6/18
		//if(fseek(fptr,sizeof(double)*globals.model.inputLoc, SEEK_SET))
			//printf("ERROR: fseek FAILED.");
		fclose(fptr);
		
	}//end else
	//only return input value
	return dataPoint[1];
}

//reads yuv camera data and turns into boolean array
//internal function
void yuv2bool(uint8* input, BOOL *output){//2/15/15
	//output should be (maxBlocks*maxBlocks+1)/2 array
	//only care about upper left triangle
	//changing to find average intensity of entire block
	int x,y,x0,y0,xndx,yndx,count,offset;
  long int sum;
  double denom = 1/(globals.camImage.hBlock*globals.camImage.wBlock);

  count=0;
  for(yndx=0;yndx<globals.camImage.maxBlocks;yndx++){
		y0=(int)(globals.camImage.topoffset+yndx*globals.camImage.hBlock);
		for(xndx=0;xndx<globals.camImage.maxBlocks-yndx;xndx++){
			x0=(int)(globals.camImage.leftoffset+xndx*globals.camImage.wBlock);
			sum = 0;
			for(y=y0;y<y0+globals.camImage.hBlock;y++){
				offset = y*globals.camImage.camXRes;
				for(x=x0;x<x0+globals.camImage.wBlock;x++){
					sum += input[x+offset];
				}//end x loop
			}//end y loop
  sum = sum * denom;
  
	if(sum < globals.camImage.threshold[count]){output[count]=FALSE;}
	else {output[count]=TRUE;}
  count++;

		}//end xndx loop
	}//end yndx loop
}
/*converts array of BOOLs representing each block to an array
 of BOOLs representing each projector pixel*/
//internal function
void bool2output(BOOL *input,BOOL *output){//2/15/15
	//converts totalBlocks input array to
	//an XRES*YRES output array
	int wBlock = XRES/(globals.camImage.maxBlocks+1);
	int hBlock = YRES/(globals.camImage.maxBlocks+1);
	int leftoverRows = YRES%(globals.camImage.maxBlocks+1);
	int leftoverCols = XRES%(globals.camImage.maxBlocks+1);
	int i,j,row,col,x,y,xndx,yndx,x0,y0,offset;
	int count = 0;
	for (y=0;y<YRES;y++){for(x=0;x<XRES;x++){
		output[x+y*XRES]=FALSE;}}
  for(yndx=0;yndx<globals.camImage.maxBlocks;yndx++){//cycle through rows of blocks
	y0=hBlock*yndx;
	for(xndx=0;xndx<globals.camImage.maxBlocks-yndx;xndx++){//cycle through columns of blocks
	x0=wBlock*xndx;
 	for(y=0;y<hBlock;y++){//cycle through rows of pixels
		//row=hBlock*yndx+y;
		offset = (y0+y)*XRES;
	for(x=0;x<wBlock;x++){//cycle through columns of pixels	
	output[x0+x+offset]=input[count];
	}}
		count++;
	}}//end x,y loops
	//end ndx loops

	//set extra rows of pixels on bottom to black	
for(j=0;j<leftoverRows;j++){for(i=0;i<XRES;i++){
  y=hBlock*(globals.camImage.maxBlocks+1)+j;
  x=i;
  output[y*XRES+x]=FALSE;}}
//set extra columns of pixels on right to black
for(i=0;i<leftoverCols;i++){for(j=0;j<YRES;j++){
	y = j;
	x = wBlock*(globals.camImage.maxBlocks+1)+i;
	output[y*XRES+x]=FALSE;}}
	
}
