#include "xbox.h"
#include "xboxkrnl.h"

#define NULL 0

char lcase(char c) {
	if (c>='A'&&c<='Z') return c + 32; else return c;
}

int cmpbeg(char* s1, char* s2) {
	for (;;s1++,s2++) {
		if (!*s2) return 1;
		if (lcase(*s1)!=lcase(*s2)) return 0;
	}
}

char* skipspaces(char* s) {
	while (((*s==' ')||(*s=='\t'))&&(*s)) s++;
	return s;
}

char* skipspacescrlf(char* s) {
	while (((*s==' ')||(*s=='\t')||(*s=='\n')||(*s=='\r'))&&(*s)) s++;
	return s;
}

char* skipuntilcrlf(char* s) {
	while ((*s!='\n')&&(*s!='\r')&&(*s)) s++;
	return s;
}

char* copyuntil(char* d, char* s, int max) {
	while ((*s!=' ')&&(*s!='\n')&&(*s!='\r')&&(*s)&&(max--)) {
		*d++ = *s++;
	}
	*d = 0;
	return s;
}

int eol(char *s) {
	if ((*s=='\n')||(*s=='\r')||(!*s)) return 1; else return 0;
}

char* scan0(char* s) {
	while (*s) s++;
	return s;
}

void prepend(char* d, char* s) {
	char temp_mem[BUFFERSIZE*2];
	char *temp;
	temp = temp_mem;

	copyuntil(temp, s, BUFFERSIZE*2);
	temp = scan0(temp);
	copyuntil(temp, d, BUFFERSIZE*2 - (temp - temp_mem));
	copyuntil(d, temp_mem, BUFFERSIZE*2);
}

int parse(char* config, char* kernel, char* initrd, char* command_line_mem) {
//	char string[1024] = "ROOT \\Device\\CdRom0\nKERNEL    \\Device\\Harddisk0\\Partition1\\vmlinuz parameter1\nAPPEND console=/dev/null initrd=\\Device\\Harddisk0\\Partition1\\initrd blah=x\nINITRD blah";
	char root[512];
	char *command_line;
	int a;

	command_line = command_line_mem;

	*root = 0;
	*kernel = 0;
	*initrd = 0;
	*command_line = 0;

	while (*config) {
		config = skipspacescrlf(config);
		if (cmpbeg(config, "TITLE ")||cmpbeg(config, "LABEL ")) {
			config += 6;
			/* multiboot... one day... */
		} else if (cmpbeg(config, "KERNEL ")) {
			config += 7;
			config = skipspaces(config);
			config = copyuntil(kernel, config, BUFFERSIZE);
			if (!eol(config)) goto append;
		} else if (cmpbeg(config, "APPEND ")) {
			config += 7;
append:
			while (!eol(config)) {
				config = skipspaces(config);
				if (cmpbeg(config, "initrd=")) {
					config += 7;
					config = copyuntil(initrd, config, BUFFERSIZE);
				} else {
					config = copyuntil(command_line, config, BUFFERSIZE - (command_line - command_line_mem));
					command_line = scan0(command_line);
					*command_line++ = ' ';
					*command_line = 0;
				}
			}
		} else if (cmpbeg(config, "INITRD ")) {
			config += 7;
			config = skipspaces(config);
			config = copyuntil(initrd, config, BUFFERSIZE);
		} else if (cmpbeg(config, "ROOT ")) {
			config += 5;
			config = skipspaces(config);
			config = copyuntil(root, config, BUFFERSIZE);
		} else {
			/* just ignore */
		}
		config = skipuntilcrlf(config);
	}

	if (*root) {
		prepend(kernel, root);
		prepend(initrd, root);
	}
}

void ParseConfig(char* kernel, char* initrd, char* command_line) {
	char config[CONFIG_BUFFERSIZE];
	ANSI_STRING ConfigFileString;
	HANDLE ConfigFile;
	OBJECT_ATTRIBUTES ConfigFileAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS Error;

	RtlInitAnsiString(&ConfigFileString, CONFIG_FILE);

	ConfigFileAttributes.Attributes = OBJ_CASE_INSENSITIVE;
	ConfigFileAttributes.ObjectName = &ConfigFileString;
	ConfigFileAttributes.RootDirectory = NULL;

	Error = NtCreateFile(&ConfigFile, 0x80100080 /* GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES */, &ConfigFileAttributes,
		&IoStatusBlock, NULL, 0, 7 /* FILE_SHARE_READ | FILE_SHARE_WRITE |
		FILE_SHARE_DELETE*/, 1 /* FILE_OPEN */, 0x60 /* FILE_NON_DIRECTORY_FILE |
		FILE_SYNCHRONOUS_IO_NONALERT */);
	dprintf("NtCreateFile() = %08x\n", Error);
	dprintf("HANDLE ConfigFile = %08x\n", ConfigFile);

	NtReadFile(ConfigFile, NULL, NULL, NULL, &IoStatusBlock,
			config, CONFIG_BUFFERSIZE, NULL);
	dprintf("Read.\n");

	parse(config, kernel, initrd, command_line);

	dprintf("kernel \"%s\"\n", kernel);
	dprintf("initrd \"%s\"\n", initrd);
	dprintf("command line: \"%s\"\n", command_line);
}
