#include "boot.h"

void __inline * memcpy(void *dest, const void *src, size_t size) {
	__asm__  (
		"    push %%esi    \n"
		"    push %%edi    \n"
		"    push %%ecx    \n"
		"    mov %0, %%esi \n"
		"    mov %1, %%edi \n"
		"    mov %2, %%ecx \n"
		"    push %%ecx    \n"
		"    shr $2, %%ecx \n"
		"    rep movsl     \n"
		"    pop %%ecx     \n"
		"    and $3, %%ecx \n"
		"    rep movsb     \n"
		"    pop %%ecx     \n"
		"    pop %%edi     \n"
		"    pop %%esi     \n"
		: : "S" (src), "D" (dest), "c" (size)
	);
	return dest;
}

void * memset(void *dest, int data,  size_t size)
{
  
  	char *p = dest;
	while (size -- > 0)
	{
		*p ++ = data;
	}
 	return dest;
} 

