#include "xbox.h"
#include "BootString.h"
#include "BootParser.h"

int ParseConfig(char *szPath,char *szBuffer, CONFIGENTRY *entry) {
	char szLine[MAX_LINE];
	char szTmp[MAX_LINE];
	char *ptr,ptr1;
	int i;
	
	memset(entry,0,sizeof(CONFIGENTRY));
	ptr = szBuffer;
	ptr = HelpGetToken(szBuffer,10);
	entry->nValid = 1;
	HelpCopyUntil(entry->szPath,szPath,MAX_LINE);
	while(1) {
		my_memcpy(szLine,ptr,HelpStrlen(ptr));
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
			if(HelpStrncmp(ptr,"append",HelpStrlen("append")) == 0)
				HelpGetParm(entry->szAppend, ptr);
		} else {
			entry->nValid == 0;
		}
		ptr = HelpGetToken(0,10);
		if(*ptr == 0) break;
	}
	return entry->nValid;
}

void PrintConfig(CONFIGENTRY *entry) {
        dprintf("path \"%s\"\n", entry->szPath);
        dprintf("kernel \"%s\"\n", entry->szKernel);
        dprintf("initrd \"%s\"\n", entry->szInitrd);
        dprintf("command line: \"%s\"\n", entry->szAppend);
}

