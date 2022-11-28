#define MAXPATH 2048
#define MAXOPT 100
#define CONFIG_GROUP "__CONFIG"

#define ICON_SOURCE "pixmaps/grsync.png"
#define ICON_SOURCE_BUSY "pixmaps/grsync-busy.png"
#define ICON_PACKAGE PACKAGE_DATADIR "/" ICON_SOURCE
#define ICON_PACKAGE_BUSY PACKAGE_DATADIR "/" ICON_SOURCE_BUSY

extern GtkBuilder *builder;
extern GtkWidget *main_window;
extern char *argv_session;
extern char *argv_filename;
extern char *icon;
extern char *icon_busy;
extern gboolean cmdline_session;
extern gboolean cmdline_execute;
extern gboolean cmdline_stayopen;
extern gboolean cmdline_import;
extern GtkListStore *liststore_session;

void save_settings(gchar *session, gchar *filename);
gboolean load_groups(gchar *session);
void on_main_create(GtkWindow *window, gpointer user_data);
void on_expander_rsync_activate(GtkExpander *expander, gpointer user_data);
void on_button_show_errors_clicked(GtkButton *button, gpointer user_data);
void on_import1_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_rsync_show(GtkWidget *widget, gpointer user_data);
