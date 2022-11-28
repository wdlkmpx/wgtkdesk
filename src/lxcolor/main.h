#ifndef __MAIN_H__
#define __MAIN_H__

#include "common.h"


/* interface.c */
GtkWidget* create_lxcolor (void);
GtkWidget* create_save_dialog (const char * labeltext);
GtkWidget* create_popup_menu (void);


/* callbacks.c */
#define USER_COLOR         "user"
#define SYSTEM_COLOR       "system"
#define FILE_ERROR         "An error occurred trying to open file \"%s\" for %s access!\n\n" \
                           "Please check the file permissions and try again."
#define FILE_READ          "read"
#define FILE_WRITE         "write"

/* possible locations of the rgb file */
#define SYSTEM_FILE_1      "/usr/X11R6/lib/X11/rgb.txt"
#define SYSTEM_FILE_2      "/usr/lib/X11/rgb.txt"
#define SYSTEM_FILE_3      "/etc/X11/rgb.txt"
#define SYSTEM_FILE_4      "/usr/openwin/lib/X11/rgb.txt"
#define SYSTEM_FILE_5      "/usr/share/X11/rgb.txt"

extern GtkWidget *lxcolor;
extern GtkWidget *menu;
extern GdkColor   colorvalue;

enum
{
    COLOR,
    COLOR_VALUE,
    COLOR_NAME,
    COLOR_TYPE,
    N_COLUMNS
};

gchar* get_user_file();
void show_file_error (gchar* message);
void on_about_button_clicked (GtkButton *button, gpointer user_data);
void on_colorselection_color_changed (GtkColorSelection *colorselection, gpointer user_data);
void on_save_entry_changed (GtkEditable *editable, gpointer user_data);
void on_list_selection_changed (GtkTreeSelection *selection, gpointer user_data);
void on_copy_color_to_clipboard_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_show_system_colors_activate (GtkMenuItem *menuitem, gpointer user_data);
void show_popup_menu (GtkWidget *treeview, GdkEventButton *event, gpointer user_data);
gboolean on_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_treeview_popup_menu (GtkWidget *widget, gpointer user_data);
void on_save_button_clicked (GtkButton *button, gpointer user_data);
void add_color_to_treeview ();
gboolean save_selected_color ();
void on_delete_button_clicked (GtkButton *button, gpointer user_data);
gboolean delete_color (gchar* color_name, gchar* color_value);
void on_lxcolor_destroy (GtkWidget *object, gpointer user_data);
void on_quit_button_clicked (GtkButton *button, gpointer user_data);
gchar* hex_value (GdkColor color);



// main.c
void add_list_color (gchar *spec, gchar *name, gchar *type, gboolean is_new_color);
void add_rgb_file (gchar *filename, gchar *type);
gchar* get_system_file (void);


/* support.c */
GtkWidget*  lookup_widget (GtkWidget *widget, const gchar     *widget_name);
/* Use this function to set the directory containing installed pixmaps. */
void        add_pixmap_directory       (const gchar     *directory);

/* This is used to create the pixmaps used in the interface. */
GtkWidget*  create_pixmap              (GtkWidget       *widget,
                                        const gchar     *filename);
/* This is used to create the pixbufs used in the interface. */
GdkPixbuf*  create_pixbuf              (const gchar     *filename);

#endif /* __MAIN_H__ */
