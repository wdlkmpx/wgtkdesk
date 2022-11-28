include ../../config.mk

LIB_STATIC = libiniparser.a

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS = dictionary.o iniparser.o 

#CPPFLAGS += -I..
CFLAGS += -I../.. -fPIC -ansi -pedantic 
LIBS   += 

all: $(LIB_STATIC)

$(LIB_STATIC): $(OBJS)
	$(AR) rcs $(LIB_STATIC) $(OBJS)

strip:

distclean: clean

clean:
	rm -f *.o *~ *.a *.so

install:

install-strip: install

uninstall:

