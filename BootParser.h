#ifndef _BootParser_H_
#define _BootParesr_H_

#define MAX_LINE 1024

typedef struct _CONFIGENTRY {
        int  nValid;
	char szPath[MAX_LINE];
        char szKernel[MAX_LINE];
        char szInitrd[MAX_LINE];
        char szAppend[MAX_LINE];
	int vmode;
} CONFIGENTRY, *LPCONFIGENTRY;

typedef enum {
	VID_INVALID	= 0x00000000,
	NTSC_M		= 0x00400100,
	PAL_I		= 0x00800300
} VIDEO_STANDARD;

int ParseConfig(char *szPath,char *szBuffer, CONFIGENTRY *entry);
void PrintConfig(CONFIGENTRY *entry);

#endif // _BootParser_H_
