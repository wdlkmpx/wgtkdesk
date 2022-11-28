
#include "main.h"
#include <glib/gprintf.h>

GtkWidget        *savedialog;
gchar            *colorname;
GtkTreeIter       selection_iter;


/**
 * displays a simple file error dialog, where type specifies the
 * access that failed (read, write).
 */
void show_file_error (gchar* type)
{
    GtkDialog *d;

    d = GTK_DIALOG (
        gtk_message_dialog_new (GTK_WINDOW (lxcolor), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK, _(FILE_ERROR), get_user_file (), type)
    );
    gtk_dialog_run (d);
    gtk_widget_destroy (GTK_WIDGET (d));
}


void on_about_button_clicked (GtkButton *button, gpointer user_data)
{
    GtkWidget *w;
    const gchar * authors[] =
    {
        "Justin Spadea <justin@j-z-s.com>",
        NULL
    };
    /* TRANSLATORS: Replace this string with your names, one name per line. */
    gchar * translators = _("Translated by");

    GdkPixbuf * logo = create_pixbuf (PACKAGE ".png");

    /* Create and initialize the dialog. */
    w = g_object_new (GTK_TYPE_ABOUT_DIALOG,
                      "version",      VERSION,
                      "program-name", PACKAGE,
                      "copyright",    "Copyright (C) 2004-2022",
                      "comments",     "",
                      "license",      "GPL2 - see COPYING file.",
                      "website",      "",
                      "authors",      authors,
                      "translator-credits", translators,
                      "logo",         logo,
                      NULL);
    gtk_container_set_border_width (GTK_CONTAINER (w), 2);
    gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (lxcolor));
    gtk_window_set_modal (GTK_WINDOW (w), TRUE);
    gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_CENTER_ON_PARENT);

    g_signal_connect_swapped (w, "response",
                              G_CALLBACK (gtk_widget_destroy), w);
    gtk_widget_show_all (GTK_WIDGET (w));
}


void on_colorselection_color_changed (GtkColorSelection *colorselection, gpointer user_data)
{
    gtk_color_selection_get_current_color (colorselection, &colorvalue);
}


void on_save_entry_changed (GtkEditable *editable, gpointer user_data)
{
    GtkWidget *button_ok = GTK_WIDGET (user_data);
    gchar     *text;

    text = g_strdup (gtk_entry_get_text (GTK_ENTRY (editable)));
    if (strlen (text) > 0)
    {
        gtk_widget_set_sensitive (button_ok, TRUE);
    }
    else
    {
        gtk_widget_set_sensitive (button_ok, FALSE);
    }
}


void on_list_selection_changed (GtkTreeSelection *selection, gpointer user_data)
{
    GdkColor      new_color, curr_color;
    GtkTreeModel *model;
    gchar        *color;
    gchar        *type;
    GtkWidget    *delete_button = lookup_widget (lxcolor, "delete_button");

    if (gtk_tree_selection_get_selected (selection, &model, &selection_iter))
    {
        GtkColorSelection *colorsel;

        gtk_tree_model_get (model, &selection_iter, COLOR_VALUE, &color, COLOR_TYPE, &type, -1);
        colorsel = GTK_COLOR_SELECTION (lookup_widget (lxcolor, "colorselection"));
        gdk_color_parse (color, &new_color);

        /* save the old color in color wheel */
        gtk_color_selection_get_current_color (colorsel, &curr_color);
        gtk_color_selection_set_previous_color (colorsel, &curr_color);
        gtk_color_selection_set_current_color (colorsel, &new_color);
        g_free (color);

        /* if selection is a user color, enable delete button.
           if system color, disable (never modify the system file!) */
        if (!strcmp (type, USER_COLOR))
        {
            gtk_widget_set_sensitive (delete_button, TRUE);
        }
        else
        {
            gtk_widget_set_sensitive (delete_button, FALSE);
        }
    }
    else
    {
        /* nothing selected - disable delete button */
        gtk_widget_set_sensitive (delete_button, FALSE);
    }
}


void on_copy_color_to_clipboard_activate (GtkMenuItem *menuitem, gpointer user_data)
{
    gchar* hex;

    hex = hex_value (colorvalue);
    GtkClipboard *cb = gtk_clipboard_get (gdk_atom_intern ("PRIMARY", FALSE));
    gtk_clipboard_set_text (cb, hex, strlen (hex));
}


void on_show_system_colors_activate (GtkMenuItem *menuitem, gpointer user_data)
{
    if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (menuitem)))
    {
        /* re-add the system colors */
        add_rgb_file (get_system_file (), SYSTEM_COLOR);
    }
    else
    {
        /* hide the system colors. unfortunately we have to remove them
           since there's no way to simply hide them. */
        gchar *type;
        GtkTreeIter iterator;
        GtkTreeView *treeview = GTK_TREE_VIEW (lookup_widget (lxcolor, "treeview"));
        GtkTreeModel *model = gtk_tree_view_get_model (treeview);

        gtk_tree_model_get_iter_first (model, &iterator);
        while (gtk_list_store_iter_is_valid (GTK_LIST_STORE (model), &iterator))
        {
            gtk_tree_model_get (model, &iterator, COLOR_TYPE, &type, -1);
            if (!strcmp (type, SYSTEM_COLOR))
            {
                gtk_list_store_remove (GTK_LIST_STORE (model), &iterator);
            }
            else
            {
                gtk_tree_model_iter_next (model, &iterator);
            }
        }
    }
}


void show_popup_menu (GtkWidget *treeview, GdkEventButton *event, gpointer user_data)
{
    gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
                    (event != NULL ? event->button : 0),
                    gdk_event_get_time ((GdkEvent*)event));
}


gboolean on_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event,
                                         gpointer user_data)
{
    if (event->type == GDK_BUTTON_PRESS && event->button == 3)
    {
        GtkTreeSelection *selection;
        GtkTreePath *path;

        selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
        if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget),
            event->x, event->y, &path, NULL, NULL, NULL))
        {
            gtk_tree_selection_select_path (selection, path);
            gtk_tree_path_free (path);
        }
        show_popup_menu (widget, event, user_data);
    }
    return FALSE;
}


gboolean on_treeview_popup_menu (GtkWidget *widget, gpointer user_data)
{
    show_popup_menu (widget, NULL, user_data);
    return TRUE;
}


void on_save_button_clicked (GtkButton *button, gpointer user_data)
{
    gchar     *labeltext;
    gint       result;

    labeltext = g_strdup_printf (_("Enter a color name for %s:"), hex_value (colorvalue));
    savedialog = create_save_dialog (labeltext);
    g_free (labeltext);

    gtk_window_set_transient_for (GTK_WINDOW (savedialog), GTK_WINDOW (lxcolor));

    result = gtk_dialog_run (GTK_DIALOG (savedialog));
    switch (result)
    {
        case GTK_RESPONSE_OK:
            /* save to file, then add to UI */
            if (save_selected_color ())
                add_color_to_treeview ();
            break;
        case GTK_RESPONSE_CANCEL:
        default:
            colorname = "";
            break;
    }
    gtk_widget_destroy (savedialog);
}


void add_color_to_treeview ()
{
    GtkTreeView      *treeview;
    GtkTreeSelection *selection;

    /* add color to tree view */
    add_list_color (hex_value (colorvalue), colorname, USER_COLOR, TRUE);

    /* scroll tree view so user sees new color */
    treeview = GTK_TREE_VIEW (lookup_widget (lxcolor, "treeview"));
    selection = gtk_tree_view_get_selection (treeview);
    if (selection)
    {
        GtkTreeIter iter;
        if (gtk_tree_selection_get_selected (selection, NULL, &iter) )
        {
            GtkTreePath *path = gtk_tree_model_get_path (gtk_tree_view_get_model (treeview), &iter);
            gtk_tree_view_scroll_to_cell (treeview, path, NULL, FALSE, 0, 0);
            gtk_tree_path_free (path);
        }
    }
}


gboolean save_selected_color ()
{
    GtkEntry *entry;
    FILE     *fp;
    gchar     old[512] = "";

    /* get entry text */
    entry = GTK_ENTRY (lookup_widget (savedialog, "save_entry"));
    colorname = g_strdup (gtk_entry_get_text (entry));

    if (!strcmp (colorname, ""))
        return FALSE;

    /* save color in user file - write at top of the file */
    /* if file exists already, get its contents, otherwise just write to it */
    if (g_file_test (get_user_file (), G_FILE_TEST_EXISTS))
    {
        fp = fopen (get_user_file (), "r");
        if (fp) {
            fread (old, sizeof (old), 1, fp);
            fclose (fp);
        } else {
            show_file_error (FILE_READ);
            return FALSE;
        }
    }

    fp = fopen (get_user_file (), "w");
    if (!fp)
    {
        show_file_error (FILE_WRITE);
        return FALSE;
    }

    fprintf (fp, "%3d %3d %3d\t\t%s\n%s", colorvalue.red/256, colorvalue.green/256,
             colorvalue.blue/256, colorname, old);
    fclose (fp);
    return TRUE;
}


void on_delete_button_clicked (GtkButton *button, gpointer user_data)
{
    GtkWidget   *deletedialog;
    GtkTreeView *treeview;
    gchar       *color_name;
    gchar       *color_value;
    gint         result;

    treeview = GTK_TREE_VIEW (lookup_widget (lxcolor, "treeview"));
    gtk_tree_model_get (gtk_tree_view_get_model (treeview), &selection_iter,
                        COLOR_NAME, &color_name, COLOR_VALUE, &color_value, -1);

    deletedialog = gtk_message_dialog_new (GTK_WINDOW (lxcolor),
                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_QUESTION,
                        GTK_BUTTONS_YES_NO,
                        _("Are you sure you want to delete \'%s\'?"), color_name);
    gtk_window_set_title (GTK_WINDOW (deletedialog), _("Delete a Color"));

    result = gtk_dialog_run (GTK_DIALOG (deletedialog));
    switch (result)
    {
        case GTK_RESPONSE_YES:
        case GTK_RESPONSE_OK: /* delete from file, then from UI */
            if (delete_color (color_name, color_value))
                gtk_list_store_remove (GTK_LIST_STORE (gtk_tree_view_get_model (treeview)),
                                        &selection_iter);
            break;
    }
    gtk_widget_destroy (deletedialog);
}

/**
 * Deletes the first matching line from the rgb user file
 * that matches the color name and color value specified.
 *
 * color_value is given in hex format
 */
gboolean delete_color (gchar* color_name, gchar* color_value)
{
    FILE     *fp;
    gchar    *p;
    gchar    *file_color_name;
    gchar     file_color_value[8];
    gint      r, g, b;
    gchar     buffer[512] = "";
    gchar     newstuff[512] = "";
    gboolean  found = FALSE;

    /* remove from file */
    fp = fopen (get_user_file (), "r");
    if (!fp)
    {
        show_file_error (FILE_READ);
        return FALSE;
    }

    while ((p = fgets (buffer, sizeof buffer, fp)) != NULL)
    {
        if (buffer[0] == '!')
            continue;
        r = g_ascii_strtoull (p, &p, 10);
        g = g_ascii_strtoull (p, &p, 10);
        b = g_ascii_strtoull (p, &p, 10);
        p += strspn (p, " \t");
        g_sprintf (file_color_value, "#%2X%2X%2X", r, g, b);
        file_color_name = g_strchomp (g_strdup (p));

        /* make sure to only remove the first matching color. both value and
           name must match */
        if (found || strcmp (file_color_name, color_name) != 0 ||
            strcmp (g_ascii_strup (file_color_value, -1), color_value) != 0)
        {
            g_sprintf (newstuff, "%s%3d %3d %3d\t\t%s\n", newstuff, r, g, b, file_color_name);
        }
        else
        {
            found = TRUE;
        }
    }
    fclose (fp);

    /* only rewrite the file if we found a match */
    if (found)
    {
        fp = fopen (get_user_file (), "w");
        if (!fp)
        {
            show_file_error (FILE_WRITE);
            return FALSE;
        }

        fprintf (fp, "%s", newstuff);
        fclose (fp);
        return TRUE;
    }
    return FALSE;
}


void on_lxcolor_destroy (GtkWidget *object, gpointer user_data)
{
    gtk_main_quit ();
}


void on_quit_button_clicked (GtkButton *button, gpointer user_data)
{
    on_lxcolor_destroy (GTK_WIDGET (button), user_data);
}


gchar* hex_value (GdkColor color)
{
    return g_strdup_printf ("#%.2X%.2X%.2X", color.red/256, color.green/256, color.blue/256);
}
