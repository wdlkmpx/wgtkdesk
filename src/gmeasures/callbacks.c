/***************************************************************************
    file        : callbacks.c
    project     : gMeasures
    date        : 2007.02.01 (v0.7)
    copyright   : (C)2007 by Paul Schuurmans
    email       : paul.schuurmans@home.nl
 ***************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
*/
 
#include "common.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "main.h"

extern GtkWidget *App, *EditDlg, *AboutDlg;
extern GPtrArray *Msr;
extern int Mode, InSetup;

void on_app_show (GtkWidget *widget, gpointer user_data)
{
	InSetup = 1;
	AppSetUnit1();
	AppUpdateUnit2();
	InSetup = 0;
}

void on_app_destroy (GtkWidget *object, gpointer user_data)
{
	gtk_main_quit();
}

void on_aboutdlg_btn_close_clicked (GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(AboutDlg);
}

void on_btn_about_clicked (GtkButton *button, gpointer user_data)
{
	if(!GTK_IS_WIDGET(AboutDlg))  AboutDlg = create_AboutDlg();
	gtk_widget_show_all (AboutDlg);
}

void on_btn_edit_add_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *w;
	TMeasuresInfo *m;
	char text[64] = "";
	if(!GTK_IS_WIDGET(EditDlg))  EditDlg = create_EditDlg();
	m = AppGetMeasure();
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_entry_unit1");
	gtk_entry_set_text(GTK_ENTRY(w), m->Unit1);
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_entry_value");
	sprintf(text, "%g", m->Equals);
	gtk_entry_set_text(GTK_ENTRY(w), text);
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_entry_unit2");
	gtk_entry_set_text(GTK_ENTRY(w), m->Unit2);
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_btn_delete");
	gtk_widget_set_sensitive(w, TRUE);
	gtk_widget_show_all (EditDlg);
}

void on_btn_exit_clicked (GtkButton *button, gpointer user_data)
{
	gtk_main_quit();
}


void on_btn_num_clicked (GtkButton *button, gpointer user_data)
{
    intptr_t num = GPOINTER_TO_INT (user_data);
    switch (num)
    {
        case 0: AppNumPadClicked("0"); break;
        case 1: AppNumPadClicked("1"); break;
        case 2: AppNumPadClicked("2"); break;
        case 3: AppNumPadClicked("3"); break;
        case 4: AppNumPadClicked("4"); break;
        case 5: AppNumPadClicked("5"); break;
        case 6: AppNumPadClicked("6"); break;
        case 7: AppNumPadClicked("7"); break;
        case 8: AppNumPadClicked("8"); break;
        case 9: AppNumPadClicked("9"); break;
        case 11: AppNumPadClicked("."); break; /* dot   */
        case 12: AppNumPadClicked("-"); break; /* minus */
        case 13: AppNumPadClicked("B"); break; /* backspace */
        case 14: AppNumPadClicked("C"); break; /* clear */
    }
}


void on_combo_unit1_changed (GtkWidget *combobox, gpointer user_data)
{
	Mode = 0;
	if(!InSetup)  AppUpdateUnit2();
}

void on_combo_unit2_changed (GtkWidget *combobox, gpointer user_data)
{
	Mode = 0;
	if(!InSetup)  AppCalculate();
}

void on_editdlg_btn_cancel_clicked (GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(EditDlg);
}

void on_editdlg_btn_delete_clicked (GtkButton *button, gpointer user_data)
{
	TMeasuresInfo *m;
	m = AppGetMeasure();
	g_ptr_array_remove(Msr, m);
	AppReadWriteConfig(OF_WRITE);
	InSetup = 1;
	AppSetUnit1();
	InSetup = 0;
	gtk_widget_hide(EditDlg);
	on_combo_unit1_changed(NULL, NULL);
}

void on_editdlg_btn_ok_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *w;
	TMeasuresInfo *m, *mn = NULL;
	char unit1[64], unit2[64], value[64];
	m = AppGetMeasure();
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_entry_unit1");
	strcpy(unit1, gtk_entry_get_text(GTK_ENTRY(w)));
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_entry_unit2");
	strcpy(unit2, gtk_entry_get_text(GTK_ENTRY(w)));
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_entry_value");
	strcpy(value, gtk_entry_get_text(GTK_ENTRY(w)));
	if(strcasecmp(unit1, m->Unit1) || strcasecmp(unit2, m->Unit2)) {
		mn = g_malloc0(sizeof(TMeasuresInfo));
		strcpy(mn->Unit1, unit1);
		strcpy(mn->Unit2, unit2);
		mn->Equals = atof(value);
		g_ptr_array_add(Msr, mn);
	}
	else {
		strcpy(m->Unit1, unit1);
		strcpy(m->Unit2, unit2);
		m->Equals = atof(value);
	}
	AppReadWriteConfig(OF_WRITE);
	InSetup = 1;
	AppSetUnit1();
	if(mn)  m = mn;
	w = lookup_widget(GTK_WIDGET(App), "combo_unit1");
    w_gtk_combobox_search_text (w, m->Unit1, TRUE, NULL);
	w = lookup_widget(GTK_WIDGET(App), "combo_unit2");
	w_gtk_combobox_search_text (w, m->Unit2, TRUE, NULL);
	InSetup = 0;
	gtk_widget_hide(EditDlg);
	AppCalculate();
}

void on_editdlg_entry_unit1_changed (GtkEditable *editable, gpointer user_data)
{
	GtkWidget *w;
	TMeasuresInfo *m;
	gchar *text;
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_btn_delete");
	if(gtk_widget_get_sensitive(w)) {
		m = AppGetMeasure();
		text = gtk_editable_get_chars(editable, 0, -1);
		if(strcasecmp(m->Unit1, text))  gtk_widget_set_sensitive(w, FALSE);
		g_free(text);
	}
}

void on_editdlg_entry_unit2_changed (GtkEditable *editable, gpointer user_data)
{
	GtkWidget *w;
	TMeasuresInfo *m;
	gchar *text;
	w = lookup_widget(GTK_WIDGET(EditDlg), "editdlg_btn_delete");
	if(gtk_widget_get_sensitive(w)) {
		m = AppGetMeasure();
		text = gtk_editable_get_chars(editable, 0, -1);
		if(strcasecmp(m->Unit2, text))  gtk_widget_set_sensitive(w, FALSE);
		g_free(text);
	}
}

void on_sb_entry_cdegrees_changed (GtkEditable *editable, gpointer user_data)
{
	if(gtk_widget_has_focus(GTK_WIDGET(editable))) {
		Mode = 1;
		AppCalculate();
	}
}

void on_sb_entry_cdegrees_grab_focus (GtkWidget *widget, gpointer user_data)
{
	Mode = 1;
}

void on_sb_entry_fdegrees_changed (GtkEditable *editable, gpointer user_data)
{
	if(gtk_widget_has_focus(GTK_WIDGET(editable))) {
		Mode = 2;
		AppCalculate();
	}
}

void on_sb_entry_fdegrees_grab_focus (GtkWidget *widget, gpointer user_data)
{
	Mode = 2;
}

void on_sb_entry_val1_changed (GtkEditable *editable, gpointer user_data)
{
	Mode = 0;
	AppCalculate();
}

void on_sb_entry_val1_grab_focus (GtkWidget *widget, gpointer user_data)
{
	Mode = 0;
}


