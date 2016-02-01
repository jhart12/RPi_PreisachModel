//PreisachAll.c
//c extension to write images to framebuffer from Python
//for use with PreisachCodeMain.py
#define DEFINEGLOBALSHERE
#include "PreisachCode.h"
#include "globals.h"

//*****start c code*****/
//initializes the structure globals
//called by python
//refractoryTime is in seconds
//probDown is a decimal between 0 and 1
void initializeGlobals(int camXRes, int camYRes, int maxBlocks, double refractoryTime, double probDown, char *inputFilename){
	FILE *fp;
	int offsetARRAY[4];
	double sizeARRAY[2];
	globals.model.count=-1;
	//initialize camera information
	globals.camImage.camXRes = camXRes;
	globals.camImage.camYRes = camYRes;
	globals.camImage.maxBlocks = maxBlocks;//number of blocks used in the longest row
	globals.camImage.totalBlocks = (maxBlocks*(maxBlocks+1))/2;
	globals.camImage.threshold = (uint8 *)malloc(sizeof(uint8)*(maxBlocks*(maxBlocks+1))/2);

	fp = fopen("helperfiles/cameracalibrations.bin","rb");
	fread(offsetARRAY,sizeof(int),4,fp);
	fread(sizeARRAY,sizeof(double),2,fp);
	fread(globals.camImage.threshold,sizeof(uint8),maxBlocks*(maxBlocks+1)/2,fp);
	fclose(fp);

	globals.camImage.leftoffset = offsetARRAY[0];
	globals.camImage.rightoffset = offsetARRAY[1];
	globals.camImage.topoffset = offsetARRAY[2];
	globals.camImage.bottomoffset = offsetARRAY[3];

	globals.camImage.wBlock = sizeARRAY[0];
	globals.camImage.hBlock = sizeARRAY[1];
	//initialize framebuffer information
	globals.fb.fbfd = 0;//framebuffer location
	globals.fb.kbfd = 0;
	(globals.fb.fbp) = 0;//pixel location in framebuffer
	globals.fb.page_size = 0;
	globals.fb.cur_page = 0;
	globals.fb.screensize = 0;
	//initialize model variables
	globals.model.initialTime = 0;//for determining time for input from file
	globals.model.inputLoc = 0; //stores position of current input (when input is from file)
	globals.model.refracTime = refractoryTime*1000000.0;//convert to us
	globals.model.pDown = 100 * probDown;//convert to a number between 0 and 100
	memset(globals.model.inputFilename,'\0',sizeof(memset));
	strcpy(globals.model.inputFilename,inputFilename);

}




//determines alpha and beta values for Preisach Model
//called by python
void set_alphabeta(int a0){
	//convention: 0 is always at top left(so largest alpha, smallest beta)
	//a0 = maximum value of alpha/beta
	
	double d = 2.0*a0/(globals.camImage.maxBlocks+1);//vertical distance in alpha-beta space between hysterons
	double alpha0 = a0 - .5 *d;//initial alpha value
	double beta0 = -a0 + 0.5*d;//initial beta value
    // alpha[0]=alpha0;//store initial alpha and beta values
	//beta[0]=beta0;
	int i,j;
	double x = beta0-d;//beta value of the previous hysteron
	double y = alpha0;//alpha value of the previous hysteron
	int count = 0;
	//allocate alpha and beta arrays
	globals.model.alpha = (double *)malloc(sizeof(double)*globals.camImage.totalBlocks);
	globals.model.beta = (double *)malloc(sizeof(double)*globals.camImage.totalBlocks);
	globals.model.downTime = (double *)malloc(sizeof(double)*globals.camImage.totalBlocks);
	//initialize s.t. all can immediately turn up
	for (i=0;i<globals.camImage.totalBlocks;i++){
		globals.model.downTime[i] = globals.model.refracTime;
	}

	for(i=0;i<globals.camImage.maxBlocks;i++){//loop over alphas

		for(j=0;j<=globals.camImage.maxBlocks-i-1;j++){//loop over betas

			globals.model.alpha[count] = y;//set alpha			
			//if (abs(alpha[count]) < 0.0000000001) {alpha[count] = 0.0;}//fix rounding errors
			x += d;
			globals.model.beta[count] = x;//set beta
			//if (abs(beta[count]) < .0000000001) {beta[count] = 0.0;}//fix rounding errors
			count++;
		}//end beta loop
	    x = beta0 - d;
		y = y - d;//reset 
	}//end alpha loop
}


//*****framebuffer c code*****/
void allOFF(){
int xres = 608;
int yres = 684;
BOOL out[xres*yres];
int i;
for(i=0;i<xres*yres;i++)
	out[i]=FALSE;
draw(out);
}


//writes a checkerboard image to the screen (for testing/calibration)
//called by python
void checkerboard(int numXBlocks, int numYBlocks){
	//xres, yres are resolutions of framebuffer (608x684 for LCr)
	//xblocksize, yblocksize are size of blocks in screen pixels
	//out is output matrix
BOOL out[XRES*YRES];    
int xndx,yndx,x,y,sum,col,row;  
 //this is where we manipulate the image data
    int xblocksize = XRES/numXBlocks; 
	int yblocksize = YRES/numYBlocks;
	int leftoverRows=YRES%numYBlocks;
	int leftoverCols=XRES%numXBlocks;
    for(yndx=0;yndx<numYBlocks;yndx++){//cycle through rows of blocks
    for(xndx=0;xndx<numXBlocks;xndx++){//cycle through columns of blocks
	
	for(y=0;y<yblocksize;y++){//cycle through rows of pixels
	for(x=0;x<xblocksize;x++){//cycle through columns of pixels
	row=yblocksize*yndx+y;
	col=xblocksize*xndx+x;
	if 
((col/xblocksize%2)^(row/yblocksize%2)){out[row*XRES+col]=FALSE;}
	else {out[row*XRES+col]=TRUE;}
	}}//end x,y loops
    }}//end ndx loops

//set extra rows of pixels on bottom to black	
for(y=0;y<leftoverRows;y++){for(x=0;x<XRES;x++){
  row=yblocksize*yndx+y;
  col=x;
  out[row*XRES+col]=FALSE;}}
//set extra columns of pixels on right to black
for(x=0;x<leftoverCols;x++){for(y=0;y<YRES;y++){
	row = y;
	col = xblocksize*xndx+x;
	out[row*XRES+col]=FALSE;}}
//draw checkerboard to screen
draw(out);

}

//open the framebuffer, get fb info, set variable fb info
//called by python
BOOL initialize(){
    globals.fb.fbfd = open("/dev/fb0", O_RDWR);
    if (!globals.fb.fbfd) {
      printf("Error: cannot open framebuffer device.\n");
	  return FALSE;
    }
    char *kbfds = "/dev/tty";
    globals.fb.kbfd = open(kbfds, O_WRONLY);
    if (globals.fb.kbfd >= 0) {
        ioctl(globals.fb.kbfd, KDSETMODE, KD_GRAPHICS);
    }
    else {
        printf("Could not open %s.\n", kbfds);
		return FALSE;
    }


    if (ioctl(globals.fb.fbfd, FBIOGET_VSCREENINFO, &globals.fb.vinfo)) {
      printf("Error reading variable information.\n");
	  return FALSE;
    }
    if (ioctl(globals.fb.fbfd, FBIOGET_VSCREENINFO, &globals.fb.orig_vinfo)) {
      printf("Error reading variable information.\n");
	  return FALSE;
    }

	//set vscreeninfo to what we want
    globals.fb.vinfo.bits_per_pixel = 24; //RGB888
    globals.fb.vinfo.xres = XRES;
    globals.fb.vinfo.yres = YRES;
    globals.fb.vinfo.xres_virtual = globals.fb.vinfo.xres;
    globals.fb.vinfo.yres_virtual = globals.fb.vinfo.yres * 2;//double buffering
   
    if (ioctl(globals.fb.fbfd, FBIOPUT_VSCREENINFO, &globals.fb.vinfo)) {
      printf("Error setting variable information.\n");
	  return FALSE;
    }

    if (ioctl(globals.fb.fbfd, FBIOGET_FSCREENINFO, &globals.fb.finfo)) {
      printf("Error reading fixed information.\n");
	  return FALSE;
    }
    globals.fb.screensize = globals.fb.finfo.smem_len;
    globals.fb.page_size = globals.fb.finfo.line_length * globals.fb.vinfo.yres;
	//set framebufferpointer
    globals.fb.fbp = (char*)mmap(0,globals.fb.screensize,PROT_READ | PROT_WRITE, MAP_SHARED,globals.fb.fbfd,0);

    //open file to write output in binary
    globals.model.outputfp = fopen("PMoutput.txt","w");

	//initialize timer
	TIMER_Init();

	//initialize DAC
	uint8_t mode = SPI_MODE_0;
	uint32_t speed = SPI_SPEED;
	// Open SPI device
	if ((globals.DAC_fd = open("/dev/spidev0.1", O_RDWR)) < 0)
		exit_on_error ("Can't open SPI device");
	if (ioctl(globals.DAC_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) exit_on_error ("Can't set Max Speed");
	// Set SPI mode
	if (ioctl(globals.DAC_fd, SPI_IOC_WR_MODE, &mode) == -1) exit_on_error ("Can't set SPI mode");

    
    return TRUE;
    
}

//return to terminal framebuffer
//called by python
void cleanup(){

    munmap(globals.fb.fbp, globals.fb.screensize);

    if (ioctl(globals.fb.fbfd, FBIOPUT_VSCREENINFO, &globals.fb.orig_vinfo)) {
        printf("Error re-setting variable information.\n");
    }

    if (globals.fb.kbfd >= 0) {
        ioctl(globals.fb.kbfd, KDSETMODE, KD_TEXT);
        close(globals.fb.kbfd);
    }
    close(globals.fb.fbfd);
	//close output file
    fclose(globals.model.outputfp);

	//close DAC
	close(globals.DAC_fd);
}


//practice function that just switches the state of each block
//for troubleshooting/testing
//called by python
void invertBlocks(uint8 *input){
	BOOL *output;
	BOOL *out2screen;
	output = (BOOL *)malloc(sizeof(BOOL)*globals.camImage.totalBlocks);
	out2screen = (BOOL *)malloc(sizeof(BOOL)*XRES*YRES);
	yuv2bool(output, input);

	int i;
	for(i=0;i<globals.camImage.totalBlocks;i++){
		if(output[i])output[i]=FALSE;
		else output[i]=TRUE;}
	bool2output(output,out2screen);
	//output to screen
	draw(out2screen);
	free(output);
	free(out2screen);
}

//main function that operates all internal programming
int Preisach_Main(int length, uint8 * input){
	//a0 is maximum value of alpha and beta
	//threshold is intensity threshold
	int i;
	FILE * fout;
	//char filename[14]="camInput";
	//char integer[2];
	unsigned int currTime, t0,tf;
	BOOL *currentState;
	BOOL *Soutput; //screen output
	double OUTARRAY[3];//for printing output
        Soutput=(BOOL *)malloc(sizeof(BOOL)*XRES*YRES);
	currentState=(BOOL *)malloc(globals.camImage.totalBlocks*sizeof(BOOL));
	//globals.model.count=globals.model.count+1;
	//sprintf(integer,"%d",globals.model.count);
	//strcat(filename,integer);
	//strcat(filename,".csv");
	//fout=fopen(filename,"w");
	//for(i=0;i<1312*976;i++){
	//fprintf(fout,"%d,",input[i]);
	//}
	//fprintf(fout,"\n");
	//fclose(fout);
	//convert image  to boolean array
	yuv2bool(input,currentState);
//	fout=fopen("yuv2booltest.csv","a");
//	for(i=0;i<globals.camImage.totalBlocks;i++){
//	fprintf(fout,"%d,",currentState[i]);
//	}
//	fprintf(fout,"\n");
//	fclose(fout);
		//get current time
	if(globals.model.initialTime)//if not first iteration
		currTime=TIMER_GetSysTick();
	else {
		globals.model.initialTime=TIMER_GetSysTick();
		currTime=globals.model.initialTime;
		globals.model.input2=0;//assume past input is zero
	}

	//get input corresponding to current time
	globals.model.input2=globals.model.input1;//input2 is oldinput
	globals.model.input1=readInput(currTime);//input1 is new input
    //iterate Preisach Model
	Preisach(currentState, currTime);

//	fout=fopen("output.csv","a");
//	for(i=0;i<globals.camImage.totalBlocks;i++){
//	fprintf(fout,"%d,",currentState[i]);
//	}
//	fprintf(fout,"\n");
//	fclose(fout);

	globals.model.prevTime = currTime;

	//calculate Preisach output
	OUTARRAY[0]=(currTime - globals.model.initialTime)/1000000.0;//save time in seconds
	//OUTARRAY[0]=(double)(globals.model.inputLoc-1);//save time step
	OUTARRAY[1]=globals.model.input1;//save input
	OUTARRAY[2]=calc_Preisach_output(currentState);//save output
	
	fprintf(globals.model.outputfp,"%f,%f,%f\n",OUTARRAY[0],OUTARRAY[1],OUTARRAY[2]);

	//write input and output to DAC
	setvoltage(calculate_voltage_input(OUTARRAY[1]),calculate_voltage_output(OUTARRAY[2]));
	//output to screen
	bool2output(currentState,Soutput);
	
	//draw to framebuffer
	draw(Soutput);
  
	free(Soutput);
	free(currentState);

	return 0;
}
