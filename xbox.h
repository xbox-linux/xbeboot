/* you can change this */
//#define DEBUG

/* but you better shouldn't change this */
#define CONFIG_FILE "linuxboot.cfg"

#define BUFFERSIZE 256 /* we have little stack */
#define CONFIG_BUFFERSIZE (BUFFERSIZE*16)

#ifdef DEBUG
#define dprintf printf
#else
#define dprintf printf
#endif

#ifdef DEBUG
#define splash
#define splash_init()
#else
#define splash show_splash
#define splash_init do_splash_init
#endif

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_HEIGHT_576 576
#define SCREEN_HEIGHT_480 480

/* a retail Xbox has 64 MB of RAM */
#define RAMSIZE (64 * 1024*1024)
#define FB_RAM (4 * 1024 * 1024)

#define FRAMEBUFFER_START ( /*0xf0000000 |*/ /*(0x04000000-(640*480*4) -(640*4*4)-(256*4))*/ (*((DWORD *)0xfd600800)) & 0x7fffffff )
#define SCREEN_SIZE (SCREEN_HEIGHT_480 * SCREEN_WIDTH * 4)
#define FRAMEBUFFER_SIZE FB_RAM
#define NEW_FRAMEBUFFER (RAMSIZE - (FRAMEBUFFER_SIZE))
#define RAMSIZE_USE (NEW_FRAMEBUFFER)

#define MAX_KERNEL_SIZE (2*1024*1024)
#define MAX_INITRD_SIZE (6*1024*1024)

/* position of kernel setup data */
#define SETUP 0x90000
/* the GDT must not be overwritten, so we place it at the ISA VGA memory location */
#define GDT 0xA0000
/* position of protected mode kernel */
#define PM_KERNEL_DEST 0x100000

/* Lowest allowable address of the kernel (at or above 1 meg) */
#define MIN_KERNEL PM_KERNEL_DEST
/* Highest allowable address */
#define MAX_KERNEL (RAMSIZE_USE-1)

/* i386 constants */

/* CR0 bit to enable paging */
#define CR0_ENABLE_PAGING		0x80000000
/* Size of a page on x86 */
#define PAGE_SIZE			4096

/* Size of the read chunks to use when reading the kernel; bigger = a lot faster */
#define READ_CHUNK_SIZE 128*1024

