
gboolean delete_event(gpointer object, GdkEventAny *event, gpointer data);

gboolean destroy_event(gpointer object, GdkEventAny *event, gpointer data);

gint double_click_event(GtkWidget *object, GdkEventButton *event, gpointer data);

gboolean exit_ok(gpointer object, gpointer data);

gboolean exit_extra(gpointer object, gpointer data);

gboolean exit_cancel(gpointer object, gpointer data);

gboolean exit_keypress(gpointer object, GdkEventKey *event, gpointer data);

gboolean exit_help(gpointer object, gpointer data);

gboolean exit_previous(gpointer object, gpointer data);

gboolean checked(GtkWidget *button, gpointer data);

gboolean timeout_exit(gpointer data);

gboolean infobox_timeout_exit(gpointer data);
gboolean infobox_timeout(gpointer data);

gboolean gauge_timeout(gpointer data);

gboolean progress_timeout(gpointer data);

gboolean tailbox_timeout(gpointer data);
gboolean tailbox_keypress(GtkWidget *text, GdkEventKey *event, gpointer data);

gboolean logbox_timeout(gpointer data);

gboolean inputbox_ok(gpointer object, gpointer data);

gboolean input_keypress(GtkWidget *entry, GdkEventKey *event, gpointer data);

gboolean inputbox_timeout(gpointer data);

gboolean hide_passwords(GtkWidget *button, gpointer data);

gboolean editbox_ok(gpointer object, gpointer data);

gboolean print_text(gpointer object, gpointer data);

gboolean rangebox_exit(GtkButton *button, gpointer data);

gboolean rangebox_timeout(gpointer data);

gboolean spinbox_exit(GtkButton *button, gpointer data);

gboolean spinbox_timeout(gpointer data);

void item_toggle(GtkWidget *item, int i);

gboolean print_items(GtkButton *button, gpointer data);

gboolean itemlist_timeout(gpointer data);

void on_menubox_ok_click (GtkButton *button, gpointer data);
#if GTK_CHECK_VERSION(2,0,0)
void on_menubox_treeview_row_activated_cb (GtkTreeView *tree_view,    GtkTreePath *path,
                                           GtkTreeViewColumn *column, gpointer data);
void on_menubox_tip_treeview_changed (GtkTreeSelection *selection, gpointer data);
#else // -- GTK1 --
void on_menubox_item_select (GtkWidget *clist, gint row, gint column,
                             GdkEventButton *event, gpointer data);
#endif

void print_tree_selection (GtkButton *button, gpointer data);
void tree_selection_changed (GtkWidget *tree);

void buildlist_sensitive_buttons(void);
void buildlist_add_or_remove (GtkButton *button, gpointer data);
void buildlist_print_list (GtkButton *button, gpointer data);

gboolean filesel_exit(GtkWidget *filesel, gpointer client_data);

gboolean colorsel_exit(GtkWidget *colorsel, gpointer client_data);
gboolean fontsel_exit(GtkWidget *filesel, gpointer client_data);

gboolean calendar_exit(gpointer object, gpointer data);
gboolean calendar_timeout(gpointer data);

gboolean timebox_exit(gpointer object, gpointer data);
gboolean timebox_timeout(gpointer data);
