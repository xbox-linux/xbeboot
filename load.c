/*
   Xbox XBE bootloader

    by Michael Steil & anonymous
    VESA Framebuffer code by Milosch Meriac

*/

#include <stdarg.h>
#include <stdio.h>
#include <asm/io.h>
#include "printf.c"
#include "consts.h"
#include "xboxkrnl.h"
#include "xbox.h"
#include "BootString.h"
#include "BootParser.h"
#include "BootVideo.h"

volatile CURRENT_VIDEO_MODE_DETAILS currentvideomodedetails;

/* Loads the kernel image file into contiguous physical memory */
static NTSTATUS LoadFile(PVOID Filename, int *FilePos, int *FileSize) {
	HANDLE KernelFile;

	PVOID VirtKernel;

	/* Temporary buffer for use with ReadFile */
	static BYTE ReadBuffer[READ_CHUNK_SIZE];

	/* Error code from an NT kernel call */
	NTSTATUS Error;

	/* ANSI_STRING of the kernel image filename */
	ANSI_STRING KernelFileString;
	/* Object attributes of the kernel image file */
	OBJECT_ATTRIBUTES KernelFileAttributes;
	/* IO status block (for reading the file) */
	IO_STATUS_BLOCK IoStatusBlock;

	/* Size of the kernel file */
	ULONGLONG TempKernelSize;
	/* Temporary size */
	ULONG TempSize;
	/* Read pointer to the kernel file */
	PUCHAR ReadPtr;

	ULONG ReadSize;

	int i;
	
	PHYSICAL_ADDRESS max_kernel;

//	dprintf("Loading \"%s\"...", Filename);
	/* Make an ANSI_STRING out of the kernel image filename */
	RtlInitAnsiString(&KernelFileString, Filename);

	/* Kernel object attributes (ignore case, use system root) */
	KernelFileAttributes.Attributes = OBJ_CASE_INSENSITIVE;
	KernelFileAttributes.ObjectName = &KernelFileString;
	KernelFileAttributes.RootDirectory = NULL;

	/* Open a file handle to the kernel image */
	Error = NtCreateFile(&KernelFile, 0x80100080 /* GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES */, &KernelFileAttributes,
		&IoStatusBlock, NULL, 0, 7 /* FILE_SHARE_READ | FILE_SHARE_WRITE |
		FILE_SHARE_DELETE*/, 1 /* FILE_OPEN */, 0x60 /* FILE_NON_DIRECTORY_FILE |
		FILE_SYNCHRONOUS_IO_NONALERT */);
//	dprintf("NtCreateFile() = %08x\n", Error);
	if (!NT_SUCCESS(Error)) goto ErrorNothing;

//	dprintf("HANDLE KernelFile = %08x\n", KernelFile);

	TempKernelSize = *FileSize;

	/* Try to allocate contiguous physical memory for the kernel image.
	   We need an address from 1 meg or above.
	   NOTE: We CANNOT use the memory allocated here in most API calls,
	   including ReadFile().*/
	max_kernel = MAX_KERNEL;
//	dprintf("MmAllocateContiguousMemoryEx(%08x, %08x, %08x, %08x, %08x) = ", (ULONG) TempKernelSize,
//		MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
	VirtKernel = MmAllocateContiguousMemoryEx((ULONG) TempKernelSize,
		MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
//	dprintf("%08x\n", VirtKernel);
	if (!VirtKernel)
	{
		Error = STATUS_NO_MEMORY;
		goto ErrorCloseFile;
	}

//	dprintf("Reading");
	ReadPtr = (PUCHAR) VirtKernel;
	for (i=0; i < TempKernelSize; i+=READ_CHUNK_SIZE, ReadPtr+=READ_CHUNK_SIZE) {
		ReadSize = READ_CHUNK_SIZE;
		Error = NtReadFile(KernelFile, NULL, NULL, NULL, &IoStatusBlock,
			ReadBuffer, ReadSize, NULL);
		my_memcpy(ReadPtr, ReadBuffer, ReadSize);
//		dprintf(".");
		if (!NT_SUCCESS(Error)) break;
	}
//	dprintf("done. (%i bytes @ %08x)\n", i, VirtKernel);

	/* Done; return size to caller */
	*FileSize = i; /* just an estimation */
	*FilePos = (int)VirtKernel;
	Error = STATUS_SUCCESS;
	goto ErrorCloseFile;


/* Undoes the allocations and returns an error code */
ErrorFreeMemory:
	MmFreeContiguousMemory(VirtKernel);

ErrorCloseFile:
/*	NtClose(KernelFile); */

ErrorNothing:
	return Error;
}

void die(char *s) {
	printf(s);
	while(1);
}

extern int I2CTransmitByteGetReturn(char bPicAddressI2cFormat, char bDataToWrite);

int NewFramebuffer;
int KernelSize;
PHYSICAL_ADDRESS PhysKernelPos, PhysEscapeCodePos;
PVOID EscapeCodePos;

NTSTATUS GetConfig(CONFIGENTRY *entry);

void boot() {
	BYTE bAvPackType;
	int i;
	unsigned char* s;
	int KernelPos;
	int InitrdSize, InitrdPos;
	PHYSICAL_ADDRESS PhysInitrdPos;
	NTSTATUS Error;
	int data_PAGE_SIZE;
	extern int EscapeCode, EscapeCodeEnd;
	extern void* newloc, ptr_newloc;
	CONFIGENTRY entry;
	
        currentvideomodedetails.m_nVideoModeIndex=VIDEO_MODE_640x480;
        currentvideomodedetails.m_pbBaseAddressVideo=(BYTE *)0xfd000000;
        currentvideomodedetails.m_fForceEncoderLumaAndChromaToZeroInitially=0;

        BootVgaInitializationKernel((CURRENT_VIDEO_MODE_DETAILS *)&currentvideomodedetails);

	framebuffer = (unsigned int*)(0xF0000000+*(unsigned int*)0xFD600800);

	memset(framebuffer,0,640*480*4);

	memset(&entry,0,sizeof(CONFIGENTRY));
	cx = 0;
	cy = 0;
	splash_init();
//	dprintf("\n");
//	dprintf("Framebuffer at: 0x%08x\n", framebuffer);

	/* parse the configuration file */
	Error = GetConfig(&entry);

	if (!NT_SUCCESS(Error)) die("Error loading configuration file!\n");

 	splash(3);

	dprintf("Xbox Linux XBEBOOT  %s\n",VERSION);
	
	dprintf("%s -  http://xbox-linux.sf.net\n",__DATE__);
	dprintf("(C)2002 Xbox Linux Team - Licensed under the GPL\n");
	dprintf("\n");
	
	/* Load the kernel image into RAM */
	KernelSize = MAX_KERNEL_SIZE;
	Error = LoadFile(entry.szKernel, &KernelPos, &KernelSize);

	if (!NT_SUCCESS(Error)) die("Error loading kernel!\n");

	/* ED : only if initrd */
	if(entry.szInitrd[0]) {
		InitrdSize = MAX_INITRD_SIZE;
		Error = LoadFile(entry.szInitrd, &InitrdPos, &InitrdSize);

		if (!NT_SUCCESS(Error)) die("Error loading initrd!\n");
		PhysInitrdPos = MmGetPhysicalAddress((PVOID)InitrdPos);
//		dprintf("PhysInitrdPos = 0x%08x\n", PhysInitrdPos);
	} else {
		InitrdSize = 0;
		PhysInitrdPos = 0;
	}


	/* get physical addresses */
	PhysKernelPos = MmGetPhysicalAddress((PVOID)KernelPos);
//	dprintf("PhysKernelPos = 0x%08x\n", PhysKernelPos);

	/* allocate memory for EscapeCode */
	EscapeCodePos = MmAllocateContiguousMemoryEx(PAGE_SIZE, RAMSIZE /4, RAMSIZE / 2, 16, PAGE_READWRITE);
//	dprintf("EscapeCodePos = 0x%08x\n", EscapeCodePos);
	PhysEscapeCodePos = MmGetPhysicalAddress(EscapeCodePos);
//	dprintf("PhysEscapeCodePos = 0x%08x\n", PhysEscapeCodePos);

	data_PAGE_SIZE = PAGE_SIZE;
	Error = NtAllocateVirtualMemory((PVOID*)&PhysEscapeCodePos, 0, (PULONG) &data_PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//	dprintf("NtAllocateVirtualMemory() = 0x%08x\n", Error);

	/* copy EscapeCode to EscapeCodePos & PhysEscapeCodePos */
	my_memcpy(EscapeCodePos, &EscapeCode, PAGE_SIZE);
	my_memcpy((void*)PhysEscapeCodePos, &EscapeCode, PAGE_SIZE);

//	dprintf("Setup...");
	setup(KernelPos, PhysInitrdPos, InitrdSize, entry.szAppend);
//	dprintf("done.");

//	dprintf("Starting kernel...");

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

