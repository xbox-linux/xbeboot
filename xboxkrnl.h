#ifndef         __XBOXKRNL_H__
#define         __XBOXKRNL_H__

#ifdef  __cplusplus
extern "C" {
#endif

/* partially based on RectOS source code */

#define OBJ_CASE_INSENSITIVE		0x00000040L
#define PAGE_READWRITE			0x4
#define PAGE_EXECUTE_READWRITE		0x40
#define STATUS_NO_MEMORY		0xc0000017
#define STATUS_SUCCESS  		0x00000000
#define MEM_RESERVE			0x00002000
#define MEM_COMMIT			0x00001000

#define FALSE 0

/* NT Data Types */

typedef unsigned int u32;
typedef unsigned long DWORD;
typedef unsigned char u8;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned long SIZE_T;
typedef unsigned short WORD;

#define CONST const
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

typedef void VOID;
typedef VOID* PVOID;

typedef char CHAR;
typedef CHAR* PCHAR;
typedef CHAR BYTE;
typedef const CHAR *PCSZ;

typedef unsigned char UCHAR;
typedef UCHAR* PUCHAR;

typedef unsigned short USHORT;

typedef long LONG;
typedef unsigned long ULONG;
typedef ULONG* PULONG;
typedef PULONG ULONG_PTR; /* TODO: NOT SURE!! */

typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;

typedef union _LARGE_INTEGER {
	LONGLONG QuadPart;
} LARGE_INTEGER;

typedef UCHAR BOOLEAN;
typedef PVOID HANDLE;
typedef HANDLE* PHANDLE;
typedef LARGE_INTEGER* PLARGE_INTEGER;
typedef ULONG PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

typedef struct _STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR Buffer;
} STRING, *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef LONG NTSTATUS;

#define NT_SUCCESS(StatCode)  ((NTSTATUS)(StatCode) >= 0)

typedef struct _OBJECT_ATTRIBUTES
{
	HANDLE RootDirectory;
	PANSI_STRING ObjectName;
	ULONG Attributes;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK
{
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef VOID
(*PIO_APC_ROUTINE)(PVOID ApcContext,
                   struct _IO_STATUS_BLOCK* IoStatusBlock,
                   ULONG Reserved);


typedef struct {
        BYTE * m_pBitmapData;
        int m_nWidth;
        int m_nHeight;
        int m_nBytesPerPixel;
} JPEG;


/* Xbox Kernel Functions */

extern VOID __attribute__((__stdcall__))
(*RtlInitAnsiString)(PANSI_STRING, PCSZ);
extern NTSTATUS __attribute__((__stdcall__))
(*NtCreateFile)(PHANDLE, ULONG, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, PLARGE_INTEGER, ULONG, ULONG, ULONG, ULONG);
extern NTSTATUS __attribute__((__stdcall__))
(*NtClose)(HANDLE);
extern PVOID __attribute__((__stdcall__))
(*MmAllocateContiguousMemoryEx)(ULONG, PHYSICAL_ADDRESS, PHYSICAL_ADDRESS, ULONG, ULONG);
extern NTSTATUS __attribute__((__stdcall__))
(*NtReadFile)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, PLARGE_INTEGER);
extern VOID __attribute__((__stdcall__))
(*MmFreeContiguousMemory)(PVOID);
extern PHYSICAL_ADDRESS __attribute__((__stdcall__))
(*MmGetPhysicalAddress)(PVOID);
extern NTSTATUS __attribute__((__stdcall__))
(*NtAllocateVirtualMemory)(PVOID*, ULONG, PULONG, ULONG, ULONG);
extern PANSI_STRING XeImageFileName;
extern PUCHAR XboxEEPROMKey[16];
extern PUCHAR XboxHDKey[16];
extern NTSTATUS __attribute__((__stdcall__))
(*HalWriteSMBusValue)(UCHAR, UCHAR, BOOLEAN, UCHAR);
extern VOID __attribute__((__stdcall__))
(*RtlZeroMemory)(PVOID Destination,ULONG Length);
extern VOID __attribute__((__stdcall__))
(*AvSetDisplayMode)(PVOID RegisterBase,ULONG Step,ULONG Mode, ULONG Format, ULONG Pitch, ULONG FrameBuffer);
extern ULONG __attribute__((__stdcall__))
(*MmQueryAllocationSize)(PVOID   BaseAddress);
extern VOID __attribute__((__stdcall__))
(*MmPersistContiguousMemory)(PVOID BaseAddress,ULONG NumberOfBytes,BOOLEAN Persist);
extern VOID __attribute__((__stdcall__))
(*RtlMoveMemory)(VOID *Destination,CONST VOID *Source,SIZE_T Length);
extern VOID __attribute__((__stdcall__))
(*AvSetSavedDataAddress)(PVOID Address);
extern VOID __attribute__((__stdcall__))
(*AvSendTVEncoderOption)(PVOID RegisterBase,ULONG Option,ULONG Param,ULONG *Result);

#ifdef  __cplusplus
};
#endif

#endif          //__XBOXKRNL_H__

