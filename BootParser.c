#include "xbox.h"
#include "xboxkrnl.h"
#include "BootString.h"
#include "BootParser.h"

enum {
	VIDEO_MODE_UNKNOWN=-1,
	VIDEO_MODE_640x480=0,
	VIDEO_MODE_640x576,
	VIDEO_MODE_720x576,
	VIDEO_MODE_800x600,
	VIDEO_MODE_1024x576,
	VIDEO_MODE_COUNT
};

int ParseConfig(char *szPath,char *szBuffer, CONFIGENTRY *entry) {
	char *szLine;
	char *szTmp;
	char *szNorm;
        int nRivaFB = 0;
        int nVesaFB = 0;
	BYTE VideoStandard[4];
	char *ptr,ptr1;
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
	while(1) {
		memcpy(szLine,ptr,HelpStrlen(ptr));
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
			if(HelpStrncmp(ptr,"rivafb",HelpStrlen("rivafb")) == 0) {
				nRivaFB = 1;
			}
			if(HelpStrncmp(ptr,"vesafb",HelpStrlen("vesafb")) == 0) {
				nVesaFB = 1;
			}
			if(HelpStrncmp(ptr,"xymode",HelpStrlen("xymode")) == 0) {
				HelpGetParm(szTmp, ptr);
				if(HelpStrncmp(szTmp,"640x480",HelpStrlen("640x480")) == 0) {
					entry->vmode = VIDEO_MODE_640x480;					
				} else if(HelpStrncmp(szTmp,"800x600",HelpStrlen("800x600")) == 0) {
					entry->vmode = VIDEO_MODE_800x600;
				} else {
					entry->vmode = VIDEO_MODE_640x480;
				}
			}
			if(HelpStrncmp(ptr,"append",HelpStrlen("append")) == 0)
				HelpGetParm(entry->szAppend, ptr);
		} else {
			entry->nValid == 0;
		}
		ptr = HelpGetToken(0,10);
		if(*ptr == 0) break;
	}

	if(nRivaFB == 1) {
		switch(*((VIDEO_STANDARD *)&VideoStandard)) {
			case NTSC_M:
				strcpy(szNorm," video=riva:640x480,nomtrr,nohwcursor,fb_mem=4M,tv=NTSC ");
				break;
			case PAL_I:
				strcpy(szNorm," video=riva:640x480,nomtrr,nohwcursor,fb_mem=4M,tv=PAL ");
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
				if(entry->vmode == VIDEO_MODE_800x600) {
					strcpy(szNorm," video=vesa:800x600,tv=NTSC ");
				} else {
					strcpy(szNorm," video=vesa:640x480,tv=NTSC ");
				}
				break;
			case PAL_I:
				if(entry->vmode == VIDEO_MODE_800x600) {
					strcpy(szNorm," video=vesa:800x600,tv=PAL ");
				} else {
					strcpy(szNorm," video=vesa:640x480,tv=PAL ");
				}
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
        dprintf("vmode \"%d\"\n", entry->vmode);
        dprintf("command line: \"%s\"\n", entry->szAppend);
}

