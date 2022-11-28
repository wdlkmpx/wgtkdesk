include ../config.mk

APP  = Xdialog

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

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:

uninstall-data:
