#include "xboxkrnl.h"
typedef short WORD;
typedef int DWORD;
typedef int bool;

#define I2C_IO_BASE 0xC000
static __inline void IoOutputByte(WORD wAds, BYTE bValue) {
//	__asm__  ("			     out	%%al,%%dx" : : "edx" (dwAds), "al" (bValue)  );
    __asm__ __volatile__ ("outb %b0,%w1": :"a" (bValue), "Nd" (wAds));
}

static __inline void IoOutputWord(WORD wAds, WORD wValue) {
//	__asm__  ("	 out	%%ax,%%dx	" : : "edx" (dwAds), "ax" (wValue)  );
    __asm__ __volatile__ ("outw %0,%w1": :"a" (wValue), "Nd" (wAds));
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

int I2CTransmitByteGetReturn(BYTE bPicAddressI2cFormat, BYTE bDataToWrite)
{
	DWORD dwRetriesToLive=4;

	while(dwRetriesToLive--) {
		DWORD dwSpinsToLive=0x800000;

		IoOutputByte(I2C_IO_BASE+4, (bPicAddressI2cFormat<<1)|1);
		IoOutputByte(I2C_IO_BASE+8, bDataToWrite);
		IoOutputWord(I2C_IO_BASE+0, 0x10 /*IoInputWord(I2C_IO_BASE+0)*/);
		IoOutputByte(I2C_IO_BASE+2, 0x0a);

		{
			BYTE b=0;
			while( (b !=0x10) && ((b&0x26)==0) && (dwSpinsToLive--) ) {
				b=IoInputByte(I2C_IO_BASE+0);
//				if(dwSpinsToLive<0x7ffffd) bprintf("%02X\n", b);
			}
			if(dwSpinsToLive==0) return -1;
			if(b&0x2) {

				continue; // retry
			}
			if(b&0x24) return -1;
			if(!(b&0x10)) return -1;

				// we are okay, fetch returned byte
			return (int)IoInputByte(I2C_IO_BASE+6);

		}
	}
	return -1;
}

