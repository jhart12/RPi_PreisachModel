//framebuffer.c
//source code for framebuffer functions
//these functions are not called by python main.py

#include "framebuffer.h"

//function to place either "on" or "off" pixels in RGB888
//internal function
void put_pixel(int x, int y, BOOL c)
{
//r and g are on because they correspont to the first half-frame and
//second half frame, respectively
//b doesn't correspond to anything in the LCr external pattern sequence mode
    //if c=1, display an "on" pixel, otherwise put an "off" pixel 
    unsigned int pix_offset = 3 * x + y * globals.fb.finfo.line_length;
    pix_offset += globals.fb.cur_page * globals.fb.page_size;


    if(c){
    *((char*)(globals.fb.fbp + pix_offset)) = 0;//b
    *((char*)(globals.fb.fbp + pix_offset+1)) = 255;//g
    *((char*)(globals.fb.fbp + pix_offset+2)) = 255;//r
    }
    else{
    *((char*)(globals.fb.fbp + pix_offset)) = 0;//r
    *((char*)(globals.fb.fbp + pix_offset+1)) = 0;//g
    *((char*)(globals.fb.fbp + pix_offset+2)) = 0;//b
    }	

}

//internal function
void clear_screen(int c) {
    memset(globals.fb.fbp + globals.fb.cur_page * globals.fb.page_size, c, globals.fb.page_size);
}

/*draws the output to the framebuffer and handles the double 
buffering*/
//internal function
void draw(BOOL* output) {
//draws the output to the framebuffer and handles the double 
//buffering

    int x, y, Youtput_offset;
    unsigned int pix_offset, Ypix_offset;
    globals.fb.cur_page = (globals.fb.cur_page + 1) % 2;
    int page_pix_offset = globals.fb.cur_page*globals.fb.page_size;



    for (y=0;y<globals.fb.vinfo.yres;y++){
			Ypix_offset = y*globals.fb.finfo.line_length;
			Youtput_offset = globals.fb.vinfo.xres*y;
    for (x=0;x<globals.fb.vinfo.xres;x++){
     // put_pixel(x,y,output[globals.fb.vinfo.xres*y+x]);}}
		pix_offset = 3*x+Ypix_offset;
    pix_offset += page_pix_offset;

    if(output[Youtput_offset+x]){
    *((char*)(globals.fb.fbp + pix_offset)) = 0;//b
    *((char*)(globals.fb.fbp + pix_offset+1)) = 255;//g
    *((char*)(globals.fb.fbp + pix_offset+2)) = 255;//r
    }
    else{
    *((char*)(globals.fb.fbp + pix_offset)) = 0;//r
    *((char*)(globals.fb.fbp + pix_offset+1)) = 0;//g
    *((char*)(globals.fb.fbp + pix_offset+2)) = 0;//b
    }	
	}}

    globals.fb.vinfo.yoffset = globals.fb.cur_page * globals.fb.vinfo.yres;
    ioctl(globals.fb.fbfd, FBIOPAN_DISPLAY, &globals.fb.vinfo);
    ioctl(globals.fb.fbfd, FBIO_WAITFORVSYNC, 0);
}
