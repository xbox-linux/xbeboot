#include "xbox.h"
#include "xboxkrnl.h"

#define NULL 0

/* Xbox Linux XBE Bootloader
 *
 *  Configuration file parse code
 *  by Michael Steil, 2002
 *
 *  Aho probably hates me.
 */
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

char *my_strrchr(const char *string, int ch) {
	char *last = NULL;
	char c = (char) ch;
	for (; *string; string++)
		if (*string == c)
			last = (char *) string;
	return last;
}

char hex4(unsigned char n) {
	if (n<10) return n+'0'; else return n-10+'A';
}

char *hex256(char *s, unsigned char *d) {
	int i;
	for (i = 0; i < 16; i++, d++) {
		*s = hex4(*d >> 4);
		s++;
		*s = hex4(*d & 15);
		s++;
	}
	return s;
}

int parse(char *config, char *curdir, char *kernel, char *initrd, char *command_line_mem) {
	char root[BUFFERSIZE];
	char *command_line;
	int a;

	command_line = command_line_mem;

	/* the current directory is the default "root =" */
	copyuntil(root, curdir, BUFFERSIZE);

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

	prepend(kernel, root);
	/* ED : only if initrd */
	if(*initrd) prepend(initrd, root);
}

NTSTATUS ParseConfig(char *kernel, char *initrd, char *command_line) {
	char path[BUFFERSIZE];
	char filename[BUFFERSIZE];
	char config[CONFIG_BUFFERSIZE];
	ANSI_STRING ConfigFileString;
	HANDLE ConfigFile;
	OBJECT_ATTRIBUTES ConfigFileAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS Error;
	char *command_line_ptr;

	/* get the directory of the bootloader executable */
	copyuntil(path, XeImageFileName->Buffer, XeImageFileName->Length);
	my_strrchr(path, '\\')[1] = 0;
	/* read the config file from there */
	copyuntil(filename, path, BUFFERSIZE);
	copyuntil(scan0(filename), CONFIG_FILE, BUFFERSIZE);

	dprintf("Path: %s\n", path);
	dprintf("Filename: %s\n", filename);

	RtlInitAnsiString(&ConfigFileString, filename);

	ConfigFileAttributes.Attributes = OBJ_CASE_INSENSITIVE;
	ConfigFileAttributes.ObjectName = &ConfigFileString;
	ConfigFileAttributes.RootDirectory = NULL;

	Error = NtCreateFile(&ConfigFile, 0x80100080 /* GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES */, &ConfigFileAttributes,
		&IoStatusBlock, NULL, 0, 7 /* FILE_SHARE_READ | FILE_SHARE_WRITE |
		FILE_SHARE_DELETE*/, 1 /* FILE_OPEN */, 0x60 /* FILE_NON_DIRECTORY_FILE |
		FILE_SYNCHRONOUS_IO_NONALERT */);
	if (!NT_SUCCESS(Error)) return Error;
	dprintf("NtCreateFile() = %08x\n", Error);
	dprintf("HANDLE ConfigFile = %08x\n", ConfigFile);

	Error = NtReadFile(ConfigFile, NULL, NULL, NULL, &IoStatusBlock,
			config, CONFIG_BUFFERSIZE, NULL);
	if (!NT_SUCCESS(Error)) return Error;
	dprintf("Read.\n");

	parse(config, path, kernel, initrd, command_line);

	/* add "kbd-reset" to kernel command line to make the
	 * kernel detect that there's no keyboard controller */
	command_line_ptr = scan0(command_line);
	copyuntil(command_line_ptr, "kbd-reset", 9);
	command_line_ptr = scan0(command_line_ptr);
	*command_line_ptr = ' '; command_line_ptr++;
	/* add HDKey and EEPROMKey to kernel command line */
	copyuntil(command_line_ptr, "xboxeepromkey=", 14);
	command_line_ptr = scan0(command_line_ptr);
	command_line_ptr = hex256(command_line_ptr, *XboxEEPROMKey);
	*command_line_ptr = ' '; command_line_ptr++;
	copyuntil(command_line_ptr, "xboxhdkey=", 10);
	command_line_ptr = scan0(command_line_ptr);
	command_line_ptr = hex256(command_line_ptr, *XboxHDKey);
	*command_line_ptr = 0;

	dprintf("kernel \"%s\"\n", kernel);
	dprintf("initrd \"%s\"\n", initrd);
	dprintf("command line: \"%s\"\n", command_line);

	return STATUS_SUCCESS;
}
