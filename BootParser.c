#include "xbox.h"
#include "xboxkrnl.h"
#include "BootString.h"
#include "BootParser.h"
#include "boot.h"

enum {
	VIDEO_MODE_UNKNOWN=-1,
	VIDEO_MODE_640x480=0,
	VIDEO_MODE_640x576,
	VIDEO_MODE_720x576,
	VIDEO_MODE_800x600,
	VIDEO_MODE_1024x576,
	VIDEO_MODE_COUNT
};

int sprintf(char * buf, const char *fmt, ...);

int ParseConfig(char *szPath,char *szBuffer, CONFIGENTRY *entry) {
	char *szLine;
	char *szTmp;
	char *szNorm;
        int nXboxFB = 0;
        int nVesaFB = 0;
	BYTE VideoStandard[4];
	char *ptr;
	//,ptr1;
	int i;
	
        szLine = (char *)MmAllocateContiguousMemoryEx(MAX_LINE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        szTmp = (char *)MmAllocateContiguousMemoryEx(MAX_LINE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        szNorm = (char *)MmAllocateContiguousMemoryEx(MAX_LINE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);

	for(i = 0; i < 4 ; i++) {
		VideoStandard[i] = I2CTransmitByteGetReturn(0x54, 0x58 + i);
	}
	
	memset(entry,0,sizeof(CONFIGENTRY));
	
	ptr = szBuffer;
	ptr = HelpGetToken(szBuffer,10);
	entry->nValid = 1;
	HelpCopyUntil(entry->szPath,szPath,MAX_LINE);
	entry->szPath[MAX_LINE-1]=0;
	while(1) {
		_strncpy(szLine,ptr,MAX_LINE);
		szLine[MAX_LINE-1]=0;
		
		if(HelpStrlen(ptr) < MAX_LINE) {
			if(HelpStrncmp(ptr,"kernel",HelpStrlen("kernel")) == 0)  {
				HelpGetParm(szTmp, ptr);
				HelpCopyUntil(entry->szKernel,szPath,MAX_LINE);
				HelpCopyUntil(HelpScan0(entry->szKernel),szTmp,MAX_LINE);
			}
			if(HelpStrncmp(ptr,"initrd",HelpStrlen("initrd")) == 0) {
				HelpGetParm(szTmp, ptr);
				HelpCopyUntil(entry->szInitrd,szPath,MAX_LINE);
				HelpCopyUntil(HelpScan0(entry->szInitrd),szTmp,MAX_LINE);
			}
			if(HelpStrncmp(ptr,"rivafb",HelpStrlen("xboxfb")) == 0) {
				nXboxFB = 1;
			}
			if(HelpStrncmp(ptr,"vesafb",HelpStrlen("vesafb")) == 0) {
				nVesaFB = 1;
			}
			if(HelpStrncmp(ptr,"append",HelpStrlen("append")) == 0)
				HelpGetParm(entry->szAppend, ptr);
		} else {
			entry->nValid = 0;
		}
		ptr = HelpGetToken(0,10);
		if(*ptr == 0) break;
	}

	if(nXboxFB == 1) {
		switch(*((VIDEO_STANDARD *)&VideoStandard)) {
			case NTSC_M:
				strcpy(szNorm," video=xbox:640x480,nohwcursor ");
				break;
			case PAL_I:
				strcpy(szNorm," video=xbox:640x480,nohwcursor ");
				break;
			case VID_INVALID:
			default:
				dprintf("%X  ", (int)((VIDEO_STANDARD )VideoStandard));
				break;
		}
	}

	if(nVesaFB == 1) {
		switch(*((VIDEO_STANDARD *)&VideoStandard)) {
			case NTSC_M:
				strcpy(szNorm," video=vesa:640x480,tv=NTSC ");
				break;
			case PAL_I:
				strcpy(szNorm," video=vesa:640x480,tv=PAL ");
				break;
			case VID_INVALID:
			default:
				dprintf("%X  ", (int)((VIDEO_STANDARD )VideoStandard));
				break;
		}
	}
	if(szNorm[0] != 0) {
		sprintf(entry->szAppend,"%s%s",entry->szAppend,szNorm);
	}

	MmFreeContiguousMemory(szLine);
	MmFreeContiguousMemory(szTmp);
	MmFreeContiguousMemory(szNorm);

	return entry->nValid;
}

void PrintConfig(CONFIGENTRY *entry) {
        dprintf("path \"%s\"\n", entry->szPath);
        dprintf("kernel \"%s\"\n", entry->szKernel);
        dprintf("initrd \"%s\"\n", entry->szInitrd);
        dprintf("command line: \"%s\"\n", entry->szAppend);
}

