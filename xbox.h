/* configuration */
#undef DEBUG
#define CONFIG_D


#ifdef CONFIG_C
#define CONFIG_FILE "\\Device\\Harddisk0\\Partition2\\linuxboot.cfg"
#endif
#ifdef CONFIG_D
#define CONFIG_FILE "\\Device\\Cdrom0\\linuxboot.cfg"
#endif
#ifdef CONFIG_E
#define CONFIG_FILE "\\Device\\Harddisk0\\Partition1\\linuxboot.cfg"
#endif
#ifdef CONFIG_F
#define CONFIG_FILE "\\Device\\Harddisk0\\Partition6\\linuxboot.cfg"
#endif
#ifdef CONFIG_X
#define CONFIG_FILE "\\Device\\Harddisk0\\Partition3\\linuxboot.cfg"
#endif
#ifdef CONFIG_Y
#define CONFIG_FILE "\\Device\\Harddisk0\\Partition4\\linuxboot.cfg"
#endif
#ifdef CONFIG_Z
#define CONFIG_FILE "\\Device\\Harddisk0\\Partition5\\linuxboot.cfg"
#endif

#define BUFFERSIZE 512
#define CONFIG_BUFFERSIZE (BUFFERSIZE*16)

#ifdef DEBUG
#define dprintf printf
#else
#define dprintf
#endif

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 576

/* memory layout */

/* a retail Xbox has 64 MB of RAM */
#define RAMSIZE (64 * 1024*1024)

#define SCREEN_SIZE (SCREEN_HEIGHT * SCREEN_WIDTH * 4)
#define FRAMEBUFFER_SIZE ((SCREEN_SIZE+0xFFFF)& 0xFFFF0000)
#define NEW_FRAMEBUFFER (RAMSIZE - FRAMEBUFFER_SIZE)
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

