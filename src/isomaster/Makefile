include ../config.mk

APP  = isomaster

OBJS = about.o boot.o browser.o editfile.o error.o fsbrowser.o isobrowser.o isomaster.o settings.o window.o

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I.. $(GTK_CFLAGS) $(WGTK_CFLAGS)
# -DDEBUG and -g only used during development // -pedantic -Wno-deprecated-declarations
CFLAGS += -D_FILE_OFFSET_BITS=64 -std=gnu99 -Wundef -Wcast-align -W -Wpointer-arith -Wwrite-strings -Wno-unused-parameter 
# This enables overwriting the original iso,
# don't uncomment it unless you are willing to risk losing data.
#CFLAGS += -DENABLE_SAVE_OVERWRITE
#VERSION = 1.3.16
LIBS   += $(GTK_LIBS) $(WGTK_LIBS)

all: $(APP)

$(APP): bk/bk.a iniparser-4.1/libiniparser.a $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS) bk/bk.a iniparser-4.1/libiniparser.a

bk/bk.a:
	$(MAKE) -C bk
iniparser-4.1/libiniparser.a:
	$(MAKE) -C iniparser-4.1

strip: all
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe
	$(MAKE) -C bk clean
	$(MAKE) -C iniparser-4.1 clean

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/isomaster.desktop $(DESTDIR)$(datadir)/applications
	#mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/64x64/apps
	#$(INSTALL_DATA) icons/isomaster.png $(DESTDIR)$(datadir)/icons/hicolor/64x64/apps
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/isomaster.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(datadir)/isomaster/icons
	$(INSTALL_DATA) icons/*.png $(DESTDIR)$(datadir)/isomaster/icons

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/isomaster.desktop
	rm -f $(DESTDIR)$(mandir)/man1/isomaster.1
	rm -rf $(DESTDIR)$(datadir)/isomaster
	#rm -f $(DESTDIR)$(datadir)/icons/hicolor/64x64/apps/isomaster.png
