CXX       = gcc
#OPT       = -O -g
#FLAGS     = $(OPT) -ansi -W -Wall -L.
FLAG	   =
OPT	   =
THINGS =  imagebld.o sha1.o


all: clean image

%.o	: %.c
	${CXX} ${FLAGS} -o $@ -c $<

image: ${THINGS} 
	gcc -o $@ ${THINGS} 
	
clean:
	-rm -f *.o  image core
