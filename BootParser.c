#include "xbox.h"
#include "xboxkrnl.h"
#include "BootString.h"
#include "BootParser.h"
#include "BootEEPROM.h"
#include "boot.h"

int sprintf(char * buf, const char *fmt, ...);

void chrreplace(char *string, char search, char ch) {
	char *ptr = string;
	while(*ptr != 0) {
		if(*ptr == search) {
			*ptr = ch;
		} else {
			ptr++;
		}
	}
}


int ParseConfig(char *szPath,char *szBuffer, CONFIGENTRY *entry) {
	char *szLine;
	char *szTmp;
	char *szNorm;
        int nXboxFB = 0;
        int nVesaFB = 0;
	char *ptr;
	//,ptr1;
	int i;
	
        szLine = (char *)MmAllocateContiguousMemoryEx(MAX_LINE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        szTmp = (char *)MmAllocateContiguousMemoryEx(MAX_LINE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        szNorm = (char *)MmAllocateContiguousMemoryEx(MAX_LINE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);

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
			if(HelpStrncmp(ptr,"xboxfb",HelpStrlen("xboxfb")) == 0) {
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
		strcpy(szNorm," video=xbox:640x480,nohwcursor ");
	}

	if(nVesaFB == 1) {
		strcpy(szNorm," video=vesa:640x480 ");
	}
	if(szNorm[0] != 0) {
		sprintf(entry->szAppend,"%s%s",entry->szAppend,szNorm);
	}

	MmFreeContiguousMemory(szLine);
	MmFreeContiguousMemory(szTmp);
	MmFreeContiguousMemory(szNorm);

	chrreplace(entry->szInitrd, '/', '\\');
	chrreplace(entry->szKernel, '/', '\\');

	return entry->nValid;
}

void PrintConfig(CONFIGENTRY *entry) {
        dprintf("path \"%s\"\n", entry->szPath);
        dprintf("kernel \"%s\"\n", entry->szKernel);
        dprintf("initrd \"%s\"\n", entry->szInitrd);
        dprintf("vmode \"%d\"\n", entry->vmode);
        dprintf("command line: \"%s\"\n", entry->szAppend);
}

