include ../config.mk

APP  = gxmessage

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = gxmessage.o

LDFLAGS += 
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
	mkdir -p $(DESTDIR)$(mandir)/man1
	install -c -m 644 data/gxmessage.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	install -c -m 644 data/gxmessage.png $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps

uninstall-data:
	rm -f $(DESTDIR)$(mandir)/man1/gxmessage.1
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps/gxmessage.png
