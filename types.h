#ifndef _Types_H_
#define _Types_H_

#ifndef HAVE_BOOLEAN
typedef int boolean;
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#define CONST const

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef int bool;

#define true 1
#define false 0

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned int u32;
typedef unsigned char u8;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned int size_t;
typedef unsigned int SIZE_T;


typedef BYTE* PBYTE;
typedef void VOID;
typedef VOID* PVOID;

typedef char CHAR, *PCHAR, *PSTR, *LPSTR, BOOL, *PBOOL, BOOLEAN, *PBOOLEAN;
typedef const char *PCSZ, *PCSTR, *LPCSTR;

typedef unsigned int UINT;

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

typedef PVOID HANDLE;
typedef HANDLE* PHANDLE;
typedef LARGE_INTEGER* PLARGE_INTEGER;
typedef ULONG PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

typedef struct _ANSI_STRING {
	WORD Length;
	WORD MaxLength;
	PSTR Buffer;
} ANSI_STRING, *PANSI_STRING, STRING, *PSTRING;

typedef ULONG ACCESS_MASK;

typedef LONG NTSTATUS;

#define NT_SUCCESS(StatCode)  ((NTSTATUS)(StatCode) >= 0)
#define OPTIONAL

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

// CreateDisposition values for NtCreateFile()
#define FILE_SUPERSEDE                          0x00000000
#define FILE_OPEN                               0x00000001
#define FILE_CREATE                             0x00000002
#define FILE_OPEN_IF                            0x00000003
#define FILE_OVERWRITE                          0x00000004
#define FILE_OVERWRITE_IF                       0x00000005
#define FILE_MAXIMUM_DISPOSITION                0x00000005

// CreateOption values for NtCreateFile()
// FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT is what CreateFile
// uses for most things when translating to NtCreateFile.
#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080
#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_FOR_RECOVERY                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800
#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000
#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000
#define FILE_COPY_STRUCTURED_STORAGE            0x00000041
#define FILE_STRUCTURED_STORAGE                 0x00000441
#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036

// Access flags
#define DELETE                                  0x00010000
#define SYNCHRONIZE                             0x00100000
#define GENERIC_ALL                             0x10000000
#define GENERIC_EXECUTE                         0x20000000
#define GENERIC_WRITE                           0x40000000
#define GENERIC_READ                            0x80000000
#define FILE_SHARE_READ                         0x00000001
#define FILE_SHARE_WRITE                        0x00000002
#define FILE_SHARE_DELETE                       0x00000004
#define FILE_ATTRIBUTE_NORMAL                   0x00000080
#define FILE_FLAG_RANDOM_ACCESS                 0x10000000

/* partially based on RectOS source code */

#define OBJ_CASE_INSENSITIVE		0x00000040L
#define PAGE_READWRITE			0x4
#define PAGE_EXECUTE_READWRITE		0x40
#define STATUS_NO_MEMORY		0xc0000017
#define STATUS_SUCCESS  		0x00000000
#define MEM_RESERVE			0x00002000
#define MEM_COMMIT			0x00001000

#define ObDosDevices                    ((HANDLE) 0xFFFFFFFD)

// ******************************************************************
// * FILE_INFORMATION_CLASS
// ******************************************************************
typedef enum _FILE_INFORMATION_CLASS
{
    FileDirectoryInformation        = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileCopyOnWriteInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileObjectIdInformation,
    FileTrackingInformation,
    FileOleDirectoryInformation,
    FileContentIndexInformation,
    FileInheritContentIndexInformation,
    FileOleInformation,
    FileMaximumInformation
}
FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _FILE_NETWORK_OPEN_INFORMATION {
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER AllocationSize;
        LARGE_INTEGER EndOfFile;
        ULONG FileAttributes;
        ULONG FixLinuxGccDummy;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

/* NT Data Types */

// Structure of an RC4 key
typedef struct _RC4_SBOX {
	// Opaque, but copiable, structure
	BYTE Data[256 + 4];
} RC4_SBOX, *PRC4_SBOX;

#endif // _Types_H_
