/*
Asunder

Copyright(C) 2005 Eric Lathrop <eric@ericlathrop.com>
Copyright(C) 2007 Andrew Smith <http://littlesvr.ca/contact.php>

Any code in this file may be redistributed or modified under the terms of
the GNU General Public Licence as published by the Free Software 
Foundation; version 2 of the licence.

*/

#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "main.h"
#include "prefs.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "completion.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
    g_object_set_data_full (G_OBJECT (component), name, \
        g_object_ref (widget), (GDestroyNotify) g_object_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
    g_object_set_data (G_OBJECT (component), name, widget)


GtkWidget *create_main (void)
{
    GtkWidget *main_win;
    GdkPixbuf *main_icon_pixbuf;
    GtkWidget *vbox1;
    GtkWidget *toolbar1;
    GtkWidget *lookup;
    GtkWidget *preferences;
    GtkWidget *separatortoolitem1;
    GtkWidget *table;
    GtkWidget *album_artist;
    GtkWidget *album_title;
    GtkWidget *pick_disc;
    GtkWidget *disc;
    GtkWidget *artist_label;
    GtkWidget *title_label;
    GtkWidget *single_artist;
    GtkWidget *scrolledwindow1;
    GtkWidget *tracklist;
    GtkWidget *rip_button;
    GtkWidget *alignment3;
    GtkWidget *hbox4;
    GtkWidget *image1;
    GtkWidget *label8;
    GtkWidget* hbox5;
    GtkWidget* fillerBox;
    GtkWidget* statusLbl;
    GtkWidget *album_genre;			// lnr
    GtkWidget *genre_label;			// lnr
    
    main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (main_win), "Asunder");
    
    gtk_window_set_default_size (GTK_WINDOW (main_win), global_prefs->main_window_width, global_prefs->main_window_height);
    main_icon_pixbuf = create_pixbuf ("asunder.svg");
    if (main_icon_pixbuf)
    {
        gtk_window_set_icon (GTK_WINDOW (main_win), main_icon_pixbuf);
        g_object_unref (main_icon_pixbuf);
    }

    vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (main_win), vbox1);

    toolbar1 = gtk_toolbar_new ();
    gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);
    
    GtkWidget* icon;
    icon = gtk_image_new_from_stock("gtk-refresh", gtk_toolbar_get_icon_size(GTK_TOOLBAR(toolbar1)));
    lookup = (GtkWidget*)gtk_tool_button_new(icon, _("CDDB Lookup"));
    gtk_container_add (GTK_CONTAINER (toolbar1), lookup);
    gtk_tool_item_set_is_important (GTK_TOOL_ITEM (lookup), TRUE);

    preferences = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-preferences");
    gtk_container_add (GTK_CONTAINER (toolbar1), preferences);
    gtk_tool_item_set_is_important (GTK_TOOL_ITEM (preferences), TRUE);

    separatortoolitem1 = (GtkWidget*) gtk_separator_tool_item_new ();
    gtk_container_add (GTK_CONTAINER (toolbar1), separatortoolitem1);

    GtkWidget *about;
    about = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-about");
    gtk_container_add (GTK_CONTAINER (toolbar1), about);
    gtk_tool_item_set_is_important (GTK_TOOL_ITEM (about), TRUE);
    
    table = w_gtk_grid_new (vbox1, 0);

    //-- row 0
    disc = gtk_label_new (_("Disc:"));
    gtk_grid_attach (GTK_GRID(table),      disc, 0, 0, 1, 1);

    pick_disc = gtk_combo_box_new ();
    gtk_grid_attach (GTK_GRID(table), pick_disc, 1, 0, 1, 1);
    gtk_widget_set_hexpand (pick_disc, TRUE);

    //-- row 1
    artist_label = gtk_label_new (_("Album Artist:"));
    gtk_grid_attach (GTK_GRID(table), artist_label, 0, 1, 1, 1);

    album_artist = gtk_entry_new ();
    create_completion(album_artist, "album_artist");
    gtk_grid_attach (GTK_GRID(table), album_artist, 1, 1, 1, 1);
    gtk_widget_set_hexpand (album_artist, TRUE);

    single_artist = gtk_check_button_new_with_mnemonic (_("Single Artist"));
    gtk_grid_attach (GTK_GRID(table), single_artist, 2, 1, 1, 1);
    gtk_widget_set_halign (single_artist, GTK_ALIGN_START);

    //-- row 2
    title_label = gtk_label_new (_("Album Title:"));
    gtk_grid_attach (GTK_GRID(table), title_label, 0, 2, 1, 1);

    album_title = gtk_entry_new ();
    create_completion(album_title, "album_title");
    gtk_grid_attach (GTK_GRID(table), album_title, 1, 2, 1, 1);
    gtk_widget_set_hexpand (album_title, TRUE);

    //-- row 3
    genre_label	= gtk_label_new (_("Genre / Year:"));
    gtk_grid_attach (GTK_GRID(table), genre_label, 0, 3, 1, 1);

    album_genre = gtk_entry_new();
    gtk_grid_attach (GTK_GRID(table), album_genre, 1, 3, 1, 1);
    create_completion(album_genre, "album_genre");
    gtk_widget_set_hexpand (album_genre, TRUE);

    GtkWidget* album_year = gtk_entry_new();
    gtk_grid_attach (GTK_GRID(table), album_year, 2, 3, 1, 1);
    gtk_widget_set_halign (album_year, GTK_ALIGN_START);

    //-- row 4
    GtkWidget* tn_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID(table), tn_hbox, 0, 4, 3, 1);

    GtkWidget *tn_labelo = gtk_label_new (_("First track number:"));
    gtk_misc_set_alignment (GTK_MISC (tn_labelo), 0, 0.5);
    gtk_box_pack_start(GTK_BOX (tn_hbox), tn_labelo, FALSE, TRUE, 0);

    GtkWidget* tn_first = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX (tn_hbox), tn_first, FALSE, TRUE, 0);
    gtk_entry_set_width_chars(GTK_ENTRY (tn_first), 4);
    char txt[16];
    snprintf(txt, 16, "%d", global_prefs->first_track_num_offset + 1);
    gtk_entry_set_text(GTK_ENTRY (tn_first), txt);

    GtkWidget* tn_labelw = gtk_label_new (_("Track number width in filename:"));
    gtk_misc_set_alignment (GTK_MISC (tn_labelw), 0, 0.5);
    gtk_box_pack_start(GTK_BOX (tn_hbox), tn_labelw, FALSE, TRUE, 0);

    GtkWidget* tn_combo_width = gtk_combo_box_text_new ();
    gtk_box_pack_start(GTK_BOX (tn_hbox), tn_combo_width, FALSE, TRUE, 0);

    char buff[2];
    for (int i = 1; i <= 4; ++i) {
        snprintf(buff, 2, "%d", i);
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(tn_combo_width), buff);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(tn_combo_width), global_prefs->track_num_width - 1);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    tracklist = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), tracklist);
#if GTK_MAJOR_VERSION == 2
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tracklist), TRUE);
#endif
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (tracklist), FALSE);

    hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX (vbox1), hbox5, FALSE, TRUE, 5);
    
    statusLbl = gtk_label_new("");
    gtk_label_set_use_markup(GTK_LABEL(statusLbl), TRUE);
    gtk_misc_set_alignment(GTK_MISC(statusLbl), 0, 0.5);
    gtk_box_pack_start(GTK_BOX (hbox5), statusLbl, TRUE, TRUE, 0);
    
    fillerBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX (hbox5), fillerBox, TRUE, TRUE, 0);
    
    rip_button = gtk_button_new ();
    gtk_box_pack_start(GTK_BOX (hbox5), rip_button, FALSE, FALSE, 5);
    
    alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (rip_button), alignment3);

    hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add (GTK_CONTAINER (alignment3), hbox4);
    
    image1 = gtk_image_new_from_stock ("gtk-cdrom", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start (GTK_BOX (hbox4), image1, FALSE, FALSE, 0);

    label8 = gtk_label_new_with_mnemonic (_("Rip"));
    gtk_box_pack_start (GTK_BOX (hbox4), label8, FALSE, FALSE, 0);

    g_signal_connect ((gpointer) main_win, "delete_event",
                      G_CALLBACK (on_window_close), NULL);

    g_signal_connect((gpointer) tracklist, "button-press-event", 
                     G_CALLBACK (on_tracklist_mouse_click), NULL);
    
    g_signal_connect ((gpointer) lookup, "clicked",
                      G_CALLBACK (on_lookup_clicked), NULL);

    g_signal_connect ((gpointer) preferences, "clicked",
                      G_CALLBACK (on_preferences_clicked), NULL);

    g_signal_connect ((gpointer) about, "clicked",
                      G_CALLBACK (on_about_clicked), NULL);

    g_signal_connect ((gpointer) album_artist, "focus_out_event",
                      G_CALLBACK (on_album_artist_focus_out_event), NULL);

    g_signal_connect ((gpointer) album_title, "focus_out_event",
                       G_CALLBACK (on_album_title_focus_out_event), NULL);

    g_signal_connect ((gpointer) pick_disc, "changed",
                      G_CALLBACK (on_pick_disc_changed), NULL);

    g_signal_connect ((gpointer) single_artist, "toggled",
                      G_CALLBACK (on_single_artist_toggled), NULL);

    g_signal_connect ((gpointer) rip_button, "clicked",
                      G_CALLBACK (on_rip_button_clicked), NULL);

    g_signal_connect ((gpointer) album_genre, "focus_out_event",// lnr
                      G_CALLBACK (on_album_genre_focus_out_event), NULL);

    g_signal_connect ((gpointer) album_year, "focus_out_event",
                      G_CALLBACK (on_year_focus_out_event), NULL);

    g_signal_connect ((gpointer) tn_first, "focus_out_event",
                      G_CALLBACK (on_tracknum_first_focus_out_event), NULL);

    g_signal_connect ((gpointer) tn_combo_width, "changed",
                      G_CALLBACK (on_tracknum_width_changed_event), NULL);

    /* KEYBOARD accelerators */
    GtkAccelGroup* accelGroup;
    guint accelKey;
    GdkModifierType accelModifier;
    GClosure *closure = NULL;
    
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(main_win), accelGroup);
    
    gtk_accelerator_parse("<Control>W", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(on_window_close), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    
    gtk_accelerator_parse("<Control>Q", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(on_window_close), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    
    gtk_accelerator_parse("F2", &accelKey, &accelModifier);
    closure = g_cclosure_new(G_CALLBACK(on_press_f2), NULL, NULL);
    gtk_accel_group_connect(accelGroup, accelKey, accelModifier, GTK_ACCEL_VISIBLE, closure);
    /* END KEYBOARD accelerators */

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (main_win, main_win, "main");
    GLADE_HOOKUP_OBJECT (main_win, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT (main_win, toolbar1, "toolbar1");
    GLADE_HOOKUP_OBJECT (main_win, lookup, "lookup");
    GLADE_HOOKUP_OBJECT (main_win, preferences, "preferences");
    GLADE_HOOKUP_OBJECT (main_win, separatortoolitem1, "separatortoolitem1");
    GLADE_HOOKUP_OBJECT (main_win, about, "about");
    GLADE_HOOKUP_OBJECT (main_win, table, "table2");
    GLADE_HOOKUP_OBJECT (main_win, album_artist, "album_artist");
    GLADE_HOOKUP_OBJECT (main_win, album_title, "album_title");
    GLADE_HOOKUP_OBJECT (main_win, pick_disc, "pick_disc");
    GLADE_HOOKUP_OBJECT (main_win, disc, "disc");
    GLADE_HOOKUP_OBJECT (main_win, artist_label, "artist_label");
    GLADE_HOOKUP_OBJECT (main_win, title_label, "title_label");
    GLADE_HOOKUP_OBJECT (main_win, single_artist, "single_artist");
    GLADE_HOOKUP_OBJECT (main_win, scrolledwindow1, "scrolledwindow1");
    GLADE_HOOKUP_OBJECT (main_win, tracklist, "tracklist");
    GLADE_HOOKUP_OBJECT (main_win, rip_button, "rip_button");
    GLADE_HOOKUP_OBJECT (main_win, alignment3, "alignment3");
    GLADE_HOOKUP_OBJECT (main_win, hbox4, "hbox4");
    GLADE_HOOKUP_OBJECT (main_win, image1, "image1");
    GLADE_HOOKUP_OBJECT (main_win, label8, "label8");
    GLADE_HOOKUP_OBJECT (main_win, statusLbl, "statusLbl");
    GLADE_HOOKUP_OBJECT (main_win, album_genre, "album_genre");			// lnr
    GLADE_HOOKUP_OBJECT (main_win, genre_label, "genre_label" );		// lnr
    GLADE_HOOKUP_OBJECT (main_win, album_year, "album_year");
    GLADE_HOOKUP_OBJECT (main_win, tn_hbox, "tn_hbox");
    GLADE_HOOKUP_OBJECT (main_win, tn_first, "tn_first");
    GLADE_HOOKUP_OBJECT (main_win, tn_combo_width, "tn_width");

    gtk_widget_show_all (main_win);
    gtk_widget_hide (disc);
    gtk_widget_hide (pick_disc);
    return main_win;
}


GtkWidget* create_prefs (void)
{
    GtkWidget *label;
    GtkWidget *prefs;
    GtkWidget *notebook1;
    GtkWidget *vbox;
    GtkWidget *main_vbox;
    GtkWidget *vbox2;
    GtkWidget *music_dir;
    GtkWidget *make_playlist;
    GtkWidget *hbox12;
    GtkWidget *cdrom;
    GtkWidget *table1;
    GtkWidget *format_music;
    GtkWidget *format_albumdir;
    GtkWidget *format_playlist;
    GtkWidget *rip_wav;
    GtkWidget *mp3_vbr;
    GtkWidget *hbox9;
    GtkWidget *rip_mp3;
    GtkWidget *vbox2X;
    GtkWidget *hbox9X;
    GtkWidget *rip_fdkaac;
    GtkWidget *hbox10;
    GtkWidget *hbox11;
    GtkWidget *rip_flac;
    GtkWidget *cancelbutton1;
    GtkWidget *okbutton1;
    GtkWidget *eject_on_done;
    GtkWidget* hboxFill;
    
    prefs = w_gtk_dialog_new (_("Preferences"),
                              GTK_WINDOW(win_main),
                              FALSE,
                              &vbox);
    main_vbox = vbox;
    notebook1 = gtk_notebook_new ();
    gtk_box_pack_start (GTK_BOX (vbox), notebook1, TRUE, TRUE, 0);
    
    /* GENERAL tab */
    vbox = w_gtk_notebook_add_tab_box (notebook1, _("General"));

    label = gtk_label_new (_("Destination folder"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    music_dir = gtk_file_chooser_button_new(_("Destination folder"), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    gtk_box_pack_start (GTK_BOX (vbox), music_dir, FALSE, FALSE, 0);
    
    make_playlist = gtk_check_button_new_with_mnemonic (_("Create M3U playlist"));
    gtk_box_pack_start (GTK_BOX (vbox), make_playlist, FALSE, FALSE, 0);

    hbox12 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hbox12, FALSE, FALSE, 0);

    label = gtk_label_new (_("CD-ROM device: "));
    gtk_box_pack_start (GTK_BOX (hbox12), label, FALSE, FALSE, 0);
    
    cdrom = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (hbox12), cdrom, TRUE, TRUE, 0);
    
    gtk_widget_set_tooltip_text (cdrom, _("Default: /dev/cdrom\n" "Other example: /dev/hdc\n" "Other example: /dev/sr0"));
    
    eject_on_done = gtk_check_button_new_with_mnemonic (_("Eject disc when finished"));
    gtk_box_pack_start (GTK_BOX (vbox), eject_on_done, FALSE, FALSE, 5);

    hboxFill = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hboxFill, TRUE, TRUE, 0);
    /* END GENERAL tab */
    
    /* FILENAMES tab */
    vbox2 = w_gtk_notebook_add_tab_box (notebook1, _("Filenames"));
    vbox = w_gtk_frame_vbox_new (_("<b>Filename formats</b>"), 0, vbox2, NULL);

    table1 = w_gtk_grid_new (vbox, 0);
    
    label = gtk_label_new (_("%A - Artist\n%L - Album\n%N - Track number (2-digit)\n%Y - Year (4-digit or \"0\")\n%T - Song title"));
    gtk_grid_attach (GTK_GRID(table1), label, 0, 0, 2, 1);
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    
    label = gtk_label_new (_("%G - Genre"));
    gtk_grid_attach (GTK_GRID(table1), label, 0, 1, 2, 1);
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    
    // problem is that the same albumdir is used (threads.c) for all formats
    //~ label = gtk_label_new (_("%F - Format (e.g. FLAC)"));
    //~ gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    //~ gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
    
    label = gtk_label_new (_("Album directory: "));
    gtk_grid_attach (GTK_GRID(table1), label, 0, 2, 1, 1);
    gtk_widget_set_halign (label, GTK_ALIGN_START);

    label = gtk_label_new (_("Playlist file: "));
    gtk_grid_attach (GTK_GRID(table1), label, 0, 3, 1, 1);
    gtk_widget_set_halign (label, GTK_ALIGN_START);

    label = gtk_label_new (_("Music file: "));
    gtk_grid_attach (GTK_GRID(table1), label, 0, 4, 1, 1);
    gtk_widget_set_halign (label, GTK_ALIGN_START);

    format_albumdir = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID(table1), format_albumdir, 1, 2, 1, 1);
    gtk_widget_set_hexpand (format_albumdir, TRUE);
    gtk_widget_set_tooltip_text (format_albumdir, _("This is relative to the destination folder (from the General tab).\n"
                                                    "Can be blank.\n"
                                                    "Default: %A - %L\n"
                                                    "Other example: %A/%L"));
    format_playlist = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID(table1), format_playlist, 1, 3, 1, 1);
    gtk_widget_set_hexpand (format_playlist, TRUE);
    gtk_widget_set_tooltip_text (format_playlist, _("This will be stored in the album directory.\n"
                                                    "Can be blank.\n" "Default: %A - %L"));
    format_music = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID(table1), format_music, 1, 4, 1, 1);
    gtk_widget_set_hexpand (format_music, TRUE);
    gtk_widget_set_tooltip_text (format_music, _("This will be stored in the album directory.\n"
                                                 "Cannot be blank.\n"
                                                 "Default: %A - %T\n"
                                                 "Other example: %N - %T"));
    GtkWidget * allow_tracknum = gtk_check_button_new_with_label (_("Allow changing first track number"));
    gtk_box_pack_start (GTK_BOX (vbox2), allow_tracknum, FALSE, FALSE, 5);
    GLADE_HOOKUP_OBJECT (prefs, allow_tracknum, "allow_tracknum");
    /* END FILENAMES tab */
    
    /* ENCODE tab */
    GtkWidget *mp3bitrate;
    GtkWidget *fdkaac_bitrate;
    GtkWidget *oggLbl;
    GtkWidget *oggquality;
    GtkWidget *rip_ogg;
    GtkWidget *flacLbl;
    GtkWidget *flaccompression;
    
    vbox = w_gtk_notebook_add_tab_box (notebook1, _("Encode"));
    
    /* WAV */
    rip_wav = gtk_check_button_new_with_mnemonic (_("WAV (uncompressed)"));
    gtk_box_pack_start (GTK_BOX (vbox), rip_wav, FALSE, FALSE, 0);
    /* END WAV */


    /* FLAC */
    rip_flac = gtk_check_button_new_with_mnemonic (_("FLAC (lossless compression)"));
    g_signal_connect ((gpointer) rip_flac, "toggled",
                       G_CALLBACK (on_rip_flac_toggled), NULL);

    vbox2 = w_gtk_frame_vbox_new (NULL, 0, vbox, rip_flac);

    hbox11 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox11, TRUE, TRUE, 0);

    flacLbl = gtk_label_new (_("Compression level"));
    gtk_box_pack_start (GTK_BOX (hbox11), flacLbl, FALSE, FALSE, 0);
    
    flaccompression = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL,
                                     GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 9, 1, 1, 1)));
    gtk_box_pack_start (GTK_BOX (hbox11), flaccompression, TRUE, TRUE, 5);
    gtk_scale_set_value_pos (GTK_SCALE (flaccompression), GTK_POS_RIGHT);
    gtk_scale_set_digits (GTK_SCALE (flaccompression), 0);

    gtk_widget_set_tooltip_text (flaccompression, _("This does not affect the quality. Higher number means smaller file."));
    /* END FLAC */

    
    /* MP3 */
    rip_mp3 = gtk_check_button_new_with_mnemonic (_("MP3 (lossy compression)"));
    g_signal_connect ((gpointer) rip_mp3, "toggled",
                       G_CALLBACK (on_rip_mp3_toggled), NULL);

    vbox2 = w_gtk_frame_vbox_new (NULL, 0, vbox, rip_mp3);

    mp3_vbr = gtk_check_button_new_with_mnemonic (_("Variable bit rate (VBR)"));
    gtk_box_pack_start (GTK_BOX (vbox2), mp3_vbr, FALSE, FALSE, 0);
    g_signal_connect ((gpointer) mp3_vbr, "toggled",
                                        G_CALLBACK (on_vbr_toggled),
                                        NULL);
    gtk_widget_set_tooltip_text (mp3_vbr, _("Better quality for the same size."));
    
    hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox9, TRUE, TRUE, 0);
    
    label = gtk_label_new (_("Bitrate"));
    gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, label, "bitrate_lbl");
    
    mp3bitrate = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL,
                                GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 14, 1, 1, 1)));
    gtk_box_pack_start (GTK_BOX (hbox9), mp3bitrate, TRUE, TRUE, 5);
    gtk_scale_set_draw_value (GTK_SCALE (mp3bitrate), FALSE);
    gtk_scale_set_digits (GTK_SCALE (mp3bitrate), 0);
    g_signal_connect ((gpointer) mp3bitrate, "value_changed",
                                        G_CALLBACK (on_mp3bitrate_value_changed),
                                        NULL);
    gtk_widget_set_tooltip_text (mp3bitrate, _("Higher bitrate is better quality but also bigger file. Most people use 192Kbps."));
    
    char kbps_text[10];
    snprintf(kbps_text, 10, _("%dKbps"), 32);
    label = gtk_label_new (kbps_text);
    gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, label, "bitrate_lbl_2");
    /* END MP3 */


    /* OGG */
    rip_ogg = gtk_check_button_new_with_mnemonic (_("OGG Vorbis (lossy compression)"));
    g_signal_connect ((gpointer) rip_ogg, "toggled",
                      G_CALLBACK (on_rip_ogg_toggled), NULL);

    vbox2 = w_gtk_frame_vbox_new (NULL, 0, vbox, rip_ogg);

    hbox10 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add (GTK_CONTAINER (vbox2), hbox10);

    oggLbl = gtk_label_new (_("Quality"));
    gtk_box_pack_start (GTK_BOX (hbox10), oggLbl, FALSE, FALSE, 0);

    oggquality = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL,
                                GTK_ADJUSTMENT (gtk_adjustment_new (6, 0, 11, 1, 1, 1)));
    gtk_box_pack_start (GTK_BOX (hbox10), oggquality, TRUE, TRUE, 5);
    gtk_scale_set_value_pos (GTK_SCALE (oggquality), GTK_POS_RIGHT);
    gtk_scale_set_digits (GTK_SCALE (oggquality), 0);
    
    gtk_widget_set_tooltip_text (oggquality, _("Higher quality means bigger file. Default is 6."));
    /* END OGG */


    /* FDK-AAC  */
    rip_fdkaac = gtk_check_button_new_with_mnemonic (_("AAC (lossy compression)"));
    g_signal_connect ((gpointer) rip_fdkaac, "toggled",
                       G_CALLBACK (on_rip_fdkaac_toggled), NULL);

    vbox2X = w_gtk_frame_vbox_new (NULL, 0, vbox, rip_fdkaac);

    hbox9X = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox2X), hbox9X, TRUE, TRUE, 0);    

    label = gtk_label_new (_("Bitrate"));
    gtk_box_pack_start (GTK_BOX (hbox9X), label, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, label, "fdkaac_bitrate_lbl");
    
    fdkaac_bitrate = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL,
                                    GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 14, 1, 1, 1)));
    gtk_box_pack_start (GTK_BOX (hbox9X), fdkaac_bitrate, TRUE, TRUE, 5);
    gtk_scale_set_draw_value (GTK_SCALE (fdkaac_bitrate), FALSE);
    gtk_scale_set_digits (GTK_SCALE (fdkaac_bitrate), 0);
    g_signal_connect ((gpointer) fdkaac_bitrate, "value_changed",
                                        G_CALLBACK (on_fdkaac_bitrate_value_changed),
                                        NULL);
    
    gtk_widget_set_tooltip_text (fdkaac_bitrate, _("Higher bitrate is better quality but also bigger file. Most people use 192Kbps."));
    
    char kbps_textX[10];
    snprintf(kbps_textX, 10, _("%dKbps"), 32);
    label = gtk_label_new (kbps_textX);
    gtk_box_pack_start (GTK_BOX (hbox9X), label, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, label, "fdkaac_bitrate_lbl_2");    
    /* END FDK-AAC */

    /* OPUS */
    GtkWidget *opusLbl;
    GtkWidget *opusrate;
    GtkWidget *rip_opus;
    GtkWidget *opushbox;
    char opus_kbps[10];

    rip_opus = gtk_check_button_new_with_mnemonic (_("OPUS (lossy compression)"));
    g_signal_connect((gpointer) rip_opus, "toggled",
                     G_CALLBACK (on_rip_opus_toggled), NULL);

    vbox2 = w_gtk_frame_vbox_new (NULL, 0, vbox, rip_opus);

    opushbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_start (GTK_BOX (vbox2), opushbox, TRUE, TRUE, 0);

    opusLbl = gtk_label_new (_("Bitrate"));
    gtk_box_pack_start (GTK_BOX(opushbox), opusLbl, FALSE, FALSE,0);
    GLADE_HOOKUP_OBJECT (prefs, opusLbl, "opus_lbl");

    opusrate = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL,
                              GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 13, 1, 1, 1)));
    gtk_box_pack_start(GTK_BOX(opushbox), opusrate, TRUE, TRUE, 5);
    gtk_scale_set_draw_value (GTK_SCALE (opusrate), FALSE);
    gtk_scale_set_digits (GTK_SCALE (opusrate), 0);
    g_signal_connect ((gpointer) opusrate, "value_changed",
                                        G_CALLBACK (on_opusrate_value_changed),
                                        NULL);
    gtk_widget_set_tooltip_text (opusrate, _("Higher bitrate is better quality but also bigger file. Most people use 160Kbps."));
    GLADE_HOOKUP_OBJECT (prefs, opusrate, "opusrate");
    snprintf(opus_kbps, 10, _("%dKbps"), 32);
    label = gtk_label_new (kbps_text);
    gtk_box_pack_start (GTK_BOX (opushbox), label, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, label, "bitrate_lbl_4");
    GLADE_HOOKUP_OBJECT (prefs, rip_opus, "rip_opus");
    /* END OPUS */
   /* END ENCODE tab */

    /* ADVANCED tab */
    GtkWidget* do_cddb_updates;
    GtkWidget* hbox;
    GtkWidget* cddbServerName;
    GtkWidget* cddbPortNum;
    GtkWidget* useProxy;
    GtkWidget* serverName;
    GtkWidget* portNum;
    GtkWidget* frameVbox;
    GtkWidget* concatenated_track_separator;
    GtkWidget* do_log;
    GtkWidget* do_fast_rip;
    
    vbox = w_gtk_notebook_add_tab_box (notebook1, _("Advanced"));
    frameVbox = w_gtk_frame_vbox_new ("CDDB", 0, vbox, NULL);
    
    do_cddb_updates = gtk_check_button_new_with_mnemonic (_("Get disc info from the internet automatically"));
    gtk_box_pack_start (GTK_BOX (frameVbox), do_cddb_updates, FALSE, FALSE, 0);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (frameVbox), hbox, FALSE, FALSE, 1);
    
    label = gtk_label_new (_("Server: "));
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
    
    cddbServerName = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (hbox), cddbServerName, TRUE, TRUE, 5);
    GLADE_HOOKUP_OBJECT (prefs, cddbServerName, "cddb_server_name");
    
    gtk_widget_set_tooltip_text (cddbServerName, _("The CDDB server to get disc info from (default is gnudb.gnudb.org)"));
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (frameVbox), hbox, FALSE, FALSE, 1);
    
    label = gtk_label_new (_("Port: "));
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
    
    cddbPortNum = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (hbox), cddbPortNum, TRUE, TRUE, 5);
    GLADE_HOOKUP_OBJECT (prefs, cddbPortNum, "cddb_port_number");
    
    gtk_widget_set_tooltip_text (cddbPortNum, _("The CDDB server port (default is 8880)"));
    
    useProxy = gtk_check_button_new_with_mnemonic (_("Use an HTTP proxy to connect to the internet"));
    GLADE_HOOKUP_OBJECT (prefs, useProxy, "use_proxy");
    
    frameVbox = w_gtk_frame_vbox_new (NULL, 0, vbox, useProxy);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (frameVbox), hbox, FALSE, FALSE, 1);
    
    label = gtk_label_new (_("Server: "));
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
    
    serverName = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (hbox), serverName, TRUE, TRUE, 5);
    GLADE_HOOKUP_OBJECT (prefs, serverName, "server_name");
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (frameVbox), hbox, FALSE, FALSE, 1);
    
    label = gtk_label_new (_("Port: "));
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
    
    portNum = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (hbox), portNum, TRUE, TRUE, 5);
    GLADE_HOOKUP_OBJECT (prefs, portNum, "port_number");

    frameVbox = w_gtk_frame_vbox_new ("Right-click options", 0, vbox, NULL);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (frameVbox), hbox, FALSE, FALSE, 1);
    
    label = gtk_label_new (_("Artist/Title separator: "));
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
 
    concatenated_track_separator = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(concatenated_track_separator), 1);
    gtk_box_pack_start (GTK_BOX (hbox), concatenated_track_separator, TRUE, TRUE, 5);
    GLADE_HOOKUP_OBJECT (prefs, concatenated_track_separator,
            "concatenated_track_separator");
    
    do_log = gtk_check_button_new_with_label (_("Log to /var/log/asunder.log"));
    gtk_box_pack_start (GTK_BOX (vbox), do_log, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, do_log, "do_log");
    
    do_fast_rip = gtk_check_button_new_with_label (_("Faster ripping (no error correction)"));
    gtk_box_pack_start (GTK_BOX (vbox), do_fast_rip, FALSE, FALSE, 0);
    GLADE_HOOKUP_OBJECT (prefs, do_fast_rip, "do_fast_rip");
    
    hboxFill = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hboxFill, TRUE, TRUE, 0);
    /* END ADVANCED tab */

    cancelbutton1 = gtk_dialog_add_button (GTK_DIALOG(prefs), "gtk-cancel", GTK_RESPONSE_CANCEL);
    okbutton1 = gtk_dialog_add_button (GTK_DIALOG(prefs), "gtk-ok", GTK_RESPONSE_OK);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (prefs, prefs, "prefs");
    GLADE_HOOKUP_OBJECT (prefs, notebook1, "notebook1");
    GLADE_HOOKUP_OBJECT (prefs, music_dir, "music_dir");
    GLADE_HOOKUP_OBJECT (prefs, make_playlist, "make_playlist");
    GLADE_HOOKUP_OBJECT (prefs, cdrom, "cdrom");
    GLADE_HOOKUP_OBJECT (prefs, eject_on_done, "eject_on_done");
    GLADE_HOOKUP_OBJECT (prefs, format_music, "format_music");
    GLADE_HOOKUP_OBJECT (prefs, format_albumdir, "format_albumdir");
    GLADE_HOOKUP_OBJECT (prefs, format_playlist, "format_playlist");
    GLADE_HOOKUP_OBJECT (prefs, rip_wav, "rip_wav");
    GLADE_HOOKUP_OBJECT (prefs, mp3_vbr, "mp3_vbr");
    GLADE_HOOKUP_OBJECT (prefs, mp3bitrate, "mp3bitrate");
    GLADE_HOOKUP_OBJECT (prefs, rip_mp3, "rip_mp3");
    GLADE_HOOKUP_OBJECT (prefs, oggLbl, "ogg_lbl");
    GLADE_HOOKUP_OBJECT (prefs, oggquality, "oggquality");
    GLADE_HOOKUP_OBJECT (prefs, rip_ogg, "rip_ogg");
    GLADE_HOOKUP_OBJECT (prefs, rip_fdkaac, "rip_fdkaac");
    GLADE_HOOKUP_OBJECT (prefs, fdkaac_bitrate, "fdkaac_bitrate");
    GLADE_HOOKUP_OBJECT (prefs, flacLbl, "flac_lbl");
    GLADE_HOOKUP_OBJECT (prefs, flaccompression, "flaccompression");
    GLADE_HOOKUP_OBJECT (prefs, rip_flac, "rip_flac");
    GLADE_HOOKUP_OBJECT (prefs, do_cddb_updates, "do_cddb_updates");
    GLADE_HOOKUP_OBJECT (prefs, cancelbutton1, "cancelbutton1");
    GLADE_HOOKUP_OBJECT (prefs, okbutton1, "okbutton1");

    g_signal_connect ((gpointer) prefs, "response",
                      G_CALLBACK (on_prefs_response), NULL);

    g_signal_connect ((gpointer) prefs, "realize",
                      G_CALLBACK (on_prefs_show), NULL);

    gtk_widget_show_all (main_vbox);
    return prefs;
}


GtkWidget * create_ripping (void)
{
    GtkWidget *ripping;
    GtkWidget *main_vbox;
    GtkWidget *table3;
    GtkWidget *progress_total;
    GtkWidget *progress_rip;
    GtkWidget *progress_encode;
    GtkWidget *label25;
    GtkWidget *label26;
    GtkWidget *label27;
    GtkWidget *cancel;

    ripping = w_gtk_dialog_new (_("Ripping"),
                                GTK_WINDOW(win_main),
                                FALSE,
                                &main_vbox);

    table3 = w_gtk_grid_new (main_vbox, 5);

    // row 0
    label25 = gtk_label_new (_("Total progress"));
    gtk_grid_attach (GTK_GRID(table3), label25, 0, 0, 1, 1);
    gtk_widget_set_halign (label25, GTK_ALIGN_END);

    progress_total = gtk_progress_bar_new ();
    gtk_grid_attach (GTK_GRID(table3), progress_total, 1, 0, 1, 1);
    gtk_widget_set_hexpand (progress_total, TRUE);

    // row 1
    label26 = gtk_label_new (_("Ripping"));
    gtk_grid_attach (GTK_GRID(table3), label26, 0, 1, 1, 1);
    gtk_widget_set_halign (label26, GTK_ALIGN_END);

    progress_rip = gtk_progress_bar_new ();
    gtk_grid_attach (GTK_GRID(table3), progress_rip, 1, 1, 1, 1);

    // row 2
    label27 = gtk_label_new (_("Encoding"));
    gtk_grid_attach (GTK_GRID(table3), label27, 0, 2, 1, 1);
    gtk_widget_set_halign (label27, GTK_ALIGN_END);

    progress_encode = gtk_progress_bar_new ();
    gtk_grid_attach (GTK_GRID(table3), progress_encode, 1, 2, 1, 1);

    // cancel button
    cancel = gtk_dialog_add_button (GTK_DIALOG(ripping), "gtk-cancel", GTK_RESPONSE_CANCEL);
    g_signal_connect ((gpointer) cancel, "clicked",
                      G_CALLBACK (on_cancel_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (ripping, ripping, "ripping");
    GLADE_HOOKUP_OBJECT_NO_REF (ripping, main_vbox, "dialog_vbox2");
    GLADE_HOOKUP_OBJECT (ripping, table3, "table3");
    GLADE_HOOKUP_OBJECT (ripping, progress_total, "progress_total");
    GLADE_HOOKUP_OBJECT (ripping, progress_rip, "progress_rip");
    GLADE_HOOKUP_OBJECT (ripping, progress_encode, "progress_encode");
    GLADE_HOOKUP_OBJECT (ripping, label25, "label25");
    GLADE_HOOKUP_OBJECT (ripping, label26, "label26");
    GLADE_HOOKUP_OBJECT (ripping, label27, "label27");
    GLADE_HOOKUP_OBJECT (ripping, cancel, "cancel");

    gtk_widget_show_all (main_vbox);
    return ripping;
}

void disable_all_main_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_main, "lookup"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "preferences"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "about"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "disc"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_artist"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "artist_label"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "title_label"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_title"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "single_artist"), FALSE);
    gtk_widget_set_sensitive(tracklist, FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "rip_button"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_genre"), FALSE);	// lnr
    gtk_widget_set_sensitive(lookup_widget(win_main, "genre_label"), FALSE);	// lnr
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_year"), FALSE);
}

void enable_all_main_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_main, "lookup"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "preferences"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "about"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "disc"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_artist"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "artist_label"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "title_label"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_title"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "single_artist"), TRUE);
    gtk_widget_set_sensitive(tracklist, TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "rip_button"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_genre"), TRUE);		// lnr
    gtk_widget_set_sensitive(lookup_widget(win_main, "genre_label"), TRUE);		// lnr
    gtk_widget_set_sensitive(lookup_widget(win_main, "album_year"), TRUE);
}

void disable_mp3_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "mp3_vbr"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "bitrate_lbl"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "mp3bitrate"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "bitrate_lbl_2"), FALSE);
}

void enable_mp3_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "mp3_vbr"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "bitrate_lbl"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "mp3bitrate"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "bitrate_lbl_2"), TRUE);
}

void disable_fdkaac_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "fdkaac_bitrate_lbl"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "fdkaac_bitrate"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "fdkaac_bitrate_lbl_2"), FALSE);
}

void enable_fdkaac_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "fdkaac_bitrate_lbl"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "fdkaac_bitrate"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "fdkaac_bitrate_lbl_2"), TRUE);
}

void disable_ogg_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "ogg_lbl"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "oggquality"), FALSE);
}

void enable_ogg_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "ogg_lbl"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "oggquality"), TRUE);
}

void disable_opus_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "opus_lbl"),FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs,"opusrate"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "bitrate_lbl_4"), FALSE);
}

void enable_opus_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "opus_lbl"),TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs,"opusrate"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "bitrate_lbl_4"), TRUE);
}

void disable_flac_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "flac_lbl"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "flaccompression"), FALSE);
}

void enable_flac_widgets(void)
{
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "flac_lbl"), TRUE);
    gtk_widget_set_sensitive(lookup_widget(win_prefs, "flaccompression"), TRUE);
}

void toggle_allow_tracknum(void)
{
    // "Toggle Track Offset/Width control"
    GtkWidget* tn_hbox = lookup_widget(win_main, "tn_hbox");
    GtkWidget* tn_first = lookup_widget(win_main, "tn_first");
    GtkWidget* tn_combo_width = lookup_widget(win_main, "tn_width");
    gboolean active = global_prefs->allow_first_track_num_change;

    if (!active)
    {
        global_prefs->first_track_num_offset = 0;
        global_prefs->track_num_width = 2;
        gtk_widget_hide(tn_hbox);
    }
    else
    {
        char txt[16];
        snprintf(txt, 16, "%d", global_prefs->first_track_num_offset + 1);
        gtk_entry_set_text(GTK_ENTRY (tn_first), txt);
        gtk_combo_box_set_active(GTK_COMBO_BOX(tn_combo_width), global_prefs->track_num_width - 1);
        gtk_widget_show(tn_hbox);
    }
    update_track_offsets();
}

 
 
//===============================================================
// About

const char* GBLprogramName = "Asunder 2.9.7";

static const char* 
GBLauthors[2] = {
"Many thanks to all the following people:\n"
"\n"
"Andrew Smith\n"
"http://littlesvr.ca/contact.php\n"
"2005 - 2019\n"
"- maintainer\n"
"\n"
"Eric Lathrop\n"
"http://ericlathrop.com/\n"
"- original author\n"
"- 'eject when finished' feature\n"
"\n"
"Gregory Margo\n"
"- Shorten requested filenames that are longer than the filesystem can handle\n"
"- Work around cdparanoia's filename length limits\n"
"- Fixed bug where multiple CDDB entries would only show once\n"
"- Improved the refresh/lookup system to work much better with more complicated CDs\n"
"- Fixed FLAC progress scanner. Both to make the percentage more reliable and to prevent a possible crash\n"
"- Added feature to select/deselect all tracks by clicking on rip checkbox header\n"
"- Fixed autocomplete file loading\n"
"- Fixed several potential buffer overflow bugs\n"
"- Fixed some minor memory leaks\n"
"- Fixed race condition during abort\n"
"- Improved completion percentage feedback\n"
"- Added feature to make the artist and title columns resizable\n"
"- Remove 'single genre' code that's been unused since 2010.\n"
"- Added features to automatically fix some common mistakes in CDDB entries.\n"
"- Fixed the wavpack and mpc encoders to include APEv2 metadata.\n"
"- Allow changing the first track number and the width of the track number in the filename.\n"
"(A few patches were mistakenly attributed to \"Gregory Montego\" and \"Gergory Margo\")\n"
"\n"
"Andreas Ronnquist\n"
"- Moved the config file to ~/.config/asunder without breaking old configs\n"
"- Moved the autocomplete files to ~/.cache/asunder without breaking old caches\n"
"\n"
"Fraser Tweedale\n"
"- FreeBSD port\n"
"\n"
"Dave Tyson\n"
"- NetBSD port\n"
"\n"
"Lorraine Reed, aka Lightning Rose\n"
"http://www.lightning-rose.com/\n"
"- Editable genre feature to version 1.9.2\n"
"- Added invalid MS file chars to trim_chars()\n"
"\n"
"Radu Potop\n"
"http://wooptoo.com/\n"
"- The new Asunder icon\n"
"\n"
"Pader Reszo\n"
"- Made the about description translatable\n"
"\n"
"Cyril Brulebois\n"
"- A kFreeBSD fix.\n"
"\n"
"Richard Gill\n"
"- Support for XDG_CONFIG_HOME\n"
"- Support for XDG_CACHE_HOME\n"
"\n"
"Hiroyuki Ito\n"
"- Support for autocompletion in artist/album/genre fields.\n"
"\n"
"Jonathan 'theJPster' Pallant\n"
"- Move from neroAacEnc to fdkaac.\n"
"- Tag AAC files using neroAacTag.\n"
"\n"
"Micah Lindstrom\n"
"- Support for invalid filesystem characters in metadata.\n"
"\n"
"Tim Allen\n"
"- Fix to make the MusicBrainz CDDB gateway work with Asunder.\n"
"\n"
"Tudor\n"
"- Desktop file trick to add Asunder to the list of audio CD apps in Gnome.\n"
"\n"
"Felix Braun\n"
"- Support for encoding into OPUS.\n"
"\n"
"Nicolas Leveille\n"
"- Fix for: don't reset the track info edited by the user by doing an unnecessary CDDB lookup.\n"
"\n"
"Pierre Lestringant (whz)\n"
"- Fixed musepack and aac playlist generation\n"
"\n"
"Matthew (mw)\n"
"- Add the ALBUMARTIST tag to flac files when \"Single artist\" is not checked.\n"
"\n"
"Antony Gelberg\n"
"- Fixed when the CD is ejected to avoid problems before a full rip/encode cycle is completed.\n"
"\n"
"Tom Bailey\n"
"- Added support for joining artist and title in title field.\n"
"\n"
"Packages:\n"
"\n"
"Kevin \"Eonfge\" Degeling\n"
"- Flatpak/Flathub setup for Asunder, version 2.9.6\n"
"\n"
"Przemyslaw Buczkowski\n"
"- Fixed the CDDB tooltip after switch to Gnudb.\n"
"\n"
"Toni Graffy\n"
"Maintainer of many SuSE packages at PackMan\n"
"- SuSE packages of Asunder, versions 0.1 - 1.6\n"
"\n"
"Joao Pinto\n"
"- 64bit Debian and Ubuntu packages of Asunder, versions 1.0.1 - 1.0.2, 1.6\n"
"\n"
"Trent Weddington\n"
"http://rtfm.insomnia.org/~qg/\n"
"- Debian packages of Asunder, versions 0.8 - 1.0.1\n"
"\n"
"Daniel Baumann\n"
"- Debian maintainer for Asunder, version 1.6.2\n"
"\n"
"Marcin Zajaczkowski (Szpak)\n"
"http://timeoff.wsisiz.edu.pl/rpms.html\n"
"- Fedora packages of Asunder, versions 0.8.1 - 1.6\n"
"\n"
"Adam Williamson\n"
"http://www.happyassassin.net/\n"
"- Mandriva packages of Asunder, versions 0.9 - 1.6\n"
"\n"
"Tom Nardi\n"
"- Slackware packages of Asunder, versions 0.8.1 - 1.6\n"
"\n"
"vktgz\n"
"http://www.vktgz.homelinux.net/\n"
"- Gentoo ebuilds for Asunder, versions 0.8 - 0.8.1, 1.6\n"
"\n"
"Ronald van Haren\n"
"- Arch package of Asunder, versions 1.5 - 1.6\n"
"\n"
"Sebastien Piccand\n"
"- Arch packages of Asunder, versions 0.8.1 - 1.0.2\n"
"\n"
"Alexey Rusakov\n"
"- ALT packages of Asunder, versions 0.8.1 - 1.5\n"
"\n"
"Fraser Tweedale\n"
"- FreeBSD ports of Asunder, versions 0.8.1 - 0.9\n"
"\n"
"Philip Muller\n"
"- Paldo package of Asunder, version 0.8.1\n"
"\n"
"Christophe Lincoln\n"
"- Slitaz package of Asunder, version 1.0.2\n"
"\n"
"coolpup\n"
"- Puppy package of Asunder, version 1.6.2\n"
"\n"
,
NULL};

static const char*  GBLcomments = 
N_("An application to save tracks from an Audio CD \n"
"as WAV, MP3, OGG, FLAC, Opus and/or AAC files.");

static const char* GBLcopyright = 
"Copyright 2005 Eric Lathrop\n"
"Copyright 2007 - 2019 Andrew Smith";

static const char* GBLwebsite = "http://littlesvr.ca/asunder/";

static const char* GBLlicense = 
"Asunder is distributed under the GNU General Public Licence\n"
"version 2, please see COPYING file for the complete text\n";

void show_aboutbox (void)
{
    /* TRANSLATORS: Replace this string with your names, one name per line. */
    gchar * GBLtranslators = _("Translated by");

    gtk_show_about_dialog(GTK_WINDOW(lookup_widget(win_main, "main")), 
                          "name", GBLprogramName,
                          "program-name", GBLprogramName,
                          "authors", GBLauthors,
                          "translator-credits", GBLtranslators,
                          "comments", _(GBLcomments),
                          "copyright", GBLcopyright,
                          "license", GBLlicense,
                          "website", GBLwebsite,
                          NULL);
}

void show_completed_dialog(int numOk, int numFailed)
{
    GtkWidget* dialog;

    if (numFailed == 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(win_main),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_CLOSE,
                                        _("%d file(s) created successfully"),
                                        numOk);
    }
    else
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(win_main),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("There was an error creating %d file(s)"),
                                        numFailed);
    }
    
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}
