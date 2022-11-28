include ../config.mk

APP  = yad

# html.o not supported...
# print.o needs
# - gtk+-unix-print-2.0.pc
# - gtk+-unix-print-3.0.pc
#--
# pfd.o (pfd is a separate app)
# browser.o (browser is a separate app)
OBJS = about.o calendar.o color.o dnd.o entry.o file.o font.o form.o icons.o list.o main.o multi-progress.o notebook.o notification.o option.o paned.o picture.o progress.o scale.o text.o util.o

# configure.ac: pkg-config
#    gtk3) 
#        GTK_MODULE="gtk+-3.0"
#        GTK_REQUIRED="3.0.0"
#        GTK_PRINT="gtk+-unix-print-3.0"
#        WEBKIT="webkit2gtk-4.0"
#        SPELL="gtkspell3-3.0"
#        SOURCEVIEW="gtksourceview-3.0"
#    gtk2)
#        GTK_MODULE="gtk+-2.0"
#        GTK_REQUIRED="2.16.0"
#        GTK_PRINT="gtk+-unix-print-2.0"
#        WEBKIT="webkit-1.0"
#        SPELL="gtkspell3-2.0"
#        SOURCEVIEW="gtksourceview-2.0"


LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(WGTK_CFLAGS) -DHAVE_GIO
LIBS   += $(GTK_LIBS) $(WGTK_LIBS)

# Makefile.am
#yad_CFLAGS = $(GTK_CFLAGS) $(HTML_CFLAGS) $(SPELL_CFLAGS) $(SOURCEVIEW_CFLAGS)
#yad_LDADD = $(GTK_LIBS) $(HTML_LIBS) $(SPELL_LIBS) $(SOURCEVIEW_LIBS)

all: $(APP) yad-icon-browser pfd

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

yad-icon-browser: browser.o
	$(CC) $(CFLAGS) browser.o -o yad-icon-browser $(LDFLAGS) $(LIBS)

pfd: pfd.o
	$(CC) $(CFLAGS) pfd.o -o pfd $(LDFLAGS) $(LIBS)

strip: all
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe pfd yad-icon-browser

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/yad-icon-browser.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) data/yad.png $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	$(INSTALL_DATA) data/yad.png $(DESTDIR)$(datadir)/pixmaps
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/yad.1 $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/pfd.1 $(DESTDIR)$(mandir)/man1

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/yad-icon-browser.desktop
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps/yad.png
	rm -f $(DESTDIR)$(datadir)/pixmaps/apps/yad.png
	rm -f $(DESTDIR)$(mandir)/man1/yad.1
	rm -f $(DESTDIR)$(mandir)/man1/pfd.1
