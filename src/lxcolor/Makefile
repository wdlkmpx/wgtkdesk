include ../config.mk

APP  = lxcolor

# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = callbacks.o interface.o main.o support.o 

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

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/gcolor2
	ln -s $(APP) $(DESTDIR)$(bindir)/gcolor2

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)
	@if test -h $(DESTDIR)$(bindir)/gcolor2; then rm -fv $(DESTDIR)$(bindir)/gcolor2 ; fi

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	install -c -m 644 data/lxcolor.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/lxcolor.png $(DESTDIR)$(datadir)/pixmaps

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/lxcolor.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/lxcolor.png
