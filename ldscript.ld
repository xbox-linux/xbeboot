
OUTPUT_FORMAT ("elf32-i386");
OUTPUT_ARCH(i386);
MEMORY
{
	ram (rwx) : ORIGIN = 0, LENGTH = 64M
	rom (rx)  : ORIGIN = 0x10000, LENGTH = 16M
}

RAM_BASE = 0x10000; /* overwrite XBE header with variables */

HEADER = 0x10000;
HEADER_LOAD = 0;

SECTIONS {

	.text ( HEADER ) : AT ( HEADER_LOAD ) {
		_start_text = . ;
		*(.text);
		*(.rodata);
		_end_text = . ;
	}
		/* ram allocations */

	.data ( HEADER + SIZEOF(.text) ) : AT ( HEADER_LOAD + SIZEOF(.text) ) {
		_start_data = .;
		*(.data);
		*(.sdata);
		*(.sdata2);
		*(.got);
		*(.*);
		_end_data = .;
		}

	.bss ( HEADER + SIZEOF(.text) + SIZEOF(.data)  ) : AT (HEADER_LOAD + SIZEOF(.text) + SIZEOF(.data)) {
		_bss = .;
		*(.bss)
		*(.sbss)
		*(COMMON)
		_ebss = . ;
		_heap = . ;
	}
	
	_start_load_data = LOADADDR(.data);
	_end_load_data = LOADADDR(.data) + SIZEOF(.data) +  SIZEOF(.bss);
}
