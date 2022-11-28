include ../config.mk

APP  = lxterminal

OBJS = lxterminal.o preferences.o setting.o unixsocket.o 

ifndef VTE_CFLAGS
$(error VTE_CFLAGS is not defined)
endif

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(VTE_CFLAGS) $(WGTK_CFLAGS)
LIBS   += $(VTE_LIBS) -lX11 $(WGTK_LIBS)

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
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/lxterminal.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps
	$(INSTALL_DATA) data/lxterminal.png $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/lxterminal.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(datadir)/lxterminal
	$(INSTALL_DATA) data/lxterminal.conf $(DESTDIR)$(datadir)/lxterminal

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/lxterminal.desktop
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps/lxterminal.png
	rm -f $(DESTDIR)$(mandir)/man1/lxterminal.1
	rm -rf $(DESTDIR)$(datadir)/lxterminal
