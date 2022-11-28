/***************************************************************************
    file        : main.c
    project     : gMeasures
    date        : unknown
    revised     : 2002.03.25 (v0.5)
                  2006.11.15 (v0.6)
                  2007.02.01 (v0.7)
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "common.h" 

#include "interface.h"
#include "support.h"
#include "jpslib2.h"
#include "main.h"

GtkWidget *App, *EditDlg, *AboutDlg;
GPtrArray *Msr;
int Mode;	// 0=Measurement, 1=Celsius->Fahrenheit, 2=Fahrenheit->Celsius
int InSetup;

void AppCalculate ()
{
	GtkEntry *entry_amount, *entry_result;
	TMeasuresInfo *m;
	gchar *valstr1, *valstr2;
	gdouble val1 = 0, val2 = 0;
	char *ent_amount_str[3] = { "sb_entry_val1", "sb_entry_cdegrees", "sb_entry_fdegrees" };
	char *ent_result_str[3] = { "entry_val2", "sb_entry_fdegrees", "sb_entry_cdegrees" };
	char text[32] = "";
	entry_amount = (GtkEntry *) lookup_widget(GTK_WIDGET(App), ent_amount_str[Mode]);
	entry_result = (GtkEntry *) lookup_widget(GTK_WIDGET(App), ent_result_str[Mode]);
	valstr1 = (gchar *) gtk_entry_get_text(entry_amount);
	valstr2 = (gchar *) gtk_entry_get_text(entry_result);
	if(strlen(valstr1)) {
		val1 = atof(valstr1);
		if(Mode == 2)  val2 = (val1 - (gdouble) 32) * ((gdouble) ((gdouble) 5 / (gdouble) 9));
		else if(Mode == 1)  val2 = (val1 * ((gdouble) ((gdouble) 9 / (gdouble) 5)) + (gdouble) 32);
		else {
			m = AppGetMeasure();
			val2 = val1 * ((gdouble) m->Equals);
		}
		sprintf(text, "%g", val2);
		gtk_entry_set_text(entry_result, text);
	}
}

TMeasuresInfo *AppGetMeasure ()
{
	GtkWidget *combo1, *combo2;
	TMeasuresInfo *m = NULL;
	gchar *unit1, *unit2;
	int i;
	combo1 = lookup_widget(GTK_WIDGET(App), "combo_unit1");
	combo2 = lookup_widget(GTK_WIDGET(App), "combo_unit2");
	unit1 = w_gtk_combobox_get_active_text (combo1);
	unit2 = w_gtk_combobox_get_active_text (combo2);
	if(strlen(unit1) && strlen(unit2)) {
		for(i = 0; i < Msr->len; i++) {
			m = g_ptr_array_index(Msr, i);
			if(!strcasecmp(m->Unit1, unit1) && !strcasecmp(m->Unit2, unit2))  break;
		}
	}
	g_free(unit1);
	g_free(unit2);
	return(m);
}

void AppNumPadClicked (char *key)
{
	GtkWidget *w;
	char *txt, text[80] = "";
	int pos = 0, len = 0;
	char *entstr[3] = { "sb_entry_val1", "sb_entry_cdegrees", "sb_entry_fdegrees" };
	w = lookup_widget(GTK_WIDGET(App), entstr[Mode]);
	txt = gtk_editable_get_chars(GTK_EDITABLE(w), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(w), 0, -1);
	switch(key[0]) {
	  case '-':
		if(*txt == '-')  strcpy(text, txt+1);
		else {
			len = strlen(txt);
			if(len == 1 && *txt == '0')  strcpy(text, txt);
			else {
				strcpy(text, "-");
				strcat(text, txt);
			}
		}
		break;
	  case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': 
	  case '9': case '0':
		len = strlen(txt);
		if(len == 1 && *txt == '0')  strcpy(text, key);
		else if(len == 2 && *txt == '-' && *(txt+1) == '0') {
			strcpy(text, "-");
			strcat(text, key);
		}
		else {
			strcpy(text, txt);
			strcat(text, key);
		}
		break;
	  case '.':
		strcpy(text, txt);
		if(strchr(txt, '.') == NULL)  strcat(text, key);
		break;
	  case 'B':
		strcpy(text, txt);
		len = strlen(text);
		if(len > 1)  text[len-1] = 0;
		else  strcpy(text, "0");
		break;
	  case 'C':
		strcpy(text, "0");
		break;
	  default:
		strcpy(text, txt);
		break;
	}
	gtk_editable_insert_text(GTK_EDITABLE(w), text, strlen(text), &pos);
	gtk_editable_set_position(GTK_EDITABLE(w), -1);
	g_free(txt);
}

void AppReadWriteConfig (int opmode)
{
	char *defs[101] = {
		"Acres:Hectares:0.4047",
		"Acres:Square_Miles:0.001562",
		"Acres:Square_Feet:43560",
		"BTU:Horsepower-Hours:0.0003931",
		"BTU:Kilowatt-Hours:0.0002928",
		"BTU/Hour:Watts:0.2931",
		"Bushels:Cubic_Inches:2150.4",
		"Bushels_(U.S.):Hectoliters:0.3524",
		"Centimeters:Feet:0.03281",
		"Centimeters:Inches:0.3937",
		"Circumference:Radians:6.283",
		"Cubic_Centimeters:Cubic_Inches:0.061024",
		"Cubic_Feet:Cubic_Meters:0.0283",
		"Cubic_Inches:Cubic_Centimeters:16.387064",
		"Cubic_Inches:Liters:0.016387",
		"Cubic_Meters:Cubic_Yards:1.3079",
		"Cubic_Meters:Cubic_Feet:35.3145",
		"Cubic_Yards:Cubic_Meters:0.7646",
		"Days:Hours:24",
		"Days:Minutes:1440",
		"Days:Seconds:86400",
		"Degrees:Radians:0.01745",
		"Dynes:Grams:0.00102",
		"Euros:Guilders:2.20371",
		"Fathoms:Feet:6",
		"Feet:Meters:0.3048",
		"Feet:Miles_(Nautical):0.0001645",
		"Feet:Miles_(Statute):0.0001894",
		"Feet_Per_Second:Miles_Per_Hour:0.6818",
		"Furlongs:Feet:660",
		"Furlongs:Miles:0.125",
		"Gallons_(U.S.):Liters:3.7853",
		"Grains:Grams:0.0648",
		"Grams:Grains:15.4324",
		"Grams:Pounds:0.002205",
		"Grams:Ounces_Avdp.:0.0353",
		"Guilders:Euros:0.45378021609",
		"Hectares:Acres:2.471005",
		"Hectoliters:Bushels_(U.S.):2.8378",
		"Horsepower:Watts:745.7",
		"Hours:Minutes:60",
		"Hours:Seconds:3600",
		"Hours:Days:0.041667",
		"Inches:Millimeters:25.4",
		"Inches:Centimeters:2.54",
		"Kilograms:Ounces:35",
		"Kilograms:Pounds:2.2046",
		"Kilometers:Miles:0.6214",
		"Kilowatts:Horsepower:1.341",
		"Knots:Nautical_Miles/Hr.:1",
		"Knots:Statute_Miles/Hr.:1.151",
		"Liters:Fluid_Ounces:34",
		"Liters:Gallons_(U.S.):0.2642",
		"Liters:Pecks:0.1135",
		"Liters:Pints_(Dry):1.8162",
		"Liters:Pints_(Liquid):2.1134",
		"Liters:Quarts_(Dry):0.9081",
		"Liters:Quarts_(Liquid):1.0567",
		"Meters:Feet:3.2808",
		"Meters:Miles:0.0006214",
		"Meters:Yards:1.0936",
		"Metric_Tons:Tons_(Long):0.9842",
		"Metric_Tons:Tons_(Short):1.1023",
		"Miles:Kilometers:1.609344",
		"Miles:Feet:5280",
		"Miles_(Nautical):Miles_(Statute):1.1516",
		"Miles_(Satute):Miles_(Nautical):0.8684",
		"Miles_Per_Hour:Feet_Per_Minute:88",
		"Millimeters:Inches:0.0394",
		"Minutes:Days:0.00069444",
		"Minutes:Hours:0.016667",
		"Minutes:Seconds:60",
		"Ounces:Pounds:0.0625",
		"Ounces_(Avdp.):Grams:28.3495",
		"Ounces_(Troy):Ounces_(Avdp.):1.09714",
		"Pecks:Liters:8.8096",
		"Pints_(Dry):Liters:0.5506",
		"Pints_(Liquid):Liters:0.4732",
		"Pounds:Ounces:16",
		"Pounds:Kilograms:0.3782",
		"Pounds_Avdp.:Kilograms:0.4536",
		"Quarts_(Dry):Liters:1.1012",
		"Quarts_(Liquid):Liters:0.9463",
		"Radians:Degrees:57.3",
		"Rods:Feet:16.5",
		"Rods:Meters:5.029",
		"Seconds:Minutes:0.01666667",
		"Square_Feet:Square_Meters:0.0929",
		"Square_Kilometers:Square_Miles:0.3861",
		"Square_Meters:Square_Feet:10.7639",
		"Square_Meters:Square_Yards:1.196",
		"Square_Miles:Square_Kilometers:2.59",
		"Square_Yards:Square_Meters:0.8361",
		"Tons_(Long):Metric_Tons:1.106",
		"Tons_(Long):Pounds:2240",
		"Tons_(Short):Metric_Tons:0.9072",
		"Tons_(Short):Pounds:2000",
		"Watts:BTU/Hour:3.4129",
		"Watts:Horsepower:0.001341",
		"Yards:Meters:0.9144",
		"Yards:Miles:0.0005682"
	};
	FILE *fn = NULL;
	TMeasuresInfo *m;
	char *key, *sval, estr[32];
	int i, j, field = 0, ndx = 0, mcnt = 101;
	char configfile[MAXPATH] = "";
	key = g_malloc0(512);
	sval = g_malloc0(512);
	sprintf(configfile, "%s/.gmeasures.cfg", getenv("HOME"));
	if(opmode == OF_WRITE) {
		if((fn = fopen(configfile, "w+")) == NULL) {
			g_print("Error: AppReadWriteConfig(), fopen(%s, \"w+\") failed!\n", configfile);
			g_free(sval);
			g_free(key);
			return;
		}
		for(i = 0; i < Msr->len; i++) {
			if((m = g_ptr_array_index(Msr, i)) == NULL)  break;
			sprintf(sval, "msr%04d=%s:%s:%g", i, m->Unit1, m->Unit2, m->Equals);
			for(j = 0; j < strlen(sval); j++)  if(sval[j] == ' ') sval[j] = '_';
			fprintf(fn, "%s\n", sval);
		}
	}
	else {
		ClearArray(Msr);
		fn = fopen(configfile, "r");
		mcnt = (fn == NULL) ? 101 : 1000;
		for(i = 0; i < mcnt; i++) {
			sprintf(key, "msr%04d", i);
			*sval = 0;
			GetConfigString(fn, key, sval, defs[i]);
			if(*sval == 0)  break;
			m = g_malloc0(sizeof(TMeasuresInfo));
			for(j = field = ndx = 0; j < strlen(sval); j++) {
				switch(*(sval+j)) {
				  case '_':  *(sval+j) = ' ';  break;
				  case ':':  field++;  ndx = 0;  j++;  break;
				  default:  break;
				}
				switch(field) {
				  case 0:  m->Unit1[ndx] = *(sval+j);  ndx++;  break;
				  case 1:  m->Unit2[ndx] = *(sval+j);  ndx++;  break;
				  case 2:  estr[ndx] = *(sval+j);  estr[ndx+1] = 0;  ndx++;  break;
				  default:  break;
				}
			}
			m->Equals = atof(estr);
			g_ptr_array_add(Msr, m);
		}
	}
	if(fn)  fclose(fn);
	g_free(key);
	g_free(sval);
}

void AppSetUnit1 ()
{
	GtkWidget *combobox;
	//GtkTreeModel *model;
	TMeasuresInfo *m, *cbm;
	int i, ndx, found;
	InSetup = 1;
	combobox = lookup_widget(GTK_WIDGET(App), "combo_unit1");
	//model = gtk_combo_box_get_model(combobox);
	w_gtk_combobox_clear (combobox);
	m = g_ptr_array_index(Msr, 0);
	w_gtk_combobox_append_text (combobox, m->Unit1);
	for(i = 1; i < Msr->len; i++) {
		m = g_ptr_array_index(Msr, i);
		ndx = i - 1;
		for(found = 0; !found && ndx > -1; ndx--) {
			cbm = g_ptr_array_index(Msr, ndx);
			if(!strcasecmp(m->Unit1, cbm->Unit1))  found++;
		}
		if(!found) {
            w_gtk_combobox_append_text (combobox, m->Unit1);
        }
	}
	//gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(model), LIST_UNIT1, GTK_SORT_ASCENDING);
    w_gtk_combobox_set_active_index (combobox, 0);
	InSetup = 0;
}

void AppUpdateUnit2 ()
{
	GtkWidget *combo_unit1, *combo_unit2;
	//GtkTreeModel *model;
	TMeasuresInfo *m;
	char *unit1;
	int i;
	InSetup = 1;
	combo_unit1 = lookup_widget(GTK_WIDGET(App), "combo_unit1");
	combo_unit2 = lookup_widget(GTK_WIDGET(App), "combo_unit2");
	unit1 = w_gtk_combobox_get_active_text (combo_unit1);
	//model = gtk_combo_box_get_model(combo_unit2);
	w_gtk_combobox_clear (combo_unit2);
	for(i = 0; i < Msr->len; i++) {
		m = g_ptr_array_index(Msr, i);
		if(!strcasecmp(m->Unit1, unit1)) {
			w_gtk_combobox_append_text (combo_unit2, m->Unit2);
		}
	}
	g_free(unit1);
	//gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(model), LIST_UNIT1, GTK_SORT_ASCENDING);
	w_gtk_combobox_set_active_index (combo_unit2, 0);
	AppCalculate();
	InSetup = 0;
}


int main (int argc, char *argv[])
{
    w_gtk_check_version (GTK_MAJOR_VERSION);

	gtk_init(&argc, &argv);
	add_pixmap_directory (PACKAGE_DATADIR "/" PACKAGE "/pixmaps");
	Msr = g_ptr_array_new();
	AppReadWriteConfig(OF_READ);
	AboutDlg = create_AboutDlg();
	EditDlg = create_EditDlg();
	App = create_App();
	gtk_widget_show_all (App);
	gtk_main();
	g_ptr_array_free(Msr, TRUE);
	return(0);
}

