/*
 *      pref.h
 *
 *      Copyright (C) 2007 PCMan <pcman.tw@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "common.h"

#include <glib/gstdio.h>

#include "pref.h"
#include "main-win.h"

#define CFG_DIR    "gpicview"
#define CFG_FILE    CFG_DIR"/gpicview.conf"

Pref pref = {0};

static gboolean kf_get_bool(GKeyFile* kf, const char* grp, const char* name, gboolean* ret )
{
    GError* err = NULL;
    gboolean val = g_key_file_get_boolean(kf, grp, name, &err);
    if( G_UNLIKELY(err) )
    {
        g_error_free(err);
        return FALSE;
    }
    if(G_LIKELY(ret))
        *ret = val;
    return TRUE;
}

static int kf_get_int(GKeyFile* kf, const char* grp, const char* name, int* ret )
{
    GError* err = NULL;
    int val = g_key_file_get_integer(kf, grp, name, &err);
    if( G_UNLIKELY(err) )
    {
        g_error_free(err);
        return FALSE;
    }
    if(G_LIKELY(ret))
        *ret = val;
    return TRUE;
}

void load_preferences()
{
    /* FIXME: GKeyFile is not fast enough.
     *  Need to replace it with our own config loader in the future. */

    GKeyFile* kf;
    char* path;
    char* color;

    /* pref.auto_save_rotated = FALSE; */
    pref.ask_before_save = TRUE;
    pref.ask_before_delete = TRUE;
    pref.rotate_exif_only = TRUE;
    /* pref.open_maximized = FALSE; */
    pref.bg.red = pref.bg.green = pref.bg.blue = 65535;
    pref.bg_full.red = pref.bg_full.green = pref.bg_full.blue = 0;

    pref.jpg_quality = 90;
    pref.png_compression = 9;

    pref.show_toolbar = TRUE;

    kf = g_key_file_new();
    path = g_build_filename( g_get_user_config_dir(),  CFG_FILE, NULL );
    if( g_key_file_load_from_file( kf, path, 0, NULL ) )
    {
        kf_get_bool( kf, "General", "auto_save_rotated", &pref.auto_save_rotated );
        kf_get_bool( kf, "General", "ask_before_save", &pref.ask_before_save );
        kf_get_bool( kf, "General", "ask_before_delete", &pref.ask_before_delete );
        kf_get_bool( kf, "General", "rotate_exif_only", &pref.rotate_exif_only );
        kf_get_bool( kf, "General", "open_maximized", &pref.open_maximized );
        kf_get_int( kf, "General", "slide_delay", &pref.slide_delay );

        kf_get_int( kf, "General", "jpg_quality", &pref.jpg_quality);
        kf_get_int( kf, "General", "png_compression", &pref.png_compression );

        kf_get_bool( kf, "General", "show_toolbar", &pref.show_toolbar );

        color = g_key_file_get_string(kf, "General", "bg", NULL);
        if( color )
        {
            gdk_color_parse(color, &pref.bg);
            g_free(color);
        }

        color = g_key_file_get_string(kf, "General", "bg_full", NULL);
        if( color )
        {
            gdk_color_parse(color, &pref.bg_full);
            g_free(color);
        }
    }
    g_free( path );
    g_key_file_free( kf );

    if (pref.slide_delay == 0)
        pref.slide_delay = 5;
}

void save_preferences()
{
    FILE* f;
    char* dir = g_build_filename( g_get_user_config_dir(), CFG_DIR, NULL );
    char* path = g_build_filename( g_get_user_config_dir(),  CFG_FILE, NULL );
    if( ! g_file_test( dir, G_FILE_TEST_IS_DIR ) )
    {
        g_mkdir( g_get_user_config_dir(), 0766 );
        g_mkdir( dir, 0766 );
    }
    g_free( dir );

    if(  (f = fopen( path, "w" )) )
    {
        fputs( "[General]\n", f );
        fprintf( f, "auto_save_rotated=%d\n", pref.auto_save_rotated );
        fprintf( f, "ask_before_save=%d\n", pref.ask_before_save );
        fprintf( f, "ask_before_delete=%d\n", pref.ask_before_delete );
        fprintf( f, "rotate_exif_only=%d\n", pref.rotate_exif_only );
        fprintf( f, "open_maximized=%d\n", pref.open_maximized );
        fprintf( f, "bg=#%02x%02x%02x\n", pref.bg.red/256, pref.bg.green/256, pref.bg.blue/256 );
        fprintf( f, "bg_full=#%02x%02x%02x\n", pref.bg_full.red/256, pref.bg_full.green/256, pref.bg_full.blue/256 );
        fprintf( f, "slide_delay=%d\n", pref.slide_delay );

        fprintf( f, "jpg_quality=%d\n", pref.jpg_quality );
        fprintf( f, "png_compression=%d\n", pref.png_compression );

        fprintf( f, "show_toolbar=%d\n", pref.show_toolbar );

        fclose( f );
    }
    g_free( path );
}

static void on_set_default( GtkButton* btn, gpointer user_data )
{
    GtkWindow* parent=(GtkWindow*)user_data;
    GtkWidget* dlg=gtk_message_dialog_new_with_markup( parent, 0,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
            _("GPicView will become the default viewer for all supported image files on your system.\n"
                "(This will be done through \'xdg-mime\' program)\n\n"
                "<b>Are you sure you really want to do this?</b>") );
    if( gtk_dialog_run( (GtkDialog*)dlg ) == GTK_RESPONSE_OK )
    {
        const char cmd[]="xdg-mime default gpicview.desktop application/x-navi-animation image/bmp image/gif image/x-icns image/jpeg image/png image/x-portable-bitmap image/x-portable-graymap image/x-portable-pixmap image/x-quicktime image/svg+xml image/x-tga image/tiff image/vnd.microsoft.icon image/vnd.zbrush.pcx image/x-pcx image/x-win-bitmap image/wmf image/x-wmf image/x-xbitmap image/x-xpixmap";
        g_spawn_command_line_sync( cmd, NULL, NULL, NULL, NULL );
    }
    gtk_widget_destroy( dlg );
}

static void on_set_bg( GtkColorButton* btn, gpointer user_data )
{
    MainWin* parent=(MainWin*)user_data;
    gtk_color_button_get_color(GTK_COLOR_BUTTON(btn), &pref.bg);
    if( !parent->full_screen )
    {
        gtk_widget_modify_bg( parent->evt_box, GTK_STATE_NORMAL, &pref.bg );
        gtk_widget_queue_draw(parent->evt_box );
    }
}

static void on_set_bg_full( GtkColorButton* btn, gpointer user_data )
{
    MainWin* parent=(MainWin*)user_data;
    gtk_color_button_get_color(GTK_COLOR_BUTTON(btn), &pref.bg_full);
    if( parent->full_screen )
    {
        gtk_widget_modify_bg( parent->evt_box, GTK_STATE_NORMAL, &pref.bg_full );
        gtk_widget_queue_draw(parent->evt_box );
    }
}


void edit_preferences (GtkWindow* parent)
{
    GtkWidget *auto_save_btn, *ask_before_save_btn, *set_default_btn,
              *rotate_exif_only_btn, *slide_delay_spinner, *ask_before_del_btn, *bg_btn, *bg_full_btn;
    GtkWidget *dlg, *main_vbox = NULL, *hbox, *w;

    dlg = w_gtk_dialog_new (_("Preferences"), parent, FALSE, &main_vbox);

    ask_before_save_btn = gtk_check_button_new_with_label (_("Ask before saving images"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ask_before_save_btn), pref.ask_before_save );

    ask_before_del_btn = gtk_check_button_new_with_label (_("Ask before deleting images"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ask_before_del_btn), pref.ask_before_delete );

    auto_save_btn = gtk_check_button_new_with_label (_("Automatically save rotated images"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(auto_save_btn), pref.auto_save_rotated );

    rotate_exif_only_btn = gtk_check_button_new_with_label (_("Rotate JPEG file by changing EXIF orientation value"
"\n(only if EXIF orientation tag exists)"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(rotate_exif_only_btn), pref.rotate_exif_only );

    gtk_box_pack_start (GTK_BOX(main_vbox), ask_before_save_btn, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(main_vbox), ask_before_del_btn, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(main_vbox), auto_save_btn, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(main_vbox), rotate_exif_only_btn, TRUE, TRUE, 0);

    // ---------------------------

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start (GTK_BOX(main_vbox), hbox, TRUE, TRUE, 0);

    w = gtk_label_new (_("Slideshow interval"));
    gtk_box_pack_start (GTK_BOX(hbox), w, TRUE, TRUE, 0);

    w = gtk_spin_button_new_with_range (0.1, 1000.0, 0.5);
    slide_delay_spinner = w;
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(slide_delay_spinner), pref.slide_delay);
    gtk_box_pack_start (GTK_BOX(hbox), w, TRUE, TRUE, 0);

    w = gtk_label_new (_("sec"));
    gtk_box_pack_start (GTK_BOX(hbox), w, TRUE, TRUE, 0);

    // ---------------------------

    set_default_btn = gtk_button_new_with_label (_("Make GPicView the default viewer for images"));
    g_signal_connect( set_default_btn, "clicked", G_CALLBACK(on_set_default), parent );

    gtk_box_pack_start (GTK_BOX(main_vbox), set_default_btn, TRUE, TRUE, 0);

    // ---------------------------

    w = gtk_label_new ("");
    gtk_label_set_use_markup (GTK_LABEL(w), TRUE);
    gtk_label_set_markup (GTK_LABEL(w), _("<b>Background colors</b>"));
    gtk_box_pack_start (GTK_BOX(main_vbox), w, TRUE, TRUE, 7);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start (GTK_BOX(main_vbox), hbox, TRUE, FALSE, 0);

    w = gtk_label_new (_("Normal:"));
    gtk_box_pack_start (GTK_BOX(hbox), w, FALSE, FALSE, 0);

    w = gtk_color_button_new ();
    bg_btn = w;
    gtk_color_button_set_color(GTK_COLOR_BUTTON(bg_btn), &pref.bg);
    g_signal_connect( bg_btn, "color-set", G_CALLBACK(on_set_bg), parent );
    gtk_box_pack_start (GTK_BOX(hbox), w, FALSE, FALSE, 0);

    w = gtk_label_new (_("Fullscreen"));
    gtk_box_pack_start (GTK_BOX(hbox), w, FALSE, FALSE, 0);

    w = gtk_color_button_new ();
    bg_full_btn = w;
    gtk_color_button_set_color(GTK_COLOR_BUTTON(bg_full_btn), &pref.bg_full);
    g_signal_connect( bg_full_btn, "color-set", G_CALLBACK(on_set_bg_full), parent );
    gtk_box_pack_start (GTK_BOX(hbox), w, FALSE, FALSE, 0);

    w = gtk_label_new ("  ");
    gtk_box_pack_start (GTK_BOX(hbox), w, TRUE, TRUE, 0);

    // ---------------------------

    gtk_dialog_add_button (GTK_DIALOG(dlg), "gtk-close", GTK_RESPONSE_CLOSE);

    gtk_widget_show_all (dlg);
    gtk_dialog_run (GTK_DIALOG(dlg));

    pref.ask_before_save   = gtk_toggle_button_get_active ((GtkToggleButton*)ask_before_save_btn );
    pref.ask_before_delete = gtk_toggle_button_get_active ((GtkToggleButton*)ask_before_del_btn );
    pref.auto_save_rotated = gtk_toggle_button_get_active ((GtkToggleButton*)auto_save_btn );
    pref.rotate_exif_only  = gtk_toggle_button_get_active ((GtkToggleButton*)rotate_exif_only_btn );
    pref.slide_delay   = gtk_spin_button_get_value_as_int ((GtkSpinButton*)slide_delay_spinner );

    gtk_widget_destroy(dlg);
}
