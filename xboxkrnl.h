#ifndef         __XBOXKRNL_H__
#define         __XBOXKRNL_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "types.h"

/* partially based on RectOS source code */

#define OBJ_CASE_INSENSITIVE		0x00000040L
#define PAGE_READWRITE			0x4
#define PAGE_EXECUTE_READWRITE		0x40
#define STATUS_NO_MEMORY		0xc0000017
#define STATUS_SUCCESS  		0x00000000
#define MEM_RESERVE			0x00002000
#define MEM_COMMIT			0x00001000

/* NT Data Types */

typedef VOID
(*PIO_APC_ROUTINE)(PVOID ApcContext,
                   struct _IO_STATUS_BLOCK* IoStatusBlock,
                   ULONG Reserved);

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
extern ULONG __attribute__((__stdcall__))
(*MmQueryAllocationSize)(PVOID   BaseAddress);
extern VOID __attribute__((__stdcall__))
(*MmPersistContiguousMemory)(PVOID BaseAddress,ULONG NumberOfBytes,BOOLEAN Persist);
extern VOID __attribute__((__stdcall__))
(*RtlMoveMemory)(VOID *Destination,CONST VOID *Source,SIZE_T Length);

#ifdef  __cplusplus
};
#endif

#endif          //__XBOXKRNL_H__

