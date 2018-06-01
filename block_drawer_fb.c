#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>
#include <stdlib.h>

#define RED_COLOR    0xF800
#define GREEN_COLOR  0x07E0
#define BLUE_COLOR   0x001F
#define BLACK_COLOR  0x0000
#define WHITE_COLOR  0xffff

char *fbp = 0;
/*
 * framebuffer application code, the start code of Linux GUI application
 * compile :
 *          $/usr/local/arm/2.95.3/bin/arm-linux-gcc -o fbtest fbtest.c
 *          $cp fbtest /tftpboot/examples
 * run in target:
 *          #mount 192.168.1.180:/tftpboot/ /mnt/nfs
 *          #cd /mnt/nfs/examples
 *          #./fbtest
 */
 
void PutPixel(unsigned int x,unsigned int y,unsigned int c)
{
    if(x<640 && y<480) {
    	*(fbp + y * 640*2 + x *2) = 0x00FF&c;
    	*(fbp + y * 640*2 + x *2 +1) = (0xFF00&c)>>8;
    }
}
void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

void Glib_Rectangle(int x1,int y1,int x2,int y2,int color)
{
    Glib_Line(x1,y1,x2,y1,color);
    Glib_Line(x2,y1,x2,y2,color);
    Glib_Line(x1,y2,x2,y2,color);
    Glib_Line(x1,y1,x1,y2,color);
}

void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;
    
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;
    }

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

void Glib_FilledCircle(int x1, int y1, int x2, int y2, int color) {
	int a, b, x0, y0, y, x, dx;
	a = (x2 - x1) /2;
	b = (y2 - y1) /2;
	y0 = (y1 + y2) /2;
	x0 = (x1 + x2) /2;

	for (y = -b; y <= b; y++) {
		dx = (int)(a * sqrt(1 - 1.0*(y*y)/(b*b)));
		Glib_Line(x0-dx, y0+y, x0+dx, y0+y, color);
	}
}




int draw_blocks_fb (int x1, int y1, int x2, int y2, float offset, int data_array[], int data_array_length) {
    
    int dx, dy;
    int i, j;
    int block_num_y, block_num_x;
    int y_line_btm, y_line_top;
    int x_d0, x_d1, x_d2, x_d3, x_d4;
    
    dx = (x2 - x1) / 4;
    dy = (y2 - y1) / 4;
    
    x_d0 = x1;
    x_d1 = x_d0 +dx;
    x_d2 = x_d1 +dx;
    x_d3 = x_d2 +dx;
    x_d4 = x_d3 +dx;
    
    block_num_x = 4;
    block_num_y = 4 + (offset == 0 ? 0 : 1);
    
   
    for(i = 0; i < block_num_y; i++) {
    
        if (i == 0) {
            y_line_btm = y2;
            y_line_top = (int)(y2 - dy * (1-offset));
        } else if (i >= 4) {
            y_line_btm = y_line_top;   
            y_line_top = y1;
        } else {
            y_line_btm = y_line_top;
            y_line_top = y_line_top - dy;
        }
        
        // printf("data: %d \n", data_array[i]);
    
        // printf("x1=%d, x2 = %d, y1=%d, y2=%d \n", x_d0, x_d1, y_line_btm, y_line_top);
        
        switch (data_array[i]) {
            case 1:
            Glib_FilledRectangle(x_d0, y_line_btm, x_d1,  y_line_top, BLACK_COLOR);
            Glib_FilledRectangle(x_d1, y_line_btm, x_d4,  y_line_top, WHITE_COLOR);
            break;
            
            case 2:
            Glib_FilledRectangle(x_d1, y_line_btm, x_d2,  y_line_top, BLACK_COLOR);
            Glib_FilledRectangle(x_d0, y_line_btm, x_d1,  y_line_top, WHITE_COLOR);
            Glib_FilledRectangle(x_d2, y_line_btm, x_d4,  y_line_top, WHITE_COLOR);
            break;

            case 3:
            Glib_FilledRectangle(x_d2, y_line_btm, x_d3,  y_line_top, BLACK_COLOR);
            Glib_FilledRectangle(x_d0, y_line_btm, x_d2,  y_line_top, WHITE_COLOR);
            Glib_FilledRectangle(x_d3, y_line_btm, x_d4,  y_line_top, WHITE_COLOR);
            break;
            
            case 4:
            Glib_FilledRectangle(x_d3, y_line_btm, x_d4,  y_line_top, BLACK_COLOR);
            Glib_FilledRectangle(x_d0, y_line_btm, x_d3,  y_line_top, WHITE_COLOR);
        }
        
    }
    
    return block_num_x * block_num_y;

}

int draw_block_prepare() {
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    int i = 0, j = 0;
    
    int x = 0, y = 0;
    int k;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        exit(3);
    }

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    printf("%dx%d, %dbpp, screensize = %ld\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screensize );

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    x = 100; y = 100;       // Where we are going to put the pixel

    if(vinfo.bits_per_pixel != 16) {  // 16bpp only
      printf("Error: pp !== 16.\n");
      exit(5);
    }
    
    // 16bpp framebuffer test
    printf("16bpp framebuffer test\n");
    printf("two bytes in fbp is a pixel of LCD, just set the value of fbp to put color to LCD\n");
    printf("byte format:\n");
    printf("  bit:| 15 14 13 12 11 | 10 9 8 7 6 5 | 4 3 2 1 0 |\n");
    printf("      |     red        |   green      |    blue   |\n");

    k = 0;

    /* Circel */
    for (j = vinfo.yres/30; j >= 0; j--) {
      for (i = vinfo.xres/30; i >= 0; i-=1) {
	      Glib_FilledCircle(10+i*30, 10+j*30, 30+i*30, 30+j*30, (int)(0xffffff * random()));
	      // sleep(0.05);
      }
    }
}

 
int mainn(int argc, char **argv)
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
	int i = 0, j = 0;
    
    int x = 0, y = 0;
    int k;
    
    int block_data[40];
    
    //long int location = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        exit(3);
    }

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    printf("%dx%d, %dbpp, screensize = %ld\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screensize );

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    x = 100; y = 100;       // Where we are going to put the pixel

    if(vinfo.bits_per_pixel != 16) {  // 16bpp only
      printf("Error: pp !== 16.\n");
      exit(5);
    }
    
    // 16bpp framebuffer test
    printf("16bpp framebuffer test\n");
    printf("two bytes in fbp is a pixel of LCD, just set the value of fbp to put color to LCD\n");
    printf("byte format:\n");
    printf("  bit:| 15 14 13 12 11 | 10 9 8 7 6 5 | 4 3 2 1 0 |\n");
    printf("      |     red        |   green      |    blue   |\n");

    k = 0;

    // Circel
    for (j = vinfo.yres/30; j >= 0; j--) {
      for (i = vinfo.xres/30; i >= 0; i-=1) {
	      Glib_FilledCircle(10+i*30, 10+j*30, 30+i*30, 30+j*30, (int)(0xffffff * random()));
	      // sleep(0.05);
      }
    }
    
    // Block Drawer
    
    block_data[0] = 1;
    block_data[1] = 2;
    block_data[2] = 3;
    block_data[3] = 1;
    block_data[4] = 4;
    
    for (i=5; i<40; i++) {
        block_data[i] = 1 + random() % 4;
    }
    
    for (i=0; i<40; i++){
        for (j=0; j<1000; j+=400) {
            draw_blocks(50, 100, 450, 410, j/1000.0, block_data+i, 5);
            sleep(0.03);
        }
    }

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
