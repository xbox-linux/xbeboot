#ifndef _Boot_H_
#define _Boot_H_

/***************************************************************************
      Includes used by XBox boot code
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/////////////////////////////////
// configuration

#define VIDEO_CONEXANT	0
#define VIDEO_FOCUS	1


#include "types.h"
#include "I2C.h"

static inline double min (double a, double b)
{
        if (a < b) return a; else return b;
}

static inline double max (double a, double b)
{
        if (a > b) return a; else return b;
}

///////////////////////////////
/* BIOS-wide error codes		all have b31 set  */

static __inline void IoOutputByte(WORD wAds, BYTE bValue) {
    __asm__ __volatile__ ("outb %b0,%w1": :"a" (bValue), "Nd" (wAds));
}

static __inline void IoOutputWord(WORD wAds, WORD wValue) {
    __asm__ __volatile__ ("outw %0,%w1": :"a" (wValue), "Nd" (wAds));
	}

static __inline void IoOutputDword(WORD wAds, DWORD dwValue) {
    __asm__ __volatile__ ("outl %0,%w1": :"a" (dwValue), "Nd" (wAds));
}


static __inline BYTE IoInputByte(WORD wAds) {
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (wAds));
  return _v;
}

static __inline WORD IoInputWord(WORD wAds) {
  WORD _v;

  __asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (wAds));
  return _v;
}

static __inline DWORD IoInputDword(WORD wAds) {
  DWORD _v;

  __asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (wAds));
  return _v;
}

void setup(void* KernelPos, void* PhysInitrdPos, void* InitrdSize, char* kernel_cmdline);

void * memcpy(void *dest, const void *src,  size_t size);
void * memset(void *dest, int data,  size_t size);
int _memcmp(const BYTE *pb, const BYTE *pb1, int n);
int _strncmp(const char *sz1, const char *sz2, int nMax);
char * strcpy(char *sz, const char *szc);

char *HelpGetToken(char *ptr,char token);
void HelpGetParm(char *szBuffer, char *szOrig);
char *HelpStrrchr(const char *string, int ch);
char *HelpCopyUntil(char* d, char* s, int max);
char *HelpScan0(char* s);


int printk(const char *fmt, ...);

#define wait_ms(x) wait_us(x);

void wait_us(DWORD ticks);

#endif // _Boot_H_
