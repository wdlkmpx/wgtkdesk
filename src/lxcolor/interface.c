
#include "main.h"

#undef PACKAGE
#define PACKAGE "lxcolor"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define widget_set_data(widget,child,name) \
        g_object_set_data (G_OBJECT(widget),name,child)

GtkWidget* create_lxcolor (void)
{
    GtkWidget *lxcolor;
    GdkPixbuf *lxcolor_icon_pixbuf;
    GtkWidget *vbox1;
    GtkWidget *colorselection;
    GtkWidget *expander;
    GtkWidget *hbox1;
    GtkWidget *empty_space;
    GtkWidget *vbox2;
    GtkWidget *hbuttonbox2;
    GtkWidget *delete_button;
    GtkWidget *save_button;
    GtkWidget *scrolledwindow1;
    GtkWidget *treeview;
    GtkWidget *expander_label;
    GtkWidget *hseparator1;
    GtkWidget *hbuttonbox3;
    GtkWidget *about_button;
    GtkWidget *hbox2;
    GtkWidget *quit_button;

    lxcolor = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (lxcolor), 1);
    gtk_window_set_title (GTK_WINDOW (lxcolor), "LXcolor");
    gtk_window_set_resizable (GTK_WINDOW (lxcolor), FALSE);
    lxcolor_icon_pixbuf = create_pixbuf (PACKAGE ".png");
    if (lxcolor_icon_pixbuf)
    {
        gtk_window_set_icon (GTK_WINDOW (lxcolor), lxcolor_icon_pixbuf);
        g_object_unref (lxcolor_icon_pixbuf);
    }

    vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (lxcolor), vbox1);
    gtk_container_set_border_width (GTK_CONTAINER (vbox1), 10);

    colorselection = gtk_color_selection_new ();
    gtk_widget_show (colorselection);
    gtk_box_pack_start (GTK_BOX (vbox1), colorselection, FALSE, TRUE, 0);
    gtk_color_selection_set_has_opacity_control (GTK_COLOR_SELECTION (colorselection), TRUE);

    expander = gtk_expander_new (NULL);
    gtk_widget_show (expander);
    gtk_box_pack_start (GTK_BOX (vbox1), expander, FALSE, FALSE, 0);

    hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show (hbox1);
    gtk_container_add (GTK_CONTAINER (expander), hbox1);

    empty_space = gtk_label_new ("");
    gtk_widget_show (empty_space);
    gtk_box_pack_start (GTK_BOX (hbox1), empty_space, FALSE, FALSE, 5);

    vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_show (vbox2);
    gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);
    gtk_widget_set_size_request (vbox2, -1, 220);
    gtk_container_set_border_width (GTK_CONTAINER (vbox2), 5);

    hbuttonbox2 = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_show (hbuttonbox2);
    gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox2, FALSE, FALSE, 0);

#if GTK_MAJOR_VERSION < 3
    delete_button = gtk_button_new_from_stock ("gtk-delete");
#else
    delete_button = gtk_button_new_with_mnemonic (_("_Delete"));
#endif
    gtk_widget_show (delete_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), delete_button);
    gtk_widget_set_sensitive (delete_button, FALSE);
    gtk_widget_set_can_default (delete_button, TRUE);
    gtk_widget_set_tooltip_text (delete_button, _("Delete the selected color. You can only delete those you saved yourself."));

#if GTK_MAJOR_VERSION < 3
    save_button = gtk_button_new_from_stock ("gtk-save");
#else
    save_button = gtk_button_new_with_mnemonic (_("_Save"));
#endif
    gtk_widget_show (save_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), save_button);
    gtk_widget_set_can_default (save_button, TRUE);
    gtk_widget_set_tooltip_text (save_button, _("Save the color selected in the colorwheel"));

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow1);
    gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow1, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow1), 1);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_OUT);

    treeview = gtk_tree_view_new ();
    gtk_widget_show (treeview);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview);
#if GTK_MAJOR_VERSION < 3
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
#endif

    expander_label = gtk_label_new_with_mnemonic (_("_Show saved colors"));
    gtk_widget_show (expander_label);
    gtk_expander_set_label_widget (GTK_EXPANDER (expander), expander_label);

    hseparator1 = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_show (hseparator1);
    gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, FALSE, 0);
    gtk_widget_set_size_request (hseparator1, -1, 10);

    hbuttonbox3 = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_show (hbuttonbox3);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox3, FALSE, TRUE, 0);

#if GTK_MAJOR_VERSION < 3
    about_button = gtk_button_new_from_stock ("gtk-about");
#else
    about_button = gtk_button_new_with_mnemonic (_("_About"));
#endif
    gtk_widget_show (about_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox3), about_button);

    hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_show (hbox2);

#if GTK_MAJOR_VERSION < 3
    quit_button = gtk_button_new_from_stock ("gtk-quit");
#else
    quit_button = gtk_button_new_with_mnemonic (_("_Quit"));
#endif
    gtk_widget_show (quit_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox3), quit_button);
    gtk_widget_set_can_default (quit_button, TRUE);
    gtk_widget_set_tooltip_text (quit_button, _("Exit the program"));

    g_signal_connect ((gpointer) lxcolor, "destroy",
                      G_CALLBACK (on_lxcolor_destroy),
                      NULL);
    g_signal_connect ((gpointer) colorselection, "color_changed",
                      G_CALLBACK (on_colorselection_color_changed),
                      NULL);
    g_signal_connect ((gpointer) delete_button, "clicked",
                      G_CALLBACK (on_delete_button_clicked),
                      NULL);
    g_signal_connect ((gpointer) save_button, "clicked",
                      G_CALLBACK (on_save_button_clicked),
                      NULL);
    g_signal_connect ((gpointer) treeview, "button_press_event",
                      G_CALLBACK (on_treeview_button_press_event),
                      NULL);
    g_signal_connect ((gpointer) treeview, "popup_menu",
                      G_CALLBACK (on_treeview_popup_menu),
                      NULL);
    g_signal_connect ((gpointer) about_button, "clicked",
                      G_CALLBACK (on_about_button_clicked),
                      NULL);
    g_signal_connect ((gpointer) quit_button, "clicked",
                      G_CALLBACK (on_quit_button_clicked),
                      NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    widget_set_data (lxcolor, lxcolor, "gcolor");
    widget_set_data (lxcolor, vbox1, "vbox1");
    widget_set_data (lxcolor, colorselection, "colorselection");
    widget_set_data (lxcolor, expander, "expander");
    widget_set_data (lxcolor, hbox1, "hbox1");
    widget_set_data (lxcolor, empty_space, "empty_space");
    widget_set_data (lxcolor, vbox2, "vbox2");
    widget_set_data (lxcolor, hbuttonbox2, "hbuttonbox2");
    widget_set_data (lxcolor, delete_button, "delete_button");
    widget_set_data (lxcolor, save_button, "save_button");
    widget_set_data (lxcolor, scrolledwindow1, "scrolledwindow1");
    widget_set_data (lxcolor, treeview, "treeview");
    widget_set_data (lxcolor, expander_label, "expander_label");
    widget_set_data (lxcolor, hseparator1, "hseparator1");
    widget_set_data (lxcolor, hbuttonbox3, "hbuttonbox3");
    widget_set_data (lxcolor, about_button, "about_button");
    widget_set_data (lxcolor, hbox2, "hbox2");
    widget_set_data (lxcolor, quit_button, "quit_button");

    return lxcolor;
}


GtkWidget* create_save_dialog (const char * labeltext)
{
    GtkWidget *save_dialog;
    GdkPixbuf *save_dialog_icon_pixbuf;
    GtkWidget *dialog_vbox1;
    GtkWidget *save_label;
    GtkWidget *save_entry;
    GtkWidget *save_ok;

    save_dialog = gtk_dialog_new ();
    gtk_container_set_border_width (GTK_CONTAINER (save_dialog), 10);
    gtk_window_set_title (GTK_WINDOW (save_dialog), _("Save a Color"));
    gtk_window_set_position (GTK_WINDOW (save_dialog), GTK_WIN_POS_CENTER_ON_PARENT);
    gtk_window_set_modal (GTK_WINDOW (save_dialog), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (save_dialog), FALSE);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (save_dialog), TRUE);
    save_dialog_icon_pixbuf = create_pixbuf (PACKAGE ".png");
    if (save_dialog_icon_pixbuf)
    {
        gtk_window_set_icon (GTK_WINDOW (save_dialog), save_dialog_icon_pixbuf);
        g_object_unref (save_dialog_icon_pixbuf);
    }
    gtk_window_set_type_hint (GTK_WINDOW (save_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);

    dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (save_dialog));
    gtk_widget_show (dialog_vbox1);

    save_label = gtk_label_new ("");
    gtk_label_set_markup (GTK_LABEL (save_label), labeltext);
    gtk_widget_show (save_label);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), save_label, FALSE, FALSE, 3);
    gtk_label_set_justify (GTK_LABEL (save_label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap (GTK_LABEL (save_label), TRUE);

    save_entry = gtk_entry_new ();
    gtk_widget_show (save_entry);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), save_entry, FALSE, FALSE, 5);
    gtk_widget_set_tooltip_text (save_entry, _("Enter a name for the color you selected"));
    gtk_entry_set_activates_default (GTK_ENTRY (save_entry), TRUE);

    gtk_dialog_add_button (GTK_DIALOG (save_dialog), "gtk-cancel", GTK_RESPONSE_CANCEL);
    save_ok     = gtk_dialog_add_button (GTK_DIALOG (save_dialog), "gtk-save", GTK_RESPONSE_OK);
    gtk_widget_set_sensitive (save_ok, FALSE);
    gtk_widget_set_can_default (save_ok, TRUE);
    
    g_signal_connect ((gpointer) save_entry, "changed",
                      G_CALLBACK (on_save_entry_changed), save_ok);

    gtk_widget_grab_focus (save_entry);
    gtk_widget_grab_default (save_ok);
    return save_dialog;
}


GtkWidget* create_popup_menu (void)
{
    GtkWidget *popup_menu;
    GtkWidget *copy_color_to_clipboard;
    GtkWidget *image5;
    GtkWidget *separator1;
    GtkWidget *show_system_colors;

    popup_menu = gtk_menu_new ();

    copy_color_to_clipboard = gtk_image_menu_item_new_with_mnemonic (_("Copy color to clipboard"));
    gtk_widget_show (copy_color_to_clipboard);
    gtk_container_add (GTK_CONTAINER (popup_menu), copy_color_to_clipboard);

    image5 = gtk_image_new_from_icon_name ("gtk-copy", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image5);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (copy_color_to_clipboard), image5);

    separator1 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator1);
    gtk_container_add (GTK_CONTAINER (popup_menu), separator1);
    gtk_widget_set_sensitive (separator1, FALSE);

    show_system_colors = gtk_check_menu_item_new_with_mnemonic (_("Show system colors"));
    gtk_widget_show (show_system_colors);
    gtk_container_add (GTK_CONTAINER (popup_menu), show_system_colors);
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (show_system_colors), TRUE);

    g_signal_connect ((gpointer) copy_color_to_clipboard, "activate",
                      G_CALLBACK (on_copy_color_to_clipboard_activate),
                      NULL);
    g_signal_connect ((gpointer) show_system_colors, "activate",
                      G_CALLBACK (on_show_system_colors_activate),
                      NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    widget_set_data (popup_menu, popup_menu, "popup_menu");
    widget_set_data (popup_menu, copy_color_to_clipboard, "copy_color_to_clipboard");
    widget_set_data (popup_menu, image5, "image5");
    widget_set_data (popup_menu, separator1, "separator1");
    widget_set_data (popup_menu, show_system_colors, "show_system_colors");

    return popup_menu;
}
