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

volatile CURRENT_VIDEO_MODE_DETAILS currentvideomodedetails;


volatile CURRENT_VIDEO_MODE_DETAILS currentvideomodedetails;

/* Loads the kernel image file into contiguous physical memory */
static NTSTATUS LoadFile(PVOID Filename, long *FilePos, long *FileSize) {
	HANDLE KernelFile;

	PVOID VirtKernel = 0;

	/* Temporary buffer for use with ReadFile */
	PVOID ReadBuffer = 0;

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
	
	ReadBuffer = MmAllocateContiguousMemoryEx(READ_CHUNK_SIZE,MIN_KERNEL, 
			MAX_KERNEL, 0, PAGE_READWRITE);
	if (!ReadBuffer) {
		Error = STATUS_NO_MEMORY;
		goto ErrorCloseFile;
	}

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

	//dprintf("MmAllocateContiguousMemoryEx(%08x, %08x, %08x, %08x, %08x) = ", (ULONG) TempKernelSize,
	//	MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
	VirtKernel = MmAllocateContiguousMemoryEx((ULONG) TempKernelSize,MIN_KERNEL, 
			MAX_KERNEL, 0, PAGE_READWRITE);
	//dprintf("%08x\n", VirtKernel);
	if (!VirtKernel) {
		Error = STATUS_NO_MEMORY;
		goto ErrorCloseFile;
	}

	dprintf("Reading");
	ReadPtr = (PUCHAR) VirtKernel;
	
	
	for (i=0; i < TempKernelSize; i+=READ_CHUNK_SIZE, ReadPtr+=READ_CHUNK_SIZE) {
		ReadSize = READ_CHUNK_SIZE;
		Error = NtReadFile(KernelFile, NULL, NULL, NULL, &IoStatusBlock,
			ReadBuffer, ReadSize, NULL);
		memcpy(ReadPtr, ReadBuffer, ReadSize);
		dprintf(".");
		if (!NT_SUCCESS(Error)) break;
	}
	
	dprintf("done. (%i bytes @ %08x)\n", i, VirtKernel);

	/* Done; return size to caller */
	*FileSize = i; /* just an estimation */
	*FilePos = (int)VirtKernel;
	Error = STATUS_SUCCESS;
	goto ErrorCloseFile;


/* Undoes the allocations and returns an error code */
ErrorFreeMemory:
	MmFreeContiguousMemory(ReadBuffer);
	MmFreeContiguousMemory(VirtKernel);

ErrorCloseFile:
/*	NtClose(KernelFile); */

ErrorNothing:
	return Error;
}

static NTSTATUS LoadKernelXBE(long *FilePos, long *FileSize) {

	PVOID VirtKernel;
	/* Size of the kernel file */
	ULONGLONG TempKernelStart;
	ULONGLONG TempKernelSize;
	ULONGLONG TempKernelSizev;
	PHYSICAL_ADDRESS max_kernel;
	TempKernelSize = *FileSize;
        
	/* Try to allocate contiguous physical memory for the kernel image.
	   We need an address from 1 meg or above.
	   NOTE: We CANNOT use the memory allocated here in most API calls,
	   including ReadFile().*/
	
	max_kernel = MAX_KERNEL;
	//dprintf("MmAllocateContiguousMemoryEx(%08x, %08x, %08x, %08x, %08x) = ", (ULONG) TempKernelSize,
	//	MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
	VirtKernel = MmAllocateContiguousMemoryEx((ULONG) TempKernelSize,
		MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
	//dprintf("%08x\n", VirtKernel);
	
	if (!VirtKernel) return STATUS_NO_MEMORY;

	*FilePos = (int)VirtKernel;
	

	memcpy(&TempKernelStart,(void*)0x011080,4);	// This is the Where the Real kernel Starts in the XBE
	memcpy(&TempKernelSizev,(void*)0x011084,4);	// This is the Real kernel Size
	memcpy(&TempKernelSize,(void*)0x011088,4);	// this is the kernel Size we pass to the Kernel loader
	*FileSize= TempKernelSize;
	
	// We fille the compleate space with 0xff
	memcpy((void*)*FilePos,(void*)0x010000+TempKernelStart,TempKernelSizev);
	
	memset((void*)*FilePos+TempKernelSizev,0xff,TempKernelSize-TempKernelSizev);
	
	// We force the Cache to write back the changes to RAM
	asm volatile ("wbinvd\n");



/*
	memcpy(&TempKernelSize,(void*)0x011084,4);
	dprintf("%08x\n", TempKernelSize);
	memcpy(&TempKernelSize,(void*)0x011088,4);
	dprintf("%08x\n", TempKernelSize);
//while(1);	
  */
        return STATUS_SUCCESS;
}




static NTSTATUS LoadIinitrdXBE(long *FilePos, long *FileSize) {

	PVOID VirtKernel;
	/* Size of the initrd file */
	ULONGLONG TempInitrdStart;
	ULONGLONG TempInitrdSize;

	PHYSICAL_ADDRESS max_kernel;
	TempInitrdSize = *FileSize;
        
	/* Try to allocate contiguous physical memory for the kernel image.
	   We need an address from 1 meg or above.
	   NOTE: We CANNOT use the memory allocated here in most API calls,
	   including ReadFile().*/
	
	max_kernel = MAX_KERNEL;
	//dprintf("MmAllocateContiguousMemoryEx(%08x, %08x, %08x, %08x, %08x) = ", (ULONG) TempKernelSize,
	//	MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
	VirtKernel = MmAllocateContiguousMemoryEx((ULONG) TempInitrdSize,
		MIN_KERNEL, max_kernel, 0, PAGE_READWRITE);
	//dprintf("%08x\n", VirtKernel);
	
	if (!VirtKernel) return STATUS_NO_MEMORY;

	*FilePos = (int)VirtKernel;
	  
	memcpy(&TempInitrdStart,(void*)0x01108C,4);	// This is the Where the Real kernel Starts in the XBE
	memcpy(&TempInitrdSize,(void*)0x011090,4);	// This is the Real kernel Size


	*FileSize= TempInitrdSize;
	
	
	memcpy((void*)*FilePos,(void*)0x010000+TempInitrdStart,TempInitrdSize);
	
	// We force the Cache to write back the changes to RAM
	asm volatile ("wbinvd\n");



/*
	memcpy(&TempInitrdStart,(void*)0x01108c,4);
	dprintf("%08x\n", TempInitrdStart);
	memcpy(&TempInitrdSize,(void*)0x011090,4);
	dprintf("%08x\n", TempInitrdSize);
//while(1);	
*/
        return STATUS_SUCCESS;
}

void die() {
	while(1);
}

int NewFramebuffer;
long KernelSize;
PHYSICAL_ADDRESS PhysKernelPos, PhysEscapeCodePos;
PVOID EscapeCodePos;

NTSTATUS GetConfig(CONFIGENTRY *entry);
NTSTATUS GetConfigXBE(CONFIGENTRY *entry);

void boot() {
	BYTE bAvPackType;
	int xres,yres;
	int i;
	unsigned char* s;
	long KernelPos;
	long InitrdSize, InitrdPos;
	PHYSICAL_ADDRESS PhysInitrdPos;
	NTSTATUS Error;
	int data_PAGE_SIZE;
	extern int EscapeCode, EscapeCodeEnd;
	extern void* newloc, ptr_newloc;
	CONFIGENTRY entry;
	unsigned char tempstr[10];
	unsigned int temp;
	
	currentvideomodedetails.m_nVideoModeIndex=VIDEO_MODE_640x480;
        currentvideomodedetails.m_pbBaseAddressVideo=(BYTE *)0xfd000000;
	currentvideomodedetails.m_dwFrameBufferStart = FRAMEBUFFER_START;

        BootVgaInitializationKernelNG((CURRENT_VIDEO_MODE_DETAILS *)&currentvideomodedetails);
	
	framebuffer = (unsigned int*)(0xF0000000+*(unsigned int*)0xFD600800);
	memset(framebuffer,0,640*480*4);

	memset(&entry,0,sizeof(CONFIGENTRY));
	cx = 0;
	cy = 0;
//	dprintf("\n");
//	dprintf("Framebuffer at: 0x%08x\n", framebuffer);

	/* parse the configuration file */

	dprintf("Xbox Linux XBEBOOT  %s\n",VERSION);
	
	dprintf("%s -  http://xbox-linux.sf.net\n",__DATE__);
	dprintf("(C)2002 Xbox Linux Team - Licensed under the GPL\n");
	dprintf("\n");

#ifdef LOADHDD	
	Error = GetConfig(&entry);
#ifdef LOADHDD_CFGFALLBACK
        if (!NT_SUCCESS(Error)) {
        	Error = GetConfigXBE(&entry);
        	}
#endif
	if (!NT_SUCCESS(Error)) die();
	
	// Load the kernel image into RAM 
	KernelSize = MAX_KERNEL_SIZE;
	Error = LoadFile(entry.szKernel, &KernelPos, &KernelSize);

	/* get physical addresses */
	PhysKernelPos = MmGetPhysicalAddress((PVOID)KernelPos);

	if (!NT_SUCCESS(Error)) die();

	// ED : only if initrd 
	if(entry.szInitrd[0]) {
		InitrdSize = MAX_INITRD_SIZE;
		Error = LoadFile(entry.szInitrd, &InitrdPos, &InitrdSize);

		if (!NT_SUCCESS(Error)) die();
		PhysInitrdPos = MmGetPhysicalAddress((PVOID)InitrdPos);
		dprintf("PhysInitrdPos = 0x%08x\n", PhysInitrdPos);
	} else {
		InitrdSize = 0;
		PhysInitrdPos = 0;
	}
#endif


#ifdef LOADXBE
	Error = GetConfigXBE(&entry);

	if (!NT_SUCCESS(Error)) die();
	      
	// Load the kernel image into the correct RAM 
	KernelSize = MAX_KERNEL_SIZE;
	Error = LoadKernelXBE(&KernelPos, &KernelSize);
	/* get physical addresses */
	PhysKernelPos = MmGetPhysicalAddress((PVOID)KernelPos);
	
	// Load the Ramdisk into the correct RAM       
	InitrdSize = MAX_INITRD_SIZE;
       	Error = LoadIinitrdXBE( &InitrdPos, &InitrdSize);
	PhysInitrdPos = MmGetPhysicalAddress((PVOID)InitrdPos); 

#endif




	
//	dprintf("KernelPos 	= 0x%08x\n", KernelPos);
//	dprintf("PhysKernelPos 	= 0x%08x\n", PhysKernelPos);

  //      dprintf("\n");

	/* allocate memory for EscapeCode */
	EscapeCodePos = MmAllocateContiguousMemoryEx(PAGE_SIZE, RAMSIZE /4, RAMSIZE / 2, 16, PAGE_READWRITE);
	dprintf("EscapeCodePos = 0x%08x\n", EscapeCodePos);
	PhysEscapeCodePos = MmGetPhysicalAddress(EscapeCodePos);
//	dprintf("PhysEscapeCodePos = 0x%08x\n", PhysEscapeCodePos);

	data_PAGE_SIZE = PAGE_SIZE;
	Error = NtAllocateVirtualMemory((PVOID*)&PhysEscapeCodePos, 0, (PULONG) &data_PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//	dprintf("NtAllocateVirtualMemory() = 0x%08x\n", Error);

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

	setup(KernelPos, PhysInitrdPos, InitrdSize, entry.szAppend,xres,yres);
	
	currentvideomodedetails.m_nVideoModeIndex=entry.vmode;
        currentvideomodedetails.m_pbBaseAddressVideo=(BYTE *)0xfd000000;
        currentvideomodedetails.m_fForceEncoderLumaAndChromaToZeroInitially=0;
	
//	dprintf("Video mode %d x:%d y:%d\n",entry.vmode,xres,yres);
	
//	dprintf("Setup...");
	setup(KernelPos, PhysInitrdPos, InitrdSize, entry.szAppend,xres,yres);
//	dprintf("done.");

//	dprintf("Starting kernel...");

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

