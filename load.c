/*
   Xbox XBE bootloader

    by Michael Steil & anonymous
    VESA Framebuffer code by Milosch Meriac

*/

#include "printf.c"
#include "consts.h"
#include "xboxkrnl.h"
#include "xbox.h"
#include "boot.h"
#include "BootString.h"
#include "BootParser.h"
#include "BootVideo.h"
#include "config.h"

#define ValueDumps 0x011080


volatile CURRENT_VIDEO_MODE_DETAILS currentvideomodedetails;
int NewFramebuffer;
long KernelSize;
PHYSICAL_ADDRESS PhysKernelPos, PhysEscapeCodePos;
PVOID EscapeCodePos;

static int ReadFile(HANDLE Handle, PVOID Buffer, ULONG Size);
int WriteFile(HANDLE Handle, PVOID Buffer, ULONG Size);
int SaveFile(char *szFileName,PBYTE Buffer,ULONG Size);
void DismountFileSystems(void);
int RemapDrive(char *szDrive);
HANDLE OpenFile(HANDLE Root, LPCSTR Filename, LONG Length, ULONG Mode);
BOOL GetFileSize(HANDLE File, LONGLONG *Size);

NTSTATUS GetConfig(CONFIGENTRY *entry);
NTSTATUS GetConfigXBE(CONFIGENTRY *entry);

void die() {
	while(1);
}

#ifdef LOADHDD
/* Loads the kernel image file into contiguous physical memory */
long LoadFile(PVOID Filename, long *lFileSize) {

	HANDLE hFile;
	PBYTE Buffer = 0;
	ULONGLONG FileSize;

        if (!(hFile = OpenFile(NULL, Filename, -1, FILE_NON_DIRECTORY_FILE))) {
		dprintf("Error open file %s\n",Filename);
                die();
	}

	if(!GetFileSize(hFile,&FileSize)) {
		dprintf("Error getting file size %s\n",Filename);
		die();
	}

	Buffer = MmAllocateContiguousMemoryEx(FileSize,
			MIN_KERNEL, MAX_KERNEL, 0, PAGE_READWRITE);
	if (!Buffer) {
		dprintf("Error alloc memory for File %s\n",Filename);
		die();
	}

	if (!ReadFile(hFile, Buffer, FileSize)) {
		dprintf("Error loading file %s\n",Filename);
		die();
	}

	NtClose(hFile);

	*lFileSize = FileSize;

	return (long)Buffer;
}
#endif

#ifdef LOADXBE
long LoadKernelXBE(long *FileSize) {

	PVOID Buffer;
	/* Size of the kernel file */
	ULONGLONG TempKernelStart;
	ULONGLONG TempKernelSize;
	ULONGLONG TempKernelSizev;
	TempKernelSize = *FileSize;

	// This is the Where the Real kernel Starts in the XBE
	memcpy(&TempKernelStart,(void*)ValueDumps,4);
	// This is the Real kernel Size
	memcpy(&TempKernelSizev,(void*)(ValueDumps+0x04),4);
	// this is the kernel Size we pass to the Kernel loader
	memcpy(&TempKernelSize,(void*)(ValueDumps+0x08),4);

	*FileSize= TempKernelSize;

	Buffer = MmAllocateContiguousMemoryEx((ULONG) TempKernelSize,
		MIN_KERNEL, MAX_KERNEL, 0, PAGE_READWRITE);
	if (!Buffer) return 0;

	// We fille the compleate space with 0xff
	memcpy(Buffer,(void*)0x010000+TempKernelStart,TempKernelSizev);
	memset(Buffer+TempKernelSizev,0xff,TempKernelSize-TempKernelSizev);

	// We force the Cache to write back the changes to RAM
	asm volatile ("wbinvd\n");

        return (long)Buffer;
}

long LoadIinitrdXBE(long *FileSize) {

	PVOID Buffer;
	/* Size of the initrd file */
	ULONGLONG TempInitrdStart;
	ULONGLONG TempInitrdSize;

	memcpy(&TempInitrdStart,(void*)ValueDumps+0xC,4);	// This is the Where the Real kernel Starts in the XBE
	memcpy(&TempInitrdSize,(void*)ValueDumps+0x10,4);	// This is the Real kernel Size

	*FileSize= TempInitrdSize;

	Buffer = MmAllocateContiguousMemoryEx((ULONG) TempInitrdSize,
		MIN_KERNEL, MAX_KERNEL, 0, PAGE_READWRITE);

	if (!Buffer) return 0;

	memcpy(Buffer,(void*)0x010000+TempInitrdStart,TempInitrdSize);
	// We force the Cache to write back the changes to RAM
	asm volatile ("wbinvd\n");

        return (long)Buffer;
}

#endif

BYTE CMOS_READ(BYTE addr) {
	IoOutputByte(0x70,addr); 
	return IoInputByte(0x71);
}

void CMOS_WRITE(BYTE val, BYTE addr) { 
	IoOutputByte(0x70,addr);
	IoOutputByte(0x71,val); 
}

unsigned char staticbios[] = {
	0x50,0xfd,0x38,0xed, 0x12,0xdd,0x05,0x04, 0x07,0x03,0x2d,0x02, 0x70,0x00,0x40,0x50,
	                                                               
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	
	0x40,0x40,0x4a,0xe0, 0x42,0xc1,0x40,0x40, 0x43,0xc2,0x40,0xd8, 0x40,0x3d,0x7e,0x20,
	0x50,0xfd,0x38,0xed, 0x12,0xdd,0x05,0x04, 0x07,0x03,0x2d,0x02, 0x00,0x00,0x40,0x50,
	
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,
	0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa, 0x55,0xaa,0x55,0xaa,

	0x40,0x40,0x4a,0xe0, 0x42,0xc1,0x40,0x40, 0x43,0xc2,0x40,0xd8, 0x40,0x3d,0x7e,0x20
	
		
};

void boot() {

	int xres = 0;
	int yres = 0;

	long KernelPos;
	long InitrdSize, InitrdPos;
	PHYSICAL_ADDRESS PhysInitrdPos;
	NTSTATUS Error;
	int data_PAGE_SIZE;
	extern int EscapeCode;

	//int i;
	

	CONFIGENTRY entry;


	currentvideomodedetails.m_nVideoModeIndex=VIDEO_MODE_640x480;
        currentvideomodedetails.m_pbBaseAddressVideo=(BYTE *)0xfd000000;
	currentvideomodedetails.m_dwFrameBufferStart = FRAMEBUFFER_START;

        BootVgaInitializationKernelNG((CURRENT_VIDEO_MODE_DETAILS *)&currentvideomodedetails);

	framebuffer = (unsigned int*)(0xF0000000+*(unsigned int*)0xFD600800);
	memset(framebuffer,0,640*480*4);

	memset(&entry,0,sizeof(CONFIGENTRY));
	cx = 0;
	cy = 0;

	/* parse the configuration file */

	dprintf("Xbox Linux XBEBOOT  %s\n",VERSION);

	dprintf("%s -  http://xbox-linux.sf.net\n",__DATE__);
	dprintf("(C)2002 Xbox Linux Team - Licensed under the GPL\n");
	dprintf("\n");

#if 0
      /*
  	dprintf("RTC Init\n");
	for(i = 0; i <= 0xff; i++) {
		dprintf("%02x ",CMOS_READ(i));
		if ((i%16)==15) dprintf("\n");
	}
        */
	for(i = 0; i <= 0xff; i++) {
		CMOS_WRITE(staticbios[i],i);
	}
    //    dprintf("RTC DONE\n");

  //      while(1);

#endif

	if(!RemapDrive("\\??\\D:")) {
		dprintf("Error RemapDrive\n");
		die();
	}


#ifdef LOADHDD
	Error = GetConfig(&entry);
#ifdef LOADHDD_CFGFALLBACK
        if (!NT_SUCCESS(Error)) {
        	Error = GetConfigXBE(&entry);
        }
#endif
	if (!NT_SUCCESS(Error)) die();

	// Load the kernel image into RAM
	KernelPos = LoadFile(entry.szKernel, &KernelSize);

	/* get physical addresses */
	PhysKernelPos = MmGetPhysicalAddress((PVOID)KernelPos);

	if (KernelPos == 0) {
		dprintf("Error Loading Kernel\n");
		die();
	}

	// ED : only if initrd
	if(entry.szInitrd[0]) {
		InitrdPos = LoadFile(entry.szInitrd, &InitrdSize);
		if (InitrdPos == 0) {
		        dprintf("Error Loading Initrd\n");
			die();
		}
		PhysInitrdPos = MmGetPhysicalAddress((PVOID)InitrdPos);
	} else {
		InitrdSize = 0;
		PhysInitrdPos = 0;
	}
#endif


#ifdef LOADXBE
	if (!NT_SUCCESS(GetConfigXBE(&entry))) die();

	// Load the kernel image into the correct RAM
	KernelPos = LoadKernelXBE(&KernelSize);
	PhysKernelPos = MmGetPhysicalAddress((PVOID)KernelPos);

	// Load the Ramdisk into the correct RAM
       	InitrdPos = LoadIinitrdXBE(&InitrdSize);
	PhysInitrdPos = MmGetPhysicalAddress((PVOID)InitrdPos);
#endif

	/* allocate memory for EscapeCode */
	EscapeCodePos = MmAllocateContiguousMemoryEx(PAGE_SIZE, RAMSIZE /4, 
			RAMSIZE / 2, 16, PAGE_READWRITE);
	PhysEscapeCodePos = MmGetPhysicalAddress(EscapeCodePos);

	data_PAGE_SIZE = PAGE_SIZE;
	Error = NtAllocateVirtualMemory((PVOID*)&PhysEscapeCodePos, 0, 
			(PULONG) &data_PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, 
			PAGE_EXECUTE_READWRITE);

	/* copy EscapeCode to EscapeCodePos & PhysEscapeCodePos */
	memcpy(EscapeCodePos, &EscapeCode, PAGE_SIZE);
	memcpy((void*)PhysEscapeCodePos, &EscapeCode, PAGE_SIZE);

	switch(entry.vmode) {
		case VIDEO_MODE_640x480:
			xres = 640;
			yres = 480;
			break;
		case VIDEO_MODE_800x600:
			xres = 800;
			yres = 600;
			break;
	}

	currentvideomodedetails.m_nVideoModeIndex=entry.vmode;
        currentvideomodedetails.m_pbBaseAddressVideo=(BYTE *)0xfd000000;
        currentvideomodedetails.m_fForceEncoderLumaAndChromaToZeroInitially=0;

	setup((void*)KernelPos, (void*)PhysInitrdPos, (void*)InitrdSize, entry.szAppend,xres,yres);

	BootVgaInitializationKernelNG((CURRENT_VIDEO_MODE_DETAILS *)&currentvideomodedetails);
	memset(framebuffer,0,xres*yres*4);

	/* orange LED */
	HalWriteSMBusValue(0x20, 0x08, FALSE, 0xff);
	HalWriteSMBusValue(0x20, 0x07, FALSE, 0x01);
	NewFramebuffer = NEW_FRAMEBUFFER + 0xF0000000;

	__asm(
		"mov	PhysEscapeCodePos, %edx\n"

		/* construct jmp to new CS */
		"mov	%edx, %eax\n"
		"sub	$EscapeCode, %eax\n"
		"add	$newloc, %eax\n"
		"mov	EscapeCodePos, %ebx\n"
		"sub	$EscapeCode, %ebx\n"
		"add	$ptr_newloc, %ebx\n"
		"mov	%eax, (%ebx)\n"

		"mov	NewFramebuffer, %edi\n"
		"mov	PhysKernelPos, %ebp\n"
		"mov	KernelSize, %esp\n"

		"cli\n"
		"jmp	*%edx\n"

		/* edi = NewFramebuffer
		   ebp = PhysKernelPos
		   esp = KernelSize */
	);
}


int WriteFile(HANDLE Handle, PVOID Buffer, ULONG Size)
{
        IO_STATUS_BLOCK IoStatus;

        // Try to write the buffer
        if (!NT_SUCCESS(NtWriteFile(Handle, NULL, NULL, NULL, &IoStatus,
                Buffer, Size, NULL)))
                return 0;

        // Verify that the amount written is the correct size
        if (IoStatus.Information != Size)
                return 0;

        return 1;
}

int ReadFile(HANDLE Handle, PVOID Buffer, ULONG Size)
{
        IO_STATUS_BLOCK IoStatus;

        // Try to write the buffer
        if (!NT_SUCCESS(NtReadFile(Handle, NULL, NULL, NULL, &IoStatus,
                Buffer, Size, NULL)))
                return 0;

        // Verify that the amount read is the correct size
        if (IoStatus.Information != Size)
                return 0;

        return 1;
}

int SaveFile(char *szFileName,PBYTE Buffer,ULONG Size) {

	ANSI_STRING DestFileName;
        IO_STATUS_BLOCK IoStatus;
        OBJECT_ATTRIBUTES Attributes;
        HANDLE DestHandle = NULL;

	RtlInitAnsiString(&DestFileName,szFileName);
        Attributes.RootDirectory = NULL;
        Attributes.ObjectName = &DestFileName;
        Attributes.Attributes = OBJ_CASE_INSENSITIVE;

	if (!NT_SUCCESS(NtCreateFile(&DestHandle,
		GENERIC_WRITE  | GENERIC_READ | SYNCHRONIZE,
		&Attributes, &IoStatus,
		NULL, FILE_RANDOM_ACCESS,
		FILE_SHARE_READ, FILE_CREATE,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE))) {
			dprintf("Error saving File\n");
			return 0;
	}

	if(!WriteFile(DestHandle, Buffer, Size)) {
		dprintf("Error saving File\n");
		return 0;
	}

	NtClose(DestHandle);

	return 1;
}

// Dismount all file systems
void DismountFileSystems(void) {

        ANSI_STRING String;

        RtlInitAnsiString(&String, "\\Device\\Harddisk0\\Partition1");
        IoDismountVolumeByName(&String);
        RtlInitAnsiString(&String, "\\Device\\Harddisk0\\Partition2");
        IoDismountVolumeByName(&String);
        RtlInitAnsiString(&String, "\\Device\\Harddisk0\\Partition3");
        IoDismountVolumeByName(&String);
        RtlInitAnsiString(&String, "\\Device\\Harddisk0\\Partition4");
        IoDismountVolumeByName(&String);
        RtlInitAnsiString(&String, "\\Device\\Harddisk0\\Partition5");
        IoDismountVolumeByName(&String);
        RtlInitAnsiString(&String, "\\Device\\Harddisk0\\Partition6");
        IoDismountVolumeByName(&String);
}

NTSTATUS GetConfig(CONFIGENTRY *entry) {

	ULONGLONG FileSize;
	PBYTE Buffer;
	HANDLE hFile;

	memset(entry,0,sizeof(CONFIGENTRY));

        if (!(hFile = OpenFile(NULL, "\\??\\D:\\linuxboot.cfg", -1, FILE_NON_DIRECTORY_FILE)))
                return 1;

	if(!GetFileSize(hFile,&FileSize)) {
		dprintf("Error getting file size!\n");
		die();
	}

	Buffer = MmAllocateContiguousMemoryEx(FileSize,
			MIN_KERNEL, MAX_KERNEL, 0, PAGE_READWRITE);

	if (!Buffer) {
		dprintf("Error alloc memory for File\n");
		return 1;
	}

	if (!ReadFile(hFile, Buffer, FileSize)) {
		dprintf("Error loading file\n");
		return 1;
	}

	ParseConfig("\\??\\D:\\",Buffer,entry);

	NtClose(hFile);

	return STATUS_SUCCESS;
}

NTSTATUS GetConfigXBE(CONFIGENTRY *entry) {
	PBYTE Buffer;
        unsigned int TempConfigStart;
        unsigned int TempConfigSize;

	// This is the Real kernel Size
	memcpy(&TempConfigStart,(void*)(ValueDumps+0x14),4);
	// this is the kernel Size we pass to the Kernel loader
	memcpy(&TempConfigSize, (void*)(ValueDumps+0x18),4);

	Buffer = MmAllocateContiguousMemoryEx(CONFIG_BUFFERSIZE,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);

       	memset(Buffer,0x00,CONFIG_BUFFERSIZE);
       	memcpy(Buffer,(void*)0x010000+TempConfigStart,TempConfigSize);

	ParseConfig("\\??\\D:\\",Buffer,entry);

	return STATUS_SUCCESS;
}

// Remap the drive to the XBE directory, even if the drive already mapped
int RemapDrive(char *szDrive)
{
	ANSI_STRING LinkName, TargetName;
	char *DDrivePath = 0;
	char *temp = 0;

	// Allocate room for the drive path
	DDrivePath = (char *)MmAllocateContiguousMemoryEx(
				XeImageFileName->Length + 1,MIN_KERNEL,
	                        MAX_KERNEL, 0, PAGE_READWRITE);
	if (!DDrivePath)
		return 0;

	// Copy the XBE filename for now
	memcpy(DDrivePath, XeImageFileName->Buffer, XeImageFileName->Length);
	DDrivePath[XeImageFileName->Length] = 0;

	// Delete the trailing backslash, chopping off the XBE name, and make it
	// into an ANSI_STRING
	if (!(temp = HelpStrrchr(DDrivePath, '\\')))
		return 0;
	*temp = 0;

	RtlInitAnsiString(&TargetName, DDrivePath);

	// Set up the link
	RtlInitAnsiString(&LinkName, szDrive);
	IoDeleteSymbolicLink(&LinkName);

	if(!NT_SUCCESS(IoCreateSymbolicLink(&LinkName, &TargetName))) {
		dprintf("Error IoCreateSymbolicLink\n");
		return 0;
	}

	// Delete the filename memory
	MmFreeContiguousMemory(DDrivePath);

	return 1;
}

// Opens a file or directory for read-only access
// Length parameter is negative means use strlen()
// This was originally designed to open directories, but it turned out to be
// too much of a hassle and was scrapped.  Use only for files with the
// FILE_NON_DIRECTORY_FILE mode.
HANDLE OpenFile(HANDLE Root, LPCSTR Filename, LONG Length, ULONG Mode)
{
        ANSI_STRING FilenameString;
        OBJECT_ATTRIBUTES Attributes;
        IO_STATUS_BLOCK IoStatus;
        HANDLE Handle;

        // Initialize the filename string
        // If a length is specified, set up the string manually
        if (Length >= 0)
        {
                FilenameString.Length = (USHORT) Length;
                FilenameString.MaxLength = (USHORT) Length;
                FilenameString.Buffer = (PSTR) Filename;
        }
        // Use RtlInitAnsiString to do it for us
        else
                RtlInitAnsiString(&FilenameString, Filename);

        // Initialize the object attributes
        Attributes.Attributes = OBJ_CASE_INSENSITIVE;
        Attributes.RootDirectory = Root;
        Attributes.ObjectName = &FilenameString;

        // Try to open the file or directory
        if (!NT_SUCCESS(NtCreateFile(&Handle, GENERIC_READ | SYNCHRONIZE,
                &Attributes, &IoStatus, NULL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE
                | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT |
                Mode)))
                return NULL;

        return Handle;
}

// Gets the size of a file
BOOL GetFileSize(HANDLE File, LONGLONG *Size)
{
        FILE_NETWORK_OPEN_INFORMATION SizeInformation;
        IO_STATUS_BLOCK IoStatus;

        // Try to retrieve the file size
        if (!NT_SUCCESS(NtQueryInformationFile(File, &IoStatus,
                &SizeInformation, sizeof(SizeInformation),
                FileNetworkOpenInformation)))
                return FALSE;

        *Size = SizeInformation.EndOfFile.QuadPart;
        return TRUE;
}

