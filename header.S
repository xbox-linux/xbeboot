// Xbox Linux XBE Bootloader
//
//  Copyright (C) 2002 Michael Steil & anonymous
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// The latest version of the GPL can be retrieved at:
// http://www.gnu.org/licenses/gpl.html .
//
// Xbox is a trademark or registered trademark of Microsoft Corporation.
// No relationship between the author(s) and Microsoft Corporation exists or is
// implied.

// Constants

.code32

.text
// This includes a minimal XBE header. It's not complete, i.e. software that wants
// to decode the header, such as "xbedump" (or probably Microsoft's certification
// tools) are likely to fail, but it is a valid header for the Xbox kernel.
#define BASE_ADDRESS 0x10000
#define FILE_SIZE (1024*1024)
header_start:
  .ascii "XBEH"

// digital signature
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

  .long BASE_ADDRESS                    // base address
  .long headers_end - header_start      // size of headers
  .long FILE_SIZE+32*1024               // size of image
  .long header_end - header_start       // size of image header
  .long 0                               // time date stamp                 *unimportant*
  .long certificate                     // certificate address
  .long 1                               // number of section headers
  .long section_headers                 // section headers address
  .long 0                               // initialization flags
  .long 0x11100 ^ 0xA8FC57AB            // entry point address XOR key **GAS ISSUE: "entry" won't work**
  .long 0x18000                         // thread local storage address
  .long 0                               // size of stack commit (PE copy)  *unimportant*
  .long 0                               // size of heap reserve (PE copy)  *unimportant*
  .long 0                               // size of heap commit (PE copy)   *unimportant*
  .long 0                               // original base address (PE copy) *unimportant*
  .long 0                               // original size of image (PE copy)*unimportant*
  .long 0                               // original checksum (PE copy)     *unimportant*
  .long 0                               // original time stamp (PE copy)   *unimportant*
  .long 0                               // debug path name address         *unimportant*
  .long 0                               // debug file name address         *unimportant*
  .long 0                               // debug unicode file name address *unimportant*
  .long 0x11000 ^ 0x5B6D40B6            // kernel image thunk address XOR key **GAS ISSUE: see above**
  .long 0                               // non-kernel import directory address
  .long 0                               // number of library versions      *unimportant*
  .long 0                               // library versions address        *unimportant*
  .long 0                               // kernel library version address  *unimportant*
  .long 0                               // XAPI library address            *unimportant*
  .long 0                               // logo bitmap address             *unimportant*
  .long 0                               // logo bitmap size                *unimportant*
header_end:

certificate:
  .long certificate_end - certificate   // size of certificate
  .long 0                               // time date stamp                 *unimportant*
  .long 0                               // title id                        *unimportant?*

  // title name (unicode string, 40 chars) *unimportant, but beautiful*
  .word 'L','i','n','u','x',' ','D','e','m','o',0
  .word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .word 0,0,0,0,0,0,0,0,0,0,0,0,0

  // alternate title ids
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

//  .long 0x80000005   // allowed media types: HD or any CD/DVD; don't reboot on eject
  .long 0x00000005   // allowed media types: HD or any CD/DVD
  .long 7   // game regions: all
  .long -1  // game ratings
  .long 0   // disk number
  .long 0   // version

  // LAN key
  .long 0,0,0,0

  // signature key
  .long 0,0,0,0

  // 16 title alternate signature keys
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .long 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
certificate_end:

section_headers:
//  .long 0x16            // flags: RO_HEAD_PAGE, EXECUTABLE, PRELOAD
  .long 0x07            // flags: WRITABLE, EXECUTABLE, PRELOAD
  .long section_1_start // virtual address
  .long FILE_SIZE       // virtual size
  .long 0x1000          // file pointer to raw data
  .long FILE_SIZE       // size of raw data
  .long 0               // address of section name *unimportant*
  .long 0               // unknown
  .long rc1             // head shared page reference count address *not NULL important*
  .long rc2             // tail shared page reference count address *not NULL important*

  // unknown
  .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  .byte 0,0,0,0

section_headers_end:

rc1:
  .word 0
rc2:
  .word 0

.align 4
headers_end:


.align 0x1000
section_1_start:		// this should be 0x11000
// kernel thunk table
.globl MmAllocateContiguousMemoryEx
MmAllocateContiguousMemoryEx:
	.long	0x80000000 + 166
.globl MmGetPhysicalAddress
MmGetPhysicalAddress:
	.long	0x80000000 + 173
.globl NtAllocateVirtualMemory
NtAllocateVirtualMemory:
	.long	0x80000000 + 184
.globl RtlInitAnsiString
RtlInitAnsiString:
	.long	0x80000000 + 289
.globl NtCreateFile
NtCreateFile:
	.long	0x80000000 + 190
.globl NtQueryInformationFile
NtQueryInformationFile:
	.long	0x80000000 + 211
.globl NtClose
NtClose:
	.long	0x80000000 + 187
.globl NtReadFile
NtReadFile:
	.long	0x80000000 + 219
.globl MmFreeContiguousMemory
MmFreeContiguousMemory:
	.long	0x80000000 + 171

	.long	0		// end of table

.globl _start
.align 0x100
entry:				// this should be 0x11100
.intel_syntax noprefix
_start:

	jmp boot
	jmp newflash
newflash:
// flash the LEDs
// use this as debug code to check whether control reaches a certain point
#define LED_RED_C0	0x80
#define LED_RED_C1	0x40
#define LED_RED_C2	0x20
#define LED_RED_C3	0x10
#define LED_GREEN_C0	0x08
#define LED_GREEN_C1	0x04
#define LED_GREEN_C2	0x02
#define LED_GREEN_C3	0x01
	mov dx, 0xc004
	mov al, 0x20
	out dx, al
	mov dx, 0xc008
	mov al, 8
	out dx, al
	mov dx, 0xc006
	mov al, LED_RED_C0+LED_GREEN_C1+LED_RED_C2+LED_GREEN_C2
	out dx, al
	mov dx, 0xc000
	in ax, dx
	out dx, al
	mov dx, 0xc002
	mov al, 0x1a
	out dx, al

	mov eax, 1000000
l2:
	dec eax
	jne l2

	mov dx, 0xc004
	mov al, 0x20
	out dx, al
	mov dx, 0xc008
	mov al, 7
	out dx, al
	mov dx, 0xc006
	mov al, 1
	out dx, al
	mov dx, 0xc000
	in ax, dx
	out dx, al
	mov dx, 0xc002
	mov al, 0x1a
	out dx, al
lx:
	jmp lx

