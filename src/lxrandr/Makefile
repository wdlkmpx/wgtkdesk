include ../config.mk

APP  = lxrandr

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = lxrandr.o

#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS   += $(GTK_LIBS) $(WGTK_LIBS)

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
	$(INSTALL_DATA) data/lxrandr.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/lxrandr.1 $(DESTDIR)$(mandir)/man1

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/lxrandr.desktop
	rm -f $(DESTDIR)$(mandir)/man1/lxrandr.1
