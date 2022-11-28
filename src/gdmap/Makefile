include ../config.mk

APP  = gdmap

OBJS = colors.o gui_main.o gui_support.o main.o preferences.o tree.o utils.o

LIBXML2_CFLAGS = $(shell pkg-config libxml-2.0 --cflags)
LIBXML2_LIBS = -lxml2

LDFLAGS +=
#CPPFLAGS += -I..
CFLAGS += -I.. $(GTK_CFLAGS) $(WGTK_CFLAGS) $(LIBXML2_CFLAGS)
LIBS   += -lm  $(GTK_LIBS)   $(WGTK_LIBS)   $(LIBXML2_LIBS)

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
	$(INSTALL_DATA) data/gdmap.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) data/gdmap.png $(DESTDIR)$(datadir)/pixmaps
	mkdir -p $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) data/gdmap.1 $(DESTDIR)$(mandir)/man1

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/gdmap.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gdmap.png
	rm -f $(DESTDIR)$(mandir)/man1/gdmap.1
