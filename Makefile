### compilers and options
CC	= gcc
CFLAGS	= -O2 -mcpu=pentium -Wall -Werror -DXBE
LD	= ld
LDFLAGS	= -s -S -T ldscript.ld
OBJCOPY	= objcopy


RESOURCES = 
TOPDIR  := $(shell /bin/pwd)

### objects
OBJECTS	 = $(TOPDIR)/header.o 
OBJECTS += $(TOPDIR)/load.o 
OBJECTS += $(TOPDIR)/setup.o 
OBJECTS += $(TOPDIR)/escape.o 
OBJECTS += $(TOPDIR)/I2C_io.o 
OBJECTS += $(TOPDIR)/BootParser.o 
OBJECTS += $(TOPDIR)/BootString.o 
OBJECTS += $(TOPDIR)/BootMemory.o 
OBJECTS += $(TOPDIR)/BootEEPROM.o
OBJECTS += $(TOPDIR)/VideoInitialization.o 
OBJECTS += $(TOPDIR)/BootVgaInitialization.o


# target:
all	: clean image default.xbe

iso	: linux.iso

linux.iso: default.xbe
	mkisofs -udf $< linuxboot.cfg vmlinuz initrd > $@

image:
	$(CC) $(TOPDIR)/imagebld/imagebld.c $(TOPDIR)/imagebld/sha1.c -o $(TOPDIR)/imagebld/image
	
default.elf : ${OBJECTS} ${RESOURCES}
	${LD} -o default.elf ${OBJECTS} ${RESOURCES} ${LDFLAGS}

clean	:
	rm -rf *.o *~ core *.core image ${OBJECTS} ${RESOURCES} default.elf 
	rm -f default.xbe 
	rm -f linux.iso 
	rm -f $(TOPDIR)/imagebld/image 
	rm -f xbeboot.xbe
	#mkdir $(TOPDIR)/obj -p
	
### rules:
%.o	: %.c
	${CC} ${CFLAGS} -o $@ -c $<

%.o	: %.S
	${CC} -DASSEMBLER ${CFLAGS} -o $@ -c $<

%.xbe : %.elf
	${OBJCOPY} --output-target=binary --strip-all $< $@
	$(TOPDIR)/imagebld/image -build $(TOPDIR)/default.xbe  $(TOPDIR)/vmlinuz $(TOPDIR)/initrd  $(TOPDIR)/linuxboot.cfg
	cp default.xbe xbeboot.xbe
	@ls -l $@
