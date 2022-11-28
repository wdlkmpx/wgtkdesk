include ../config.mk

APP  = lxtask

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS = callbacks.o functions.o interface.o main.o utils.o xfce-taskmanager-linux.o 

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS   += $(GTK_LIBS) $(WGTK_LIBS) -lwgtk

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/lxtask.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/lxtask.1 $(DESTDIR)$(mandir)/man1

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/lxtask.desktop
	rm -f $(DESTDIR)$(mandir)/man1/lxtask.1

