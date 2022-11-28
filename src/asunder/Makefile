
include ../config.mk

APP  = asunder

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = callbacks.o completion.o interface.o main.o prefs.o support.o threads.o util.o wrappers.o 

LDFLAGS +=
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS   += $(GTK_LIBS) $(WGTK_LIBS) -lcddb -lgthread-2.0
# -lgthread-2.0 only makes sense for glib < 2.32 and gtk2
#  all the right flags and libs are already there, so there's no need
#  to use pkg-config to check for gthread-2.0 libs and flags

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
	install -c -m 644 data/asunder.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/asunder.png $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/asunder.svg $(DESTDIR)$(datadir)/pixmaps
#	@cp -v asunder.conf /etc/rsyslog.d/ || echo "It appears that your system is not using syslog-ng. That's ok but Asunder will be logging to the default syslog target instead of /var/log/asunder.log"

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/asunder.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/asunder.png
	rm -f $(DESTDIR)$(datadir)/pixmaps/asunder.svg
#	rm -f /etc/rsyslog.d/asunder.conf
