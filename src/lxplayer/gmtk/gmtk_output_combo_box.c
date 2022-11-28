/* not used - it's become a gtkcombobox example */

/*
 * gmtk_output_combo_box.c
 * Copyright (C) Kevin DeKorte 2009 <kdekorte@gmail.com>
 * 
 * gmtk_output_combo_box.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gmtk_output_combo_box.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 */

#include "gmtk.h"

enum {
    OUTPUT_DESCRIPTION_COLUMN,
    OUTPUT_CARD_COLUMN,
    OUTPUT_DEVICE_COLUMN,
    OUTPUT_MPLAYER_DEVICE_COLUMN,
    OUTPUT_N_COLUMNS
};


static GObjectClass *parent_class = NULL;

gint sort_iter_compare_func(GtkTreeModel * model, GtkTreeIter * a, GtkTreeIter * b, gpointer data)
{
    int sortcol = GPOINTER_TO_INT(data); // must be text
    int ret = 0;
    char *a_desc;
    char *b_desc;

    gtk_tree_model_get (model, a, sortcol, &a_desc, -1);
    gtk_tree_model_get (model, b, sortcol, &b_desc, -1);

    ret = g_strcmp0 (a_desc, b_desc);
    g_free(a_desc);
    g_free(b_desc);

    return ret;
}

G_DEFINE_TYPE(GmtkOutputComboBox, gmtk_output_combo_box, GTK_TYPE_COMBO_BOX);

static void gmtk_output_combo_box_finalize(GObject * object)
{
    //fprintf(stdout, "gmtk_output_combo_box_finalize: %p\n", object);
    GmtkOutputComboBox *output = GMTK_OUTPUT_COMBO_BOX(object);

    if (output->list) {
        gtk_list_store_clear(output->list);
        g_object_unref(output->list);
        output->list = NULL;
    }
    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void gmtk_output_combo_box_class_init(GmtkOutputComboBoxClass * class)
{
    GObjectClass *oc = G_OBJECT_CLASS(class);
    oc->finalize = gmtk_output_combo_box_finalize;
    //GtkWidgetClass *wc = GTK_WIDGET_CLASS(class);

    parent_class = g_type_class_peek_parent(class);
}


static void gmtk_output_combo_box_init(GmtkOutputComboBox * output)
{
    GtkTreeIter iter;
    GtkCellRenderer *renderer;
    GtkTreeSortable *sortable;

    GList *gliter;
    AudioDevice *adevice; // gm_audio.c

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(output), renderer, FALSE);
    gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(output), renderer, "text", 0);

    output->list =
        gtk_list_store_new(OUTPUT_N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                           G_TYPE_STRING);

    gm_audio_update_device (NULL); // gm_audio.c
    gliter = gm_audio_devices;
    while (gliter)
    {
        adevice = (AudioDevice*) gliter->data;
        // --
        gtk_list_store_append(output->list, &iter);
        gtk_list_store_set(output->list, &iter,
                        OUTPUT_DESCRIPTION_COLUMN, adevice->description /*ALSA*/,
                        OUTPUT_CARD_COLUMN,       -1,
                        OUTPUT_DEVICE_COLUMN,     -1,
                        OUTPUT_MPLAYER_DEVICE_COLUMN, adevice->mplayer_ao /*alsa*/, -1);
        // --
        gliter = gliter->next;
    }

    sortable = GTK_TREE_SORTABLE(output->list);
    gtk_tree_sortable_set_sort_func(sortable, OUTPUT_DESCRIPTION_COLUMN,
                                    sort_iter_compare_func,
                                    GINT_TO_POINTER(OUTPUT_DESCRIPTION_COLUMN), NULL);
    gtk_tree_sortable_set_sort_column_id(sortable, OUTPUT_DESCRIPTION_COLUMN, GTK_SORT_ASCENDING);

    gtk_combo_box_set_model(GTK_COMBO_BOX(output), GTK_TREE_MODEL(output->list));
}


GtkWidget *gmtk_output_combo_box_new()
{
    GtkWidget *output;
    output = g_object_new(GMTK_TYPE_OUTPUT_COMBO_BOX, NULL);
    return output;
}


const gchar *gmtk_output_combo_box_get_active_device(GmtkOutputComboBox * output)
{
    GtkTreeIter iter;
    const gchar *device = NULL;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output), &iter)) {
        gtk_tree_model_get(GTK_TREE_MODEL(output->list), &iter, OUTPUT_MPLAYER_DEVICE_COLUMN, &device, -1);
    }
    return device;
}


const gchar *gmtk_output_combo_box_get_active_description(GmtkOutputComboBox * output)
{
    GtkTreeIter iter;
    const gchar *desc = NULL;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output), &iter)) {
        gtk_tree_model_get(GTK_TREE_MODEL(output->list), &iter, OUTPUT_DESCRIPTION_COLUMN, &desc, -1);
    }
    return desc;
}


gint gmtk_output_combo_box_get_active_card(GmtkOutputComboBox * output)
{
    GtkTreeIter iter;
    gint card;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output), &iter)) {
        gtk_tree_model_get(GTK_TREE_MODEL(output->list), &iter, OUTPUT_CARD_COLUMN, &card, -1);
    }
    return card;
}

