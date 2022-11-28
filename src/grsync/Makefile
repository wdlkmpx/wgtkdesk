include ../config.mk

APP  = grsync

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = callbacks.o main.o

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += -rdynamic $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS   += -lm $(GTK_LIBS) $(WGTK_LIBS)

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
	install -c grsync-batch $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)
	rm -f $(DESTDIR)$(bindir)/grsync-batch

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	install -c -m 644 data/grsync.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(mandir)/man1
	install -c -m 644 data/grsync.1 $(DESTDIR)$(mandir)/man1
	install -c -m 644 data/grsync-batch.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(datadir)/grsync
	install -c -m 644 grsync.ui $(DESTDIR)$(datadir)/grsync
	mkdir -p $(DESTDIR)$(datadir)/mime/packages
	install -c -m 644 data/grsync.xml $(DESTDIR)$(datadir)/mime/packages
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 pixmaps/application-x-grsync-session.png  $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 pixmaps/grsync.png $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 pixmaps/grsync-busy.png $(DESTDIR)$(datadir)/pixmaps

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/grsync.desktop
	rm -f $(DESTDIR)$(mandir)/man1/grsync.1
	rm -f $(DESTDIR)$(mandir)/man1/grsync-batch.1
	rm -rf $(DESTDIR)$(datadir)/grsync
	rm -f $(DESTDIR)$(datadir)/mime/packages/grsync.xml
	rm -f $(DESTDIR)$(datadir)/pixmaps/application-x-grsync-session.png
	rm -f $(DESTDIR)$(datadir)/pixmaps/grsync.png
	rm -f $(DESTDIR)$(datadir)/pixmaps/grsync-busy.png
