### compilers and options
CC	= gcc
CFLAGS	= -O2 
#-mcpu=pentium -Wall -Werror 
CFLAGS += -DXBE
LD	= ld
LDFLAGS	= -s -S -T ldscript.ld
OBJCOPY	= objcopy

### objects
OBJECTS	= header.o load.o setup.o escape.o parse.o I2C_io.o BootParser.o BootString.o BootMemory.o VideoInitialization.o BootVgaInitialization.o

RESOURCES = 
TOPDIR  := $(shell /bin/pwd)

# target:
all	: clean image default.xbe

iso	: linux.iso

linux.iso: default.xbe
	mkisofs -udf $< linuxboot.cfg vmlinuz initrd > $@

image:
	$(CC) $(TOPDIR)/imagebld/imagebld.c $(TOPDIR)/imagebld/sha1.c -o $(TOPDIR)/imagebld/image
	
default.elf : ${OBJECTS} ${RESOURCES}
	${LD} -o $@ ${OBJECTS} ${RESOURCES} ${LDFLAGS}

clean	:
	rm -rf *.o *~ core *.core image ${OBJECTS} ${RESOURCES} default.elf default.xbe linux.iso $(TOPDIR)/imagebld/image xbeboot.xbe

### rules:
%.o	: %.c
	${CC} ${CFLAGS} -o $@ -c $<

%.o	: %.S
	${CC} -DASSEMBLER ${CFLAGS} -o $@ -c $<

%.xbe : %.elf
	${OBJCOPY} --output-target=binary --strip-all $< $@
	#dd if=/dev/zero bs=1k count=30000 >> $@
	$(TOPDIR)/imagebld/image $(TOPDIR)/default.xbe  $(TOPDIR)/vmlinuz $(TOPDIR)/initrd  $(TOPDIR)/linuxboot.cfg
	cp default.xbe xbeboot.xbe
	@ls -l $@
