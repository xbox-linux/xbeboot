#include "xbox.h"
#include "tux.h"

extern int framebuffer;

enum {
	LOGO_WIDE,
	LOGO_NARROW,
	LOGO_OTHER
};

int logo;

int clipping(float x, int y) {
// coordinates
#define nx 397
#define ny 100
#define ox 342
#define oy 188
#define px 319
#define py 215
#define qx 250
#define qy 302
#define rx 342
#define ry 243

// m's
#define m1 -625
#define m2 -793
#define m3 821

	int temp;

	if (logo==LOGO_WIDE) x = (x - 320) / 1.333333333 + 320;

	y = y - 5; /* hack: this function assumes video ram begins 5 lines below */
	if (y>=oy) goto l3;
	temp=y-ny;
	temp=temp*m1;
	temp=temp/1000;
	temp=temp+nx;
	if (x<temp) goto l_set;
	goto l_noset;
l3:
	if (y>=py) goto l4;
	if (x<ox) goto l_set;
	goto l_noset;
l4:
	temp=y-py;
	temp=temp*m2;
	temp=temp/1000;
	temp=temp+px;
	if (x<temp) goto l_set;
	temp=y-ry;
	temp=temp*m3;
	temp=temp/1000;
	temp=temp+rx;
	if (x<temp) goto l_noset;
	if (x>=ox) goto l_noset;
l_set:
	return 1;
l_noset:
	return 0;
}

void draw_tux(int start_x, int start_y, unsigned char weight, int do_clipping) {
	int x, y;
	unsigned int pixelrgb, pixelbgr;
	int i;

	for (y = 0; y<tux.height; y++) {
		for (x = 0; x<tux.width; x++) {
			pixelrgb = ((unsigned int*)tux.pixel_data)[y*tux.width+x];
			if (pixelrgb && ((!do_clipping)||(clipping(start_x+x, start_y+y)))) {
				pixelbgr = (pixelrgb & 0xff00ff00) | ((pixelrgb & 0xff0000) >> 16) | ((pixelrgb & 0xff) << 16);
					for (i=0; i<3; i++) {
						unsigned int p1 = (pixelbgr >> (8*i) & 255) * weight/255;
						unsigned int p2 = ((unsigned char*)framebuffer)[((y+start_y)*SCREEN_WIDTH+x+start_x)*4+i] * (255-weight)/255;
						((unsigned char*)framebuffer)[((y+start_y)*SCREEN_WIDTH+x+start_x)*4+i] = p1 + p2;
					}
			}
		}
	}
}

void show_splash(int progress) {
	if (logo==LOGO_OTHER) {
		unsigned char weight = 255/3*progress;
		draw_tux(240,100,weight,0);
	} else {
		switch (progress) {
		case 1:
			draw_tux(270,210,0,1);
			break;
		case 2:
			draw_tux(255,155,0,1);
			break;
		case 3:
			draw_tux(240,100,0,1);
			break;
		}
	}
}

void do_splash_init() {
	int x,y,c;
	c = 0;

	/* simple checksum  of a certain screen region */
	for (x=SCREEN_WIDTH/2-50; x<SCREEN_WIDTH/2+50; x++) {
		for (y=SCREEN_HEIGHT/2-50; y<SCREEN_HEIGHT/2+50; y++) {
			c ^= ((unsigned int*)framebuffer)[y*SCREEN_WIDTH+x];
		}
	}

	switch (c) {
		case 0x0039ef00:
			logo = LOGO_WIDE;
			break;
		case 0xffffffff:
			logo = LOGO_NARROW;
			break;
		default:
			logo = LOGO_OTHER;
	}

//	printf("\n\n\nFramebuffer Checksum: %08x\n", c);
}
