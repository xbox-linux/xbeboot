# This is a quick and dirty makefile which doesn't use any
# of the real features of "make" - anyway, it works.
all:
	cpp xbeboot.S -o xbeboot.s
	as -o xbeboot.o xbeboot.s
	ld -Ttext 0x10000 -Tdata 0x10000 -e _start -s --oformat binary -o default.xbe xbeboot.o
	cat ../image-xbe.bin >> default.xbe
	dd if=/dev/zero bs=1024 count=200 >> default.xbe
	mkisofs -udf default.xbe > xbox-linux.iso

clean:
	rm -f *.o *.s default.xbe xbox-linux.iso *~
