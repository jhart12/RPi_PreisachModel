//c source file for camera calibration

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/ioctl.h>
#include <linux/kd.h>
#include <fcntl.h>

#define XRES 608
#define YRES 684

typedef unsigned char uint8;

typedef char BOOL;
#define TRUE 1
#define FALSE 0

//*****GLOBAL VARIABLES*****/
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

int getleftoffset(){
	return leftoffset;}
int getrightoffset(){
	return rightoffset;}
int gettopoffset(){
	return topoffset;}
int getbottomoffset(){
	return bottomoffset;}
void printthresholds(int maxBlocks){
	int i;
	FILE *fp;
	fp=fopen("helperfiles/thresholds.txt","w");
	for(i=0;i<maxBlocks*(maxBlocks+1)/2;i++)
			fprintf(fp,"threshold[%d] = %d\n",i,threshold[i]);
	fclose(fp);
}


void put_pixel(int x, int y, BOOL c)
{
//r and g are on because they correspont to the first half-frame and
//second half frame, respectively
//b doesn't correspond to anything in the LCr external pattern sequence mode
    //if c=1, display an "on" pixel, otherwise put an "off" pixel 
    unsigned int pix_offset = 3 * x + y * finfo.line_length;

    pix_offset += cur_page * page_size;

    if(c){
    *((char*)(fbp + pix_offset)) = 0;//b
    *((char*)(fbp + pix_offset+1)) = 255;//g
    *((char*)(fbp + pix_offset+2)) = 255;//r
    }
    else{
    *((char*)(fbp + pix_offset)) = 0;//r
    *((char*)(fbp + pix_offset+1)) = 0;//g
    *((char*)(fbp + pix_offset+2)) = 0;//b
    }	

}

//internal function
void clear_screen(int c) {
    memset(fbp + cur_page * page_size, c, page_size);
}

/*draws the output to the framebuffer and handles the double 
buffering*/
//internal function
void draw(BOOL* output) {
//draws the output to the framebuffer and handles the double 
//buffering
    int x, y;
    
    cur_page = (cur_page + 1) % 2;

    clear_screen(0);


    for (x=0;x<vinfo.xres;x++){
    for (y=0;y<vinfo.yres;y++){

      put_pixel(x,y,output[vinfo.xres*y+x]);}}


    vinfo.yoffset = cur_page * vinfo.yres;
    ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo);
    ioctl(fbfd, FBIO_WAITFORVSYNC, 0);
}


void bool2output(BOOL *input,BOOL *output,int maxBlocks){//2/15/15
	//converts totalBlocks input array to
	//an XRES*YRES output array
	int wBlock = XRES/(maxBlocks+1);
	int hBlock = YRES/(maxBlocks+1);
	int leftoverRows = YRES%(maxBlocks+1);
	int leftoverCols = XRES%(maxBlocks+1);
	int i,j,row,col,x,y,xndx,yndx,pixNDX;
	int count = 0;
	for (y=0;y<YRES;y++){for(x=0;x<XRES;x++){
		output[x+y*XRES]=FALSE;}}
    for(yndx=0;yndx<maxBlocks;yndx++){//cycle through rows of blocks
    for(xndx=0;xndx<maxBlocks-yndx;xndx++){//cycle through columns of blocks
	
 	for(y=0;y<hBlock;y++){//cycle through rows of pixels
		row=hBlock*yndx+y;
	for(x=0;x<wBlock;x++){//cycle through columns of pixels	
	col=wBlock*xndx+x;
	pixNDX=col+row*XRES;
	output[pixNDX]=input[count];
	}}
		count++;
	}}//end x,y loops
	//end ndx loops

	//set extra rows of pixels on bottom to black	
for(j=0;j<leftoverRows;j++){for(i=0;i<XRES;i++){
  y=hBlock*(maxBlocks+1)+j;
  x=i;
  pixNDX=y*XRES+x;
  output[pixNDX]=FALSE;}}
//set extra columns of pixels on right to black
for(i=0;i<leftoverCols;i++){for(j=0;j<YRES;j++){
	y = j;
	x = wBlock*(maxBlocks+1)+i;
	output[y*XRES+x]=FALSE;}}
	
}

//writes a single block to the screen
//called by python
void oneBlock(int blockNum, int maxBlocks){
	int i;
	BOOL * boolOutput;
	BOOL * screenOutput;
  int totalBlocks = (maxBlocks*(maxBlocks+1))/2;
	boolOutput = (BOOL *)malloc(sizeof(BOOL)*totalBlocks);
	screenOutput = (BOOL *)malloc(sizeof(BOOL)*XRES*YRES);
for(i=0;i<totalBlocks;i++){
		if(i==blockNum)
			boolOutput[i]=TRUE;
		else
			boolOutput[i]=FALSE;
	}

	bool2output(boolOutput,screenOutput,maxBlocks);
	draw(screenOutput);

	free(boolOutput);
	free(screenOutput);
}
void allON(int maxBlocks){
	BOOL out[maxBlocks*(maxBlocks+1)/2];
	int i;
	BOOL *screenOutput;
	screenOutput = (BOOL *)malloc(sizeof(BOOL)*XRES*YRES);
	for (i=0;i<maxBlocks*(maxBlocks+1)/2;i++)
		out[i]=TRUE;

	bool2output(out,screenOutput,maxBlocks);
	draw(screenOutput);
	free(screenOutput);
}
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


//internal function to find the largest element in an array
int max_array(int a[], int num_elements)
{
   int i, max=-32000;
   for (i=0; i<num_elements; i++)
   {
	 if (a[i]>max)
	 {
	    max=a[i];
	 }
   }
   return(max);
}


//finds average intensity of each "block" and stores in
//a global variable
//called by python before running Preisach Model
void findAvgIntensity(int xndx, int yndx, int blockNum){
	int x,y,x0,y0,pixNDX;
	int sum=0;
	uint8 *data;
	FILE *fid;
	data = (uint8 *)malloc(sizeof(uint8)*camXRes*camYRes);
	//read in image file
	fid=fopen("helperfiles/Icalibrate.yuv","rb");
	fread(data,sizeof(uint8),camXRes*camYRes,fid);
	fclose(fid);
	x0 = leftoffset + xndx * wBlock;
	y0 = topoffset + yndx * hBlock;
	//cycle through each pixel of the block
	for(y=y0;y<y0+hBlock;y++){
		for(x=x0;x<x0+wBlock;x++){
			pixNDX = x + y * camXRes;
			sum += data[pixNDX];
		}//end x loop
	}//end y loop
	//get average intensity
	sum = sum / (int)(hBlock * wBlock);
	threshold[blockNum] = sum*.60;//threshold is 80 percent of average
	free(data);
}

//calibrates the camera to the projected image
//called by python
void calibrateCamera(int thresh, char *imageFile, char 
*outFile, int maxBlocks){
	//thresh is the y (intensity) value for which a pixel is considered on
	//uint8 threshold = 30;//30 works well with lights off
	int i,k,l,pixPosition;
	int j = 0;
	int dobreak = 0;//boolean
	int sum = 0;
	int ARR[4], ARR2[4], ARR3[4], ARR4[4];
	int OUTARRAY1[4];
	double OUTARRAY2[2];
	FILE * fptr;
	uint8 *Y;//stores y part of yuv data
	Y = (uint8 *)malloc(sizeof(uint8)*(camXRes)*(camYRes));
	fptr = fopen(imageFile,"rb");
	fread(Y,sizeof(uint8),(camXRes)*(camYRes),fptr);	
	fclose(fptr);
		
	//find first bright pixel (in upper left corner)
	while(j<camYRes-10){//for(j=0;j<globals.camImage.camYRes;j++)
	for(i=0;i<camXRes-20;i++){
		pixPosition = i+j* camXRes;
		if (Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(k=0;k<10;k++){//check to make sure next 10x10 are bright
				for(l=0;l<10;l++){
					sum += Y[pixPosition+l+k*( camXRes)];}}
			if (sum>=thresh*100){//if it is the top left on pixel
				dobreak = 1;
				break;}//break while loop
		}//end if

	}
	if (dobreak)
		break;j++;
	}//end i and j for loops

	ARR[0]=-1*j;//top
	ARR4[0]=-1*i;//left
	dobreak = 0;
	j =  camYRes - 2;
	
	//find last bright pixel (in lower right corner)
	while(j>10){
	for(i= camXRes-20;i>10;i--){
		pixPosition = i+j* camXRes;
		if(Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(k=0;k>-10;k--){
				for(l=0;l>-10;l--){sum+=Y[pixPosition+l+k*( camXRes)];}}
			if(sum>=thresh*100){//if it is the bottom right pixel
				dobreak = 1;
					break;}//break while loop
			}//end if

	}
			if (dobreak)
			break;
			j--;
	}//end i and j loops
	ARR3[0]=j;//bottom
	ARR2[0]=i;//right


		//find last bright pixel (in bottom left corner)
	dobreak = 0;
	j =  camYRes - 2;
	
	while(j>10){
	for(i=0;i< camXRes-20;i++){
		pixPosition = i+j* camXRes;
		if(Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(k=0;k>-10;k--){
				for(l=0;l<10;l++){sum+=Y[pixPosition+l+k*( camXRes)];}}
			if(sum>=thresh*100){//if it is the bottom right pixel
				dobreak = 1;
					break;}//break while loop
			}//end if

	}
			if (dobreak)
			break;
			j--;}//end i and j loops
	ARR3[1]=j;//bottom
	ARR4[1]=-1*i;//left
	dobreak=0;
	j=0;
	
	//top right
	while(j< camYRes-10){//for(j=0;j< camYRes;j++)
	for(i= camXRes-20;i>10;i--){
		pixPosition = i+j* camXRes;
		if (Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(k=0;k<10;k++){//check to make sure next 10x10 are bright
				for(l=0;l>-10;l--){
					sum += Y[pixPosition+l+k*( camXRes)];}}
			if (sum>=thresh*100){//if it is the top left on pixel
				dobreak = 1;
				break;}//break while loop
		}//end if

	}
			if (dobreak)
			break;j++;}//end i and j for loops
	ARR[1] = -1*j;//top
	ARR2[1] = i;//right

	//go vertical first now
	dobreak=0;
	i=0;
	//bottom left
	while(i< camXRes-20){//for(j=0;j< camYRes;j++)
	for(j= camYRes-1;j>10;j--){
		pixPosition = i+j* camXRes;
		if (Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(l=0;l<10;l++){//check to make sure next 10x10 are bright
				for(k=0;k>-10;k--){
					sum += Y[pixPosition+l+k*( camXRes)];}}
			if (sum>=thresh*100){//if it is the top left on pixel
				dobreak = 1;
				break;}//break while loop
		}//end if

	}
			if (dobreak)
			break;i++;}//end i and j for loops

	ARR3[2]=j;//bottom
	ARR4[2]=-1*i;//left

			//find last bright pixel (in top right corner)
	dobreak = 0;
	i =  camXRes - 20;
	
	while(i>10){
	for(j=0;j< camYRes-10;j++){
		pixPosition = i+j* camXRes;
		if(Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(l=0;l>-10;l--){
				for(k=0;k<10;k++){sum+=Y[pixPosition+l+k*( camXRes)];}}
			if(sum>=thresh*100){//if it is the bottom right pixel
				dobreak = 1;
					break;}//break while loop
			}//end if

	}
			if (dobreak)
			break;
			i--;}//end i and j loops
	ARR2[2]=i;//right
	ARR[2]=-1*j;//top

	dobreak=0;
	i=0;
	//find first bright pixel (in top left corner)
	while(i< camXRes-20){//for(j=0;j< camYRes;j++)
	for(j=0;j< camYRes-10;j++){
		pixPosition = i+j* camXRes;
		if (Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(l=0;l<10;l++){//check to make sure next 10x10 are bright
				for(k=0;k<10;k++){
					sum += Y[pixPosition+l+k*( camXRes)];}}
			if (sum>=thresh*100){//if it is the top left on pixel
				dobreak = 1;
				break;}//break while loop
		}//end if
	}
			if (dobreak)
			break;i++;}//end i and j for loops

	ARR[3]=-1*j;//top
	ARR4[3]=-1*i;//left
	dobreak=0;
	i= camXRes-20;
		//find last bright pixel (in lower right corner)
	while(i>10){
	for(j= camYRes-2;j>10;j--){
		pixPosition = i+j* camXRes;
		if(Y[pixPosition]>thresh){//if pixel is bright
			sum = 0;
			for(l=0;l>-10;l--){
				for(k=0;k>-10;k--){sum+=Y[pixPosition+l+k*( camXRes)];}}
			if(sum>=thresh*100){//if it is the bottom right pixel
				dobreak = 1;
					break;}//break while loop
			}//end if

	}
			if (dobreak)
			break;
			i--;}//end i and j loops
	ARR3[3]=j;//bottom
	ARR2[3]=i;//right

	//pick extremes
	OUTARRAY1[0]=-1*max_array(ARR4,4);//left offset
	OUTARRAY1[1]=max_array(ARR2,4);//right offset
	OUTARRAY1[2]=-1*max_array(ARR,4);//top offset
	OUTARRAY1[3]=max_array(ARR3,4);//bottom offset

	leftoffset = OUTARRAY1[0];
	rightoffset = OUTARRAY1[1];
	topoffset = OUTARRAY1[2];
	bottomoffset = OUTARRAY1[3];

	//set wBlock and hBlock
	OUTARRAY2[0] = (rightoffset - leftoffset)/(double)(maxBlocks+1);//block width
	OUTARRAY2[1] = (bottomoffset - topoffset)/(double)(maxBlocks+1);//block height

	fptr = fopen(outFile,"wb");
	fwrite(OUTARRAY1,sizeof(int),4,fptr);
	fwrite(OUTARRAY2,sizeof(double),2,fptr);
	fclose(fptr);

	wBlock = OUTARRAY2[0];
	hBlock = OUTARRAY2[1];

	free(Y);

}

//code to determine camera top, left, bottom, and right offsets upon initialization
//turns on the 4 corners
//called by python
void projectorON(int numXBlocks, int numYBlocks){
	// Turn projector screen on
	//must occur after FB is opened/initialize
	int i,j;
	BOOL output[XRES*YRES];
    int xblocksize = XRES/numXBlocks;
	int yblocksize = YRES/numYBlocks;
	//clear_screen(0);//set all black
	//turn corners on 
	//all black
	for (j=0;j<YRES;j++){
		for(i=0;i<XRES;i++){
			output[i+j*XRES]=FALSE;}}
	//top left
    for(j=0;j<yblocksize;j++){
		for (i=0;i<xblocksize;i++){
			output[i+j*XRES]=TRUE;}}
	//bottom left
	for (j=YRES-YRES%numYBlocks;j>YRES-YRES%numYBlocks-yblocksize;j--){
		for(i=0;i<xblocksize;i++){
			output[i+j*XRES]=TRUE;}}
	//bottom right
	for (j=YRES-YRES%numYBlocks;j>YRES-YRES%numYBlocks-yblocksize;j--){
		for(i=XRES-XRES%numXBlocks;i>XRES-XRES%numXBlocks-xblocksize;i--){
			output[i+j*XRES]=TRUE;}}
	//top right
	for (j=0;j<yblocksize;j++){
		for(i=XRES-XRES%numXBlocks;i>XRES-XRES%numXBlocks-xblocksize;i--){
				output[i+j*XRES]=TRUE;}}
	draw(output);
}


BOOL initialize(int cameraXRes, int cameraYRes, int maxBlocks){
	camXRes = cameraXRes;
	camYRes = cameraYRes;

    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
      printf("Error: cannot open framebuffer device.\n");
	  return FALSE;
    }
    char *kbfds = "/dev/tty";
    kbfd = open(kbfds, O_WRONLY);
    if (kbfd >= 0) {
        ioctl(kbfd, KDSETMODE, KD_GRAPHICS);
    }
    else {
        printf("Could not open %s.\n", kbfds);
		return FALSE;
    }


    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
      printf("Error reading variable information.\n");
	  return FALSE;
    }
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &orig_vinfo)) {
      printf("Error reading variable information.\n");
	  return FALSE;
    }

	//set vscreeninfo to what we want
    vinfo.bits_per_pixel = 24; //RGB888
	vinfo.xres = XRES;
    vinfo.yres = YRES;
    vinfo.xres_virtual = vinfo.xres;
    vinfo.yres_virtual = vinfo.yres * 2;//double buffering
   
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
      printf("Error setting variable information.\n");
	  return FALSE;
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
      printf("Error reading fixed information.\n");
	  return FALSE;
    }
    screensize = finfo.smem_len;
    page_size = finfo.line_length * vinfo.yres;
	//set framebufferpointer
    fbp = (char*)mmap(0,screensize,PROT_READ | PROT_WRITE, MAP_SHARED,fbfd,0);

	threshold = (uint8 *)malloc(sizeof(uint8)*maxBlocks*(maxBlocks+1)/2);

    return TRUE;
    
}

//return to terminal framebuffer
//called by python
void cleanup(char *thresholdFile,int maxBlocks){
	//write threshold values to file
	FILE * fID;
	fID = fopen(thresholdFile,"ab");
	fwrite(threshold,sizeof(uint8),maxBlocks*(maxBlocks+1)/2,fID);
	fclose(fID);

    munmap(fbp, screensize);

    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo)) {
        printf("Error re-setting variable information.\n");
    }

    if (kbfd >= 0) {
        ioctl(kbfd, KDSETMODE, KD_TEXT);
        close(kbfd);
    }
    close(fbfd);
}
