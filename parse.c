#include "xbox.h"
#include "xboxkrnl.h"
#include "BootString.h"
#include "BootParser.h"

NTSTATUS GetConfig(CONFIGENTRY *entry) {
	char *path;
	char *filename;
	char *config;
	ANSI_STRING ConfigFileString;
	HANDLE ConfigFile;
	OBJECT_ATTRIBUTES ConfigFileAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS Error;

        path = (char *)MmAllocateContiguousMemoryEx(BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        filename = (char *)MmAllocateContiguousMemoryEx(BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        config = (char *)MmAllocateContiguousMemoryEx(CONFIG_BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
	
	memset(path,0,BUFFERSIZE);
	memset(config,0,CONFIG_BUFFERSIZE);
	/* get the directory of the bootloader executable */
	HelpCopyUntil(path, XeImageFileName->Buffer, XeImageFileName->Length);
	HelpStrrchr(path, '\\')[1] = 0;
	/* read the config file from there */
	HelpCopyUntil(filename, path, BUFFERSIZE);
	HelpCopyUntil(HelpScan0(filename), CONFIG_FILE, BUFFERSIZE);

//	dprintf("Path: %s\n", path);
//	dprintf("Filename: %s\n", filename);

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
//	dprintf("NtCreateFile() = %08x\n", Error);
//	dprintf("HANDLE ConfigFile = %08x\n", ConfigFile);

	Error = NtReadFile(ConfigFile, NULL, NULL, NULL, &IoStatusBlock,
			config, CONFIG_BUFFERSIZE, NULL);
	if (!NT_SUCCESS(Error)) return Error;
//	dprintf("Read.\n");

	ParseConfig(path,config,entry);
//	PrintConfig(entry);

	MmFreeContiguousMemory(path);
	MmFreeContiguousMemory(filename);
	return STATUS_SUCCESS;
}

NTSTATUS GetConfigXBE(CONFIGENTRY *entry) {
	char *path;
	char *filename;
	char *config;
	ANSI_STRING ConfigFileString;
	HANDLE ConfigFile;
	OBJECT_ATTRIBUTES ConfigFileAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS Error;
        unsigned int TempConfigStart;
        unsigned int TempConfigSize;
        
        path = (char *)MmAllocateContiguousMemoryEx(BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        filename = (char *)MmAllocateContiguousMemoryEx(BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
        config = (char *)MmAllocateContiguousMemoryEx(CONFIG_BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);

	memset(path,0,BUFFERSIZE);
	memset(config,0,CONFIG_BUFFERSIZE);
	/* get the directory of the bootloader executable */
	HelpCopyUntil(path, XeImageFileName->Buffer, XeImageFileName->Length);
	HelpStrrchr(path, '\\')[1] = 0;
	/* read the config file from there */
	HelpCopyUntil(filename, path, BUFFERSIZE);
	HelpCopyUntil(HelpScan0(filename), CONFIG_FILE, BUFFERSIZE);

//	dprintf("Path: %s\n", path);
//	dprintf("Filename: %s\n", filename);

	memcpy(&TempConfigStart,(void*)0x011094,4);	// This is the Real kernel Size
	memcpy(&TempConfigSize, (void*)0x011098,4);	// this is the kernel Size we pass to the Kernel loader
       	
       	memset(config,0x00,sizeof(config));
       	
       	memcpy(config,(void*)0x010000+TempConfigStart,TempConfigSize);

	ParseConfig(path,config,entry);
//	PrintConfig(entry);

	MmFreeContiguousMemory(path);
	MmFreeContiguousMemory(filename);
	return STATUS_SUCCESS;
}
