#include "xbox.h"

/* parameters to be passed to the kernel */
struct kernel_setup_t {
	unsigned char  orig_x;                  /* 0x00 */
	unsigned char  orig_y;                  /* 0x01 */
	unsigned short ext_mem_k;   		/* 0x02 -- EXT_MEM_K sits here */
	unsigned short orig_video_page;         /* 0x04 */
	unsigned char  orig_video_mode;         /* 0x06 */
	unsigned char  orig_video_cols;         /* 0x07 */
	unsigned short unused2;                 /* 0x08 */
	unsigned short orig_video_ega_bx;       /* 0x0a */
	unsigned short unused3;                 /* 0x0c */
	unsigned char  orig_video_lines;        /* 0x0e */
	unsigned char  orig_video_isVGA;        /* 0x0f */
	unsigned short orig_video_points;       /* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	unsigned short lfb_width;               /* 0x12 */
	unsigned short lfb_height;              /* 0x14 */
	unsigned short lfb_depth;               /* 0x16 */
	unsigned long  lfb_base;                /* 0x18 */
	unsigned long  lfb_size;                /* 0x1c */
	unsigned short cmd_magic;	/*  32: Command line magic 0xA33F */
	unsigned short cmd_offset;	/*  34: Command line offset from 0x90000 */
	unsigned short lfb_linelength;          /* 0x24 */
	unsigned char  red_size;                /* 0x26 */
	unsigned char  red_pos;                 /* 0x27 */
	unsigned char  green_size;              /* 0x28 */
	unsigned char  green_pos;               /* 0x29 */
	unsigned char  blue_size;               /* 0x2a */
	unsigned char  blue_pos;                /* 0x2b */
	unsigned char  rsvd_size;               /* 0x2c */
	unsigned char  rsvd_pos;                /* 0x2d */
	unsigned short vesapm_seg;              /* 0x2e */
	unsigned short vesapm_off;              /* 0x30 */
	unsigned short pages;                   /* 0x32 */
	unsigned short vesa_attributes;         /* 0x34 */ // NEW BY ED
	char __pad2[443];
	unsigned char  setup_sects; /* 497: setup size in sectors (512) */
	unsigned short root_flags;	/* 498: 1 = ro ; 0 = rw */
	unsigned short kernel_para;	/* 500: kernel size in paragraphs (16) */
	unsigned short swap_dev;	/* 502: */
	unsigned short ram_size;	/* 504: */
	unsigned short vid_mode;	/* 506: */
	unsigned short root_dev;	/* 508: */
	unsigned short boot_flag;	/* 510: signature*/
	unsigned short jump;        /* 512: jump to startup code */
	char signature[4];          /* 514: "HdrS" */
	unsigned short version;     /* 518: header version */
	unsigned short x,y,z;       /* 520: LOADLIN hacks */
	unsigned short ver_offset;  /* 526: kernel version string */
	unsigned char loader;       /* 528: loader type */
	unsigned char flags;        /* 529: loader flags */
	unsigned short a;           /* 530: more LOADLIN hacks */
	unsigned long code32_start;        /* 532: kernel start, filled in by loader */
	unsigned long ramdisk;      /* 536: RAM disk start address */
	unsigned long ramdisk_size; /* 540: RAM disk size */
	unsigned short b,c;         /* 544: bzImage hacks */
	unsigned short heap_end_ptr;/* 548: end of free area after setup code */
	unsigned char __pad3[2];
	unsigned int cmd_line_ptr;  /* 552: pointer to command line */
	unsigned int initrd_addr_max;/*556: highest address that can be used by initrd */
};

extern void* framebuffer;

void setup(void* KernelPos, void* PhysInitrdPos, void* InitrdSize, char* kernel_cmdline, int xres, int yres) {
    int i;
    int cmd_line_ptr;
    struct kernel_setup_t *kernel_setup = (struct kernel_setup_t*)KernelPos;

    /* init kernel parameters */
    kernel_setup->loader = 0xFF;		/* must be != 0 */
    kernel_setup->heap_end_ptr = 0xffff;	/* 64K heap */
    kernel_setup->flags = 0x81;			/* loaded high, heap existant */
    kernel_setup->code32_start = PM_KERNEL_DEST;
    kernel_setup->ext_mem_k = RAMSIZE_USE/1024-1024; /* *extended* (minus first MB) memory in kilobytes */
    /* initrd */
    /* ED : only if initrd */

    if((long)InitrdSize != 0) {
	    kernel_setup->ramdisk = (long)PhysInitrdPos;
	    kernel_setup->ramdisk_size = (long)InitrdSize;
	    kernel_setup->initrd_addr_max = RAMSIZE_USE;
    }

    /* Framebuffer setup */
    kernel_setup->orig_video_isVGA = 0x23;
    kernel_setup->orig_x = 0;
    kernel_setup->orig_y = 0;
    kernel_setup->vid_mode = 0x312;		/* 640x480x16M Colors */
    kernel_setup->orig_video_mode = kernel_setup->vid_mode-0x300;
    kernel_setup->orig_video_cols = 80;
    kernel_setup->orig_video_lines = 30;
    kernel_setup->orig_video_ega_bx = 0;
    kernel_setup->orig_video_points = 16;
    kernel_setup->lfb_depth = 32;
    kernel_setup->lfb_width = xres;
    kernel_setup->lfb_height = yres;
    kernel_setup->lfb_base = (0xf0000000|*(unsigned int*)0xFD600800);
    kernel_setup->lfb_size = (4 * 1024 * 1024)/0x10000;
    kernel_setup->lfb_linelength = xres*4;
    kernel_setup->pages=1;
    kernel_setup->vesapm_seg = 0;
    kernel_setup->vesapm_off = 0;
    kernel_setup->blue_size = 8;
    kernel_setup->blue_pos = 0;
    kernel_setup->green_size = 8;
    kernel_setup->green_pos = 8;
    kernel_setup->red_size = 8;
    kernel_setup->red_pos = 16;
    kernel_setup->rsvd_size = 8;
    kernel_setup->rsvd_pos = 24;

    kernel_setup->boot_flag = 0xAA55;
    kernel_setup->version = 0x0203;

    memcpy(kernel_setup->signature,"HdrS",4);

    /* set command line */
    cmd_line_ptr = (kernel_setup->setup_sects) * 512; /* = 512 bytes from top of SETUP */
    kernel_setup->cmd_offset = (unsigned short) cmd_line_ptr;
    kernel_setup->cmd_magic = 0xA33F;
    kernel_setup->cmd_line_ptr = 0;
    memcpy((char*)(KernelPos+cmd_line_ptr), kernel_cmdline, 512);
    *(char*)(KernelPos+cmd_line_ptr+511) = 0;
}
