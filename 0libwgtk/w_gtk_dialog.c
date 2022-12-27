/*
 * Public Domain
 */


// Supported widgets:
// -

// all w_gtk_dialog_..() functions take GtkWidget as parameter

/*
*/

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "w_gtk.h"

#ifdef ENABLE_NLS
#include <locale.h>
#include <libintl.h>
#else
#define gettext(x) (x)
#endif

///#define W_COMBO_DEBUG

#ifdef W_COMBO_DEBUG
# define W_DEBUG_PUTS(text) puts(text) 
#else
# define W_DEBUG_PUTS(text)
#endif


/*--------------------------------------------------------------
/                  w_gtk_fontsel_dialog_new
/-------------------------------------------------------------*/

/*
*/

GtkWidget * w_gtk_combobox_new ()
{
    GtkWidget * combo;

#if GTK_CHECK_VERSION(4,0,0) // 4,10,0 //TODO this has not been tested
    // Some people say GtkComboBox is broken in gtk4
    GtkStringList *list = gtk_string_list_new (NULL);
    combo = gtk_drop_down_new (G_LIST_MODEL(list), NULL);
#endif
#if GTK_CHECK_VERSION(2,24,0)
    combo = gtk_combo_box_text_new (); // deprecated in GTK 4.10
    w_gtk_combo_box_set_w_model (combo, FALSE);
#elif GTK_CHECK_VERSION(2,4,0)
    combo = gtk_combo_box_new_text (); // deprecated in GTK 2.24
    w_gtk_combo_box_set_w_model (combo, FALSE);
#elif !GTK_CHECK_VERSION(2,4,0)
    // GtkOptionMenu is ugly and not suitable for complex operations
    ///combo = gtk_option_menu_new (); // deprecated in GTK 2.4
    ///gtk_option_menu_set_menu (GTK_OPTION_MENU(combo), gtk_menu_new());
    combo = gtk_combo_new (); // deprecated in GTK 2.4
    gtk_editable_set_editable (GTK_EDITABLE(GTK_COMBO(combo)->entry), FALSE);
#endif
    return combo;
}

/*--------------------------------------------------------------
/              w_gtk_combobox_new_with_entry
/-------------------------------------------------------------*/

GtkWidget * w_gtk_combobox_new_with_entry ()
{
    GtkWidget * combo;
#if GTK_CHECK_VERSION(4,0,0) // 4,10,0 //TODO this has not been tested
    // Some people say GtkComboBox is broken in gtk4
    GtkStringList *list = gtk_string_list_new (NULL);
    combo = gtk_drop_down_new (G_LIST_MODEL(list), NULL);
    gtk_drop_down_set_enable_search (GTK_DROP_DOWN(combo), TRUE);
    W_DEBUG_PUTS ("w_gtk_combobox: new GtkDropDown");
#endif
#if GTK_CHECK_VERSION(2,24,0)
    combo = gtk_combo_box_text_new_with_entry ();  // deprecated in GTK 4.10
    w_gtk_combo_box_set_w_model (combo, FALSE);
    W_DEBUG_PUTS ("w_gtk_combobox: new GtkComboBoxText with Entry (w model)");
#elif GTK_CHECK_VERSION(2,4,0)
    combo = gtk_combo_box_entry_new_text ();  // deprecated in GTK 2.24
    w_gtk_combo_box_set_w_model (combo, FALSE);
    W_DEBUG_PUTS ("w_gtk_combobox: new GtkComboBoxEntry with text (w model)");
#elif !GTK_CHECK_VERSION(2,4,0)
    combo = gtk_combo_new ();  // deprecated in GTK 2.4
    W_DEBUG_PUTS ("w_gtk_combobox: new GtkCombo");
#endif
    w_gtk_combobox_set_search_case_insensitive (combo, FALSE);
    return combo;
}


/*--------------------------------------------------------------
/         w_gtk_combobox_add_changed_handler
/-------------------------------------------------------------*/

/* the "changed" signal
 
GtkComboBox:     changed (combo,data)
GtkOptionMenu:   changed (combo,data) [not in GTK1]
GtkCombo->entry: changed (entry,data) / combo = get_parent(entry)

How to handle the "changed" signal:

static void on_combo_changed_cb (GtkWidget *widget, gpointer user_data)
{
    char *text;
    GtkWidget *combo = w_gtk_combobox_cb_ensure_combo (widget);
    if (!combo) {
        return;
    }
    text = w_gtk_combobox_get_active_text (combo);
    if (text) {
        puts (text);
        g_free (text);
    }
}
*/

#if GTK_CHECK_VERSION(4,0,0)
static void dropdown_to_combobox_changed (GtkWidget *widget, GParamSpec *pspec, gpointer user_data)
{
    void (*func) (void*, void*);
    func = user_data;
    func(widget, NULL); // GtkDropdown: cb_data is not passed 
}
#endif


int w_gtk_combobox_add_changed_handler (GtkWidget *combo, gpointer cb, gpointer cb_data)
{
    int handler = -1;
#if GTK_CHECK_VERSION(4,0,0)
    if (GTK_IS_DROP_DOWN(combo)) //TODO: this has not been tested
    {
        W_DEBUG_PUTS ("w_gtk_combobox: add changed cb for GtkDropDown");
        // void selected_cb (GtkDropDown *widget, GParamSpec *pspec, gpointer user_data)
        //                                        ----(problem)----
        // I think there is also notify::selected-item
        handler = g_signal_connect (G_OBJECT(combo), "notify::selected",
                  G_CALLBACK(dropdown_to_combobox_changed), cb);
        return handler;
    }
#endif
#if (GTK_CHECK_VERSION(2,4,0) && GTK_MAJOR_VERSION <= 4)
    if (GTK_IS_COMBO_BOX(combo))
    {
        W_DEBUG_PUTS ("w_gtk_combobox: add changed cb for GtkComboBox");
        // void changed_cb (GtkComboBox *widget, gpointer user_data)
        handler = g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(cb), cb_data);
        return handler;
    }
#endif
#if GTK_MAJOR_VERSION <= 2
    if (GTK_IS_COMBO(combo))
    {
        W_DEBUG_PUTS ("w_gtk_combobox: add changed cb for GtkCombo entry");
        // void changed_cb (GtkEntry *widget, gpointer user_data)
        handler = g_signal_connect (G_OBJECT(GTK_COMBO(combo)->entry),
                                    "changed", G_CALLBACK(cb), cb_data);
        /// void selection_cb (GtkList *list, gpointer user_data)
        ///g_signal_connect (GTK_COMBO(combo)->list, "selection-changed",
        ///                  G_CALLBACK(cb), wcomboentry2);
    }
    else if (GTK_IS_OPTION_MENU(combo))
    {
        W_DEBUG_PUTS ("w_gtk_combobox: add changed cb for GtkOptionMenu");
#      if GTK_CHECK_VERSION(2,0,0)
        // void changed_cb (GtkOptionMenu *widget, gpointer user_data)
        handler = g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(cb), cb_data);
#      else
        // HACK: GTK1 doesn't implement the changed signal.. GtkMenuItems will call w_changed_cb
        g_object_set_data (G_OBJECT(combo), "w_changed_cb", cb);
        g_object_set_data (G_OBJECT(combo), "w_changed_cb_data", cb_data);
#      endif
    }
#endif
    if (handler == -1) {
        g_warning ("Invalid wcombo widget");
    }
    return handler;
}

/*--------------------------------------------------------------
/          w_gtk_combobox_cb_ensure_combo
/-------------------------------------------------------------*/

GtkWidget *w_gtk_combobox_cb_ensure_combo (GtkWidget *widget)
{
#if GTK_CHECK_VERSION(4,0,0)
    if (GTK_IS_DROP_DOWN(widget)) {
        W_DEBUG_PUTS ("wcombo changed signal: it's GtkDropDown");
        return widget;
    }
#endif
#if (GTK_CHECK_VERSION(2,4,0) && GTK_MAJOR_VERSION <= 4)
    if (GTK_IS_COMBO_BOX(widget)) {
        W_DEBUG_PUTS ("wcombo changed signal: it's GtkComboBox");
        return widget;
    }
#endif
#if GTK_MAJOR_VERSION <= 2
    if (GTK_IS_COMBO(gtk_widget_get_parent(widget))) {
        // GtkCombo doesn't have signals, this is coming from its GtkEntry or GtkList
        W_DEBUG_PUTS ("wcombo changed signal: it's GtkCombo entry");
        return gtk_widget_get_parent(widget);
    } else if (GTK_IS_OPTION_MENU(widget)) {
        // GtkOptionMenu has the changed signal only in GTK2
        W_DEBUG_PUTS ("wcombo changed signal: it's GtkOptionMenu");
        return widget;
    }
#  if GTK_MAJOR_VERSION == 1 // GTK1 Hack to emulate the changed signal in GtkOptionMenu
    else {
        GtkWidget *combo = g_object_get_data (G_OBJECT(widget), "w_gtk_combobox");
        if (GTK_IS_OPTION_MENU(combo)) {
            W_DEBUG_PUTS ("wcombo changed signal: GTK1 GtkOptionMenu hack");
            return combo;
        }
    }
#  endif
#endif
    //g_return_val_if_fail (combo != NULL, NULL);
    return NULL;
}


/*--------------------------------------------------------------
/             w_gtk_combobox_get_entry
/-------------------------------------------------------------*/

GtkWidget * w_gtk_combobox_get_entry (GtkWidget *combo)
{
    GtkWidget *entry = NULL;
    // GtkDropDown (currently unsupported)
#if GTK_CHECK_VERSION(4,0,0)
    if (GTK_IS_DROP_DOWN(combo)) // TODO
    {
        ///g_warning ("GtkDropDown: no entry");
        return entry;
    }
#endif
    // GtkComboBox
#if (GTK_CHECK_VERSION(2,4,0) && GTK_MAJOR_VERSION <= 4)
    if (GTK_IS_COMBO_BOX(combo))
    {
#   if GTK_MAJOR_VERSION == 4 // GtkBin was removed in gtk4
        entry = gtk_combo_box_get_child (GTK_COMBO_BOX(combo));
#   else // gtk 2 & 3
        entry = gtk_bin_get_child (GTK_BIN(combo));
#   endif
        if (entry && GTK_IS_ENTRY(entry)) {
            return entry;
        }
        return NULL;
    }
#endif
    // GtkCombo (GtkOptionMenu doesn't have an entry)
#if GTK_MAJOR_VERSION <= 2
    if (GTK_IS_COMBO(combo))
    {
        entry = GTK_COMBO(combo)->entry;
    }
#endif
    return entry;
}



/*--------------------------------------------------------------
/            w_gtk_combobox_set_enable_entry
/-------------------------------------------------------------*/

void w_gtk_combobox_set_enable_entry (GtkWidget *combo, gboolean enabled)
{
    GtkWidget *entry = w_gtk_combobox_get_entry (combo);
    if (entry) {
        gtk_editable_set_editable (GTK_EDITABLE(entry), enabled);
    }
}

/*--------------------------------------------------------------
/             w_gtk_combobox_set_entry_text
/-------------------------------------------------------------*/

void w_gtk_combobox_set_entry_text (GtkWidget *combo, const char *text)
{
    GtkWidget *entry = w_gtk_combobox_get_entry (combo);
    if (entry) {
#if GTK_CHECK_VERSION(4,0,0)
        gtk_editable_set_text (GTK_EDITABLE(entry), text ? text : "");
#else
        gtk_entry_set_text (GTK_ENTRY(entry), text ? text : "");
#endif
    }
}

