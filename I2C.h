#ifndef _I2C_H_
#define _I2C_H_

#define I2C_IO_BASE 0xc000

enum {
	I2C_LED_RED0 = 0x80,
	I2C_LED_RED1 = 0x40,
	I2C_LED_RED2 = 0x20,
	I2C_LED_RED3 = 0x10,
	I2C_LED_GREEN0 = 0x08,
	I2C_LED_GREEN1 = 0x04,
	I2C_LED_GREEN2 = 0x02,
	I2C_LED_GREEN3 = 0x01
};

///////////////////////////////
/* BIOS-wide error codes		all have b31 set  */

enum {
	ERR_SUCCESS = 0,  // completed without error

	ERR_I2C_ERROR_TIMEOUT = 0x80000001,  // I2C action failed because it did not complete in a reasonable time
	ERR_I2C_ERROR_BUS = 0x80000002, // I2C action failed due to non retryable bus error

	ERR_BOOT_PIC_ALG_BROKEN = 0x80000101 // PIC algorithm did not pass its self-test
};

unsigned int I2CTransmitWord(BYTE bPicAddressI2cFormat, WORD wDataToWrite);
unsigned int I2CTransmitByteGetReturn(BYTE bPicAddressI2cFormat, BYTE bDataToWrite);

unsigned int I2CWriteBytetoRegister(BYTE bPicAddressI2cFormat, BYTE bRegister, BYTE wDataToWrite);
unsigned int I2CWriteWordtoRegister(BYTE bPicAddressI2cFormat, BYTE bRegister, WORD wDataToWrite);

#endif // _I2C_H_
