#include "xbox.h"

.globl EscapeCode
.globl newloc, ptr_newloc

.code32

.text
.intel_syntax noprefix

/* This code escapes from the Xbox kernel environment, so that we
   are in plain 32 bit flat protected mode */

EscapeCode:

/*
 edi = NewFramebuffer
 ebp = PhysKernelPos
 esp = KernelSize
*/

/* turn off paging */
	mov	eax, cr0
	and	eax, 0xFFFFFFFF - CR0_ENABLE_PAGING
	mov	cr0, eax

/* flush the TLB */
	xor	eax, eax
	mov	cr3, eax

/* load new GDT/LDT/IDT */
	xor	eax, eax
	lldt	ax	/* no LDT */
	mov	dword ptr[GDT+0x00], eax
	mov	dword ptr[GDT+0x04], eax
	mov	dword ptr[GDT+0x08], eax
	mov	dword ptr[GDT+0x0C], eax
	dec	ax	/* EAX = 0x0000FFFF */
	mov	dword ptr[GDT+0x10], eax
	mov	dword ptr[GDT+0x14], 0x00CF9A00
	mov	dword ptr[GDT+0x18], eax
	mov	dword ptr[GDT+0x1C], 0x00CF9200

	mov	word ptr[GDT+0x20], 0x8000
	mov	eax, GDT
	mov	dword ptr[GDT+0x22], eax
	lidt	[eax]	/* eax points to at least 6 zeros, so we load an empty IDT */

	mov	eax, GDT+0x20
	lgdt	[eax]

/* initialize segment registers */
	mov	ax, 0x0018
	mov	ds, ax
	mov	es, ax
	mov	ss, ax

/* set EIP to a flat address and init CS */
	.byte	0xea /* = ljmp */
ptr_newloc:
	.long	0
	.word	0x0010

newloc:

/* move framebuffer to new position */
	mov	ebx, 0xFD600800
	mov	esi, dword ptr[ebx]
	mov	eax, edi
	mov	ecx, SCREEN_SIZE / 4
	.byte	0xF3, 0xA5  /* rep movsd */
	mov	dword ptr [ebx], eax

/* copy setup to its final position */
	xor	eax, eax
	mov	al, [ebp+0x1f1] // setup_setcs
	inc	al
	shl	eax, 9 // * 512
	mov	ecx, eax
	shr	ecx, 2 // /4
	mov	esi, ebp
	mov	edi, SETUP
	.byte	0xF3, 0xA5  /* rep movsd */

/* copy kernel to its final position */
	mov	esi, ebp
	add	esi, eax
	mov	ecx, esp
	sub	ecx, eax
	shr	ecx, 2 // /4
	mov	edi, PM_KERNEL_DEST
	.byte	0xF3, 0xA5  /* rep movsd */

/* jump to kernel */
	xor	ebx, ebx
	mov	esi, SETUP
	.byte	0xea //  = ljmp
	.long	PM_KERNEL_DEST
	.word	0x0010

