include ../config.mk

APP  = gmeasures

OBJS = callbacks.o interface.o jpslib2.o main.o support.o

LDFLAGS +=
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS   += $(GTK_LIBS) -lX11 $(WGTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: all
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
#	mkdir -p $(DESTDIR)$(datadir)/applications
#	$(INSTALL_DATA) data/gmeasures.desktop $(DESTDIR)$(datadir)/applications
#	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps
#	$(INSTALL_DATA) data/gmeasures.png $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps
#	mkdir -p $(DESTDIR)$(mandir)/man1
#	$(INSTALL_DATA) data/gmeasures.1 $(DESTDIR)$(mandir)/man1
#	mkdir -p $(DESTDIR)$(datadir)/gmeasures
#	$(INSTALL_DATA) data/gmeasures.oonf $(DESTDIR)$(datadir)/gmeasures

uninstall-data:
#	rm -f $(DESTDIR)$(datadir)/applications/gmeasures.desktop
#	rm -f $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps/gmeasures.png
#	rm -f $(DESTDIR)$(mandir)/man1/gmeasures.1
#	rm -rf $(DESTDIR)$(datadir)/gmeasures

