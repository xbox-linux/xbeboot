#include "font.h"
#include "vsprintf.c"

int cx, cy;
unsigned int* framebuffer;

void printc(char c) {
	int x, y;
	int pixel;

	if (c=='\n') {
		cx = 0;
		if (++cy>=30) cy = 0;
		return;
	}
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 8; x++) {
			pixel = (font[c*16+y] >> (8-x)) & 1;
			if (pixel) pixel = -1;
			framebuffer[(cy*16+y)*640+cx*8+x] = pixel;
		}
	}

	if (++cx>=80) {
		cx = 0;
		if (++cy>=30) cy = 0;
	}
}
int print(const unsigned char* s, unsigned short len) {
	int i;
	for (i=0; i<len; i++) printc(s[i]);
	return 0;
}

int printk(const char *fmt, ...) {
        char buf[512];
        unsigned short len;
        va_list argList;
        va_start(argList, fmt);
        len=(unsigned short) vsprintf(buf, fmt, argList);
        va_end(argList);
        return print(&buf[0], len);
}
