#ifndef _BootString_H_
#define _BootString_H_

int HelpStrlen(char *ptr);
char *HelpGetToken(char *ptr,char token);
void HelpGetParm(char *szBuffer, char *szOrig);
char *HelpStrrchr(const char *string, int ch);
char *HelpCopyUntil(char* d, char* s, int max);
char *HelpScan0(char* s);
int HelpStrncmp(const char *sz1, const char *sz2, int nMax);
char * strcpy(char *sz, const char *szc);

#endif // _BootString_H_
