### compilers and options
CC	= gcc
CFLAGS	= -g -O2
LD	= ld
LDFLAGS	= -s -S -T ldscript.ld
OBJCOPY	= objcopy

### objects
#OBJECTS	= header.o load.o escape.o boot.o
OBJECTS	= header.o load.o setup.o escape.o
RESOURCES =

# target:
all	: linux.iso


linux.iso: default.xbe vmlinuz initrd
	mkisofs -f -udf $< vmlinuz initrd > $@

default.elf : ${OBJECTS} ${RESOURCES}
	${LD} -o $@ ${OBJECTS} ${RESOURCES} ${LDFLAGS}

clean	:
	rm -rf *.o *~ core *.core ${OBJECTS} ${RESOURCES} default.elf default.xbe linux.iso

### rules:
%.o	: %.c
	${CC} ${CFLAGS} -o $@ -c $<

%.o	: %.S
	${CC} -DASSEMBLER ${CFLAGS} -o $@ -c $<

%.xbe : %.elf
	${OBJCOPY} --output-target=binary --strip-all $< $@
	dd if=/dev/zero bs=1k count=1024 >> $@
	@ls -l $@
