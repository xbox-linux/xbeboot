#include "boot.h"
#include "BootEEPROM.h"

void BootEepromReadEntireEEPROM(EEPROMDATA *eeprom) {
	int i;
	BYTE *pb=(BYTE *)eeprom;
	DWORD dwTempInt;

	for(i = 0; i < 256; i++) {
		*pb++ = I2CTransmitByteGetReturn(0x54, i);
	}
}
