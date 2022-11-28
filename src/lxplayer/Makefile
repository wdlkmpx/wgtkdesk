include ../config.mk

APP  = lxplayer

OBJS = gmtk/gm_log.o \
	gmtk/gm_pref_store.o \
	gmtk/gm_strfuncs.o \
	gmtk/gmtk_media_player.o \
	gmtk/gmtk_media_tracker.o \
	gui.o \
	gui-playlist.o \
	main.o \
	resume_cache.o \
	support.o

LDFLAGS += 
#CPPFLAGS += 
CFLAGS += -I.. -Igmtk -DGIO_ENABLED $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS += -lm $(GTK_LIBS) $(WGTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: all
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o gmtk/*.o *~ $(APP) $(APP).exe

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)
	ln -sfv $(APP) $(DESTDIR)$(bindir)/gnome-mplayer

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)
	@if test -h $(DESTDIR)$(bindir)/gnome-mplayer ; then \
		rm -vf $(DESTDIR)$(bindir)/gnome-mplayer; \
	fi

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/lxplayer.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/lxplayer.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/16x16/apps
	$(INSTALL_DATA) data/player16x16.png $(DESTDIR)$(datadir)/icons/hicolor/16x16/apps/lxplayer.png
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/24x24/apps
	$(INSTALL_DATA) data/player24x24.png $(DESTDIR)$(datadir)/icons/hicolor/24x24/apps/lxplayer.png
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/32x32/apps
	$(INSTALL_DATA) data/player32x32.png $(DESTDIR)$(datadir)/icons/hicolor/32x32/apps/lxplayer.png
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	$(INSTALL_DATA) data/player48x48.png $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps/lxplayer.png
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/scalable/apps
	$(INSTALL_DATA) data/player.svg $(DESTDIR)$(datadir)/icons/hicolor/scalable/apps/lxplayer.svg
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) data/player48x48.png $(DESTDIR)$(datadir)/pixmaps/lxplayer.png

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/lxplayer.desktop
	rm -f $(DESTDIR)$(mandir)/man1/lxplayer.1
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/16x16/apps/lxplayer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/24x24/apps/lxplayer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/32x32/apps/lxplayer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps/lxplayer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/scalable/apps/lxplayer.svg
	rm -f $(DESTDIR)$(datadir)/pixmaps/lxplayer.png

