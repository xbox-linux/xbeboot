/*
 * I2C-related code
 * AG 2002-07-27
 */

 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "boot.h"
#include "consts.h"


// ----------------------------  I2C -----------------------------------------------------------
//
// get a value from a given device address
// errors will have b31 set, ie, will be negative, otherwise fetched byte in LSB of return

int I2CTransmitByteGetReturn(BYTE bPicAddressI2cFormat, BYTE bDataToWrite)
{
	DWORD dwRetriesToLive=4;

	__asm __volatile__ ( "pushf ; cli" );

	while(IoInputWord(I2C_IO_BASE+0)&0x0800) ;  // Franz's spin while bus busy with any master traffic

	while(dwRetriesToLive--) {
		DWORD dwSpinsToLive=0x8000000;

		IoOutputByte(I2C_IO_BASE+4, (bPicAddressI2cFormat<<1)|1);
		IoOutputByte(I2C_IO_BASE+8, bDataToWrite);
		IoOutputWord(I2C_IO_BASE+0, 0xffff); // clear down all preexisting errors
		IoOutputByte(I2C_IO_BASE+2, 0x0a);

		{
			BYTE b=0;
			while( (b !=0x10) && ((b&0x26)==0) && (dwSpinsToLive--) ) {
				b=IoInputByte(I2C_IO_BASE+0);
			}
			if(dwSpinsToLive==0) { __asm __volatile__ ( "popf" ); return ERR_I2C_ERROR_TIMEOUT; }
			if(b&0x2) {

				continue; // retry
			}

			if(b&0x24) {
				continue;
			}
			if(!(b&0x10)) {
				continue;
			}

			__asm __volatile__ ( "popf" );

			// we are okay, fetch returned byte
			return (int)IoInputByte(I2C_IO_BASE+6);

		}
	}
		__asm __volatile__ ( "popf" );

	return ERR_I2C_ERROR_BUS;
}

// transmit a word, no returned data from I2C device

int I2CTransmitWord(BYTE bPicAddressI2cFormat, WORD wDataToWrite)
{
	DWORD dwRetriesToLive=4;
__asm __volatile__ ( "pushf; cli" );

	while(IoInputWord(I2C_IO_BASE+0)&0x0800) ;  // Franz's spin while bus busy with any master traffic

	while(dwRetriesToLive--) {
		DWORD dwSpinsToLive=0x8000000;

		IoOutputByte(I2C_IO_BASE+4, (bPicAddressI2cFormat<<1)|0);

		IoOutputByte(I2C_IO_BASE+8, (BYTE)(wDataToWrite>>8));
		IoOutputByte(I2C_IO_BASE+6, (BYTE)wDataToWrite);
		IoOutputWord(I2C_IO_BASE+0, 0xffff);  // clear down all preexisting errors
		IoOutputByte(I2C_IO_BASE+2, 0x1a);

		{
			BYTE b=0x0;
			while( (b!= 0x10) && ((b&0x26)==0) && (dwSpinsToLive--) ) { b=IoInputByte(I2C_IO_BASE+0); }

			if(dwSpinsToLive==0) { 		__asm __volatile__ ( "popf" ); return ERR_I2C_ERROR_TIMEOUT; }
			if(b&0x2) {
				continue; // retry
			}
			if(b&0x24) {
				continue;
			}
			if(!(b&0x10)) {
				continue;
			}

			__asm __volatile__ ( "popf" );

			// we are okay, return happy code
			return ERR_SUCCESS;
		}
	}
		__asm __volatile__ ( "popf" );
	return ERR_I2C_ERROR_BUS;
}

extern int I2cSetFrontpanelLed(BYTE b)
{
	__asm __volatile__ ( "pushf ; cli" );
	I2CTransmitWord( 0x10, 0x800 | b);  // sequencing thanks to Jarin the Penguin!
	I2CTransmitWord( 0x10, 0x701);
	__asm __volatile__ ( "popf" );

	return ERR_SUCCESS;
}

