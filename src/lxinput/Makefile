include ../config.mk

APP  = lxinput

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS = lxinput.o

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS) $(WGTK_CFLAGS)
LIBS   += -lX11 $(GTK_LIBS) $(WGTK_LIBS)

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
	$(INSTALL_DATA) data/lxinput.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/lxinput
	$(INSTALL_DATA) data/*.ui $(DESTDIR)$(datadir)/lxinput

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/lxinput.desktop
	rm -rf $(DESTDIR)$(datadir)/lxinput

