include ../../config.mk

LIB_STATIC = bk.a

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS = bkAdd.o bkCache.o bkDelete.o bkError.o bkExtract.o bkGet.o bkIoWrappers.o bkLink.o bkMangle.o bkMisc.o bkPath.o bkRead7x.o bkRead.o bkSet.o bkSort.o bkTime.o bkWrite7x.o bkWrite.o example.o printsizes.o 

CFLAGS  += -D_FILE_OFFSET_BITS=64 -pedantic -std=gnu99 -Wundef -Wcast-align -W -Wpointer-arith -Wwrite-strings -Wno-unused-parameter

# the _FILE_OFFSET_BITS=64 is to enable stat() for large files
#CPPFLAGS = -D_FILE_OFFSET_BITS=64

# -DDEBUG and -g only used during development
CFLAGS += -I../.. -Wall -pedantic -std=gnu99 -Wundef -Wcast-align -W -Wpointer-arith -Wwrite-strings -Wno-unused-parameter
LIBS   += 

all: $(LIB_STATIC) example

$(LIB_STATIC): $(OBJS)
	$(AR) rcs $(LIB_STATIC) $(OBJS)

example: example.c
	$(CC) $(CFLAGS) $(CPPFLAGS) example.c bk.a -o example

strip:

distclean: clean

clean:
	rm -f *.o *~ *.a *.so example

install:

install-strip: install

uninstall:

