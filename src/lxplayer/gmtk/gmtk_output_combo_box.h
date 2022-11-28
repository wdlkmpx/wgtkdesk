/* not used - it's become a gtkcombobox example */

/*
 * gmtk_output_combo_box.h
 * Copyright (C) Kevin DeKorte 2009 <kdekorte@gmail.com>
 * 
 * gmtk_output_combo_box.h is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gmtk_output_combo_box.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 */

#ifndef __GMTK_OUTPUT_COMBO_BOX_H__
#define __GMTK_OUTPUT_COMBO_BOX_H__

G_BEGIN_DECLS
#define GMTK_TYPE_OUTPUT_COMBO_BOX		(gmtk_output_combo_box_get_type ())
#define GMTK_OUTPUT_COMBO_BOX(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GMTK_TYPE_OUTPUT_COMBO_BOX, GmtkOutputComboBox))
#define GMTK_OUTPUT_COMBO_BOX_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GMTK_OUTPUT_COMBO_BOX, GmtkOutputComboBoxClass))
#define GMTK_IS_OUTPUT_COMBO_BOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMTK_TYPE_OUTPUT_COMBO_BOX))
#define GMTK_IS_OUTPUT_COMBO_BOX_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GMTK_TYPE_OUTPUT_COMBO_BOX))
#define GMTK_OUTPUT_COMBO_BOX_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMTK_TYPE_OUTPUT_COMBO_BOX, GmtkOutputComboBoxClass))
typedef struct _GmtkOutputComboBox GmtkOutputComboBox;
typedef struct _GmtkOutputComboBoxClass GmtkOutputComboBoxClass;

struct _GmtkOutputComboBox
{
    GtkComboBox parent;
    /* < private > */
    GtkListStore *list;
};

struct _GmtkOutputComboBoxClass {
    GtkComboBoxClass parent_class;
};

GType gmtk_output_combo_box_get_type(void);
GtkWidget *gmtk_output_combo_box_new();
const gchar *gmtk_output_combo_box_get_active_device(GmtkOutputComboBox * output);
const gchar *gmtk_output_combo_box_get_active_description(GmtkOutputComboBox * output);
gint gmtk_output_combo_box_get_active_card(GmtkOutputComboBox * output);
gint gmtk_output_combo_box_get_active_index(GmtkOutputComboBox * output);

G_END_DECLS
#endif
