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
typedef unsigned long SIZE_T;

typedef void VOID;
typedef VOID* PVOID;

typedef char CHAR;
typedef CHAR* PCHAR;
//typedef CHAR BYTE;
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

#endif // _Types_H_
