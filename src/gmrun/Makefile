
include ../config.mk

APP  = gmrun

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = config_prefs.o gtkcompletionline.o history.o main.o

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

clean:
	rm -f *.o *~ $(APP) $(APP).exe

distclean: clean

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	install -c -m 644 data/gmrun.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/gmrun.png $(DESTDIR)$(datadir)/pixmaps
	mkdir -p $(DESTDIR)$(mandir)/man1
	install -c -m 644 data/gmrun.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(sysconfdir)
	install -c -m 644 data/gmrunrc $(DESTDIR)$(sysconfdir)

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/gmrun.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gmrun.png
	rm -f $(DESTDIR)$(mandir)/man1/gmrun.1
	rm -f $(DESTDIR)$(sysconfdir)/gmrunrc

