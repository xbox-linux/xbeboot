int HelpStrlen(char *ptr) {
	int count;

	if(ptr == 0) return 0;
	for (count = 0; ptr[count] != 0;count++){};
	return count;
}
char *HelpGetToken(char *ptr,char token) {
	static char *old;
	char *mark;

	if(ptr != 0) old = ptr;
	mark = old;
	for(;*old != 0;old++) {
		if(*old == token) {
			*old = 0;
			old++;
			break;
		}
	}
	return mark;
}

void HelpGetParm(char *szBuffer, char *szOrig) {
	char *ptr,*copy;
	int nBeg = 0;
	int nCopy = 0;

	copy = szBuffer;
	for(ptr = szOrig;*ptr != 0;ptr++) {
		if(*ptr == ' ') nBeg = 1;
		if(*ptr != ' ' && nBeg == 1) nCopy = 1;
		if(nCopy == 1) {
			*copy = *ptr;
		 	copy++;
		}
	}
	*copy = 0;
}

char *HelpStrrchr(const char *string, int ch) {
        char *last = 0;
        char c = (char) ch;
        for (; *string; string++) {
 		if (*string == c) {
			last = (char *) string;
		}
	}		
        return last;
}

char *HelpCopyUntil(char* d, char* s, int max) {
        while ((*s!=' ')&&(*s!='\n')&&(*s!='\r')&&(*s)&&(max--)) {
                *d++ = *s++;
        }
        *d = 0;
        return s;
}

char *HelpScan0(char* s) {
        while (*s) s++;
        return s;
}

int HelpStrncmp(const char *sz1, const char *sz2, int nMax) {
        while((*sz1) && (*sz2) && nMax--) {
		if(*sz1 != *sz2) return (*sz1 - *sz2);
	                sz1++; sz2++;
        }
        if(nMax==0) return 0;
	        if((*sz1) || (*sz2)) return 0;
        return 0; // used up nMax
}

char * strcpy(char *sz, const char *szc)
{
        char *szStart=sz;
        while(*szc) *sz++=*szc++;
        *sz='\0';
        return szStart;
}

