#ifndef         __XBOXKRNL_H__
#define         __XBOXKRNL_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "types.h"

typedef VOID
(*PIO_APC_ROUTINE)(PVOID ApcContext,
                   struct _IO_STATUS_BLOCK* IoStatusBlock,
                   ULONG Reserved);

/* Xbox Kernel Functions */
extern VOID __attribute__((__stdcall__))
(*RtlInitAnsiString)(PANSI_STRING, PCSZ);
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
extern VOID __attribute__((__stdcall__))
(*XcRC4Crypt)(PRC4_SBOX Sbox, SIZE_T DataLength, CONST BYTE *Data);
extern VOID __attribute__((__stdcall__))
(*XcRC4Key)(PRC4_SBOX Sbox, SIZE_T KeyLength, CONST BYTE *Key);
extern PVOID __attribute__((__stdcall__))
(*ExAllocatePoolWithTag)(SIZE_T NumberOfBytes,ULONG Tag);
extern VOID __attribute__((__stdcall__))
(*ExFreePool)(PVOID P);
extern NTSTATUS __attribute__((__stdcall__))(*NtWriteFile)(
        HANDLE  FileHandle,
        PVOID   Event,
        PVOID   ApcRoutine,
        PVOID   ApcContext,
        PVOID   IoStatusBlock,
        PVOID   Buffer,
        ULONG   Length,
        PVOID   ByteOffset
);
extern NTSTATUS __attribute__((__stdcall__))(*NtCreateFile) (
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize OPTIONAL,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions
);
extern NTSTATUS __attribute__((__stdcall__))(*NtQueryInformationFile)(
        HANDLE FileHandle,
        PIO_STATUS_BLOCK IoStatusBlock,
        PVOID FileInformation,
        ULONG FileInformationLength,
        FILE_INFORMATION_CLASS FileInformationClass
);
extern NTSTATUS __attribute__((__stdcall__))(*NtSetInformationFile)(
        HANDLE  FileHandle,
        PVOID   IoStatusBlock,
        PVOID   FileInformation,
        ULONG   Length,
        ULONG   FileInformationClass
);
extern NTSTATUS __attribute__((__stdcall__))(*IoCreateSymbolicLink)(
	PANSI_STRING SymbolicLinkName,
	PANSI_STRING DeviceName
);
extern NTSTATUS __attribute__((__stdcall__))(*IoDeleteSymbolicLink)(
	PANSI_STRING SymbolicLinkName
);
extern NTSTATUS __attribute__((__stdcall__))(*IoDismountVolumeByName)(
	PANSI_STRING DeviceName
);


#ifdef  __cplusplus
};
#endif

#endif          //__XBOXKRNL_H__

