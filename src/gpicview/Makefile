include ../config.mk

APP  = gpicview

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = exif.o file-dlgs.o gpicview.o image-list.o image-view.o jhead.o jpgfile.o main-win.o pref.o ptk-menu.o working-area.o

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(LIBJPEG_CFLAGS) $(WGTK_CFLAGS)
LIBS   += $(GTK_LIBS) $(LIBJPEG_LIBS) -lX11 -lm $(WGTK_LIBS)

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
	$(INSTALL_DATA) data/gpicview.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	$(INSTALL_DATA) data/gpicview.png $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	mkdir -p $(DESTDIR)$(datadir)/gpicview/pixmaps
	$(INSTALL_DATA) data/pixmaps/object-rotate-right.png $(DESTDIR)$(datadir)/gpicview/pixmaps
	$(INSTALL_DATA) data/pixmaps/object-rotate-left.png $(DESTDIR)$(datadir)/gpicview/pixmaps
	$(INSTALL_DATA) data/pixmaps/object-flip-horizontal.png $(DESTDIR)$(datadir)/gpicview/pixmaps
	$(INSTALL_DATA) data/pixmaps/object-flip-vertical.png $(DESTDIR)$(datadir)/gpicview/pixmaps
	mkdir -p $(DESTDIR)$(datadir)/gpicview

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/gpicview.desktop
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	rm -rf $(DESTDIR)$(datadir)/gpicview
