/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gui.h
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 *
 * gui.h is free software.
 *
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * gui.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

#include "gmtk.h"

#include <math.h>

GtkWidget *window;

extern GtkMenuItem *menuitem_edit_random;
extern GtkMenuItem *menuitem_edit_loop;
extern GtkMenuItem *menuitem_edit_play_single;
extern GtkMenuItem *menuitem_edit_select_audio_lang;
extern GtkMenuItem *menuitem_edit_select_sub_lang;

GtkWidget *repeat;
GtkWidget *shuffle;
GtkWidget *tracks;
GtkWidget *subtitles;

GtkMenuItem *menuitem_view_info;
GtkMenuItem *menuitem_view_playlist;

GtkMenuItem *menuitem_prev;
GtkMenuItem *menuitem_next;

GtkWidget *pane;
GtkWidget *hbox;

GtkWidget *media;
GtkWidget *cover_art;
GtkWidget *audio_meter;

GtkWidget *details_table;

GtkWidget *play_event_box;

GtkWidget *prev_event_box;
GtkWidget *next_event_box;
GtkWidget *menu_event_box;
GtkWidget *fs_event_box;

GmtkMediaTracker *tracker;
GtkWidget *vol_slider;

// Playlist container
GtkWidget *plvbox;
GSList *lang_group;
GSList *audio_group;

GtkAccelGroup *accel_group;

gboolean popup_handler(GtkWidget * widget, GdkEvent * event, void *data);
gboolean delete_callback(GtkWidget * widget, GdkEvent * event, void *data);
void config_close(GtkWidget * widget, void *data);

gboolean rew_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean play_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean pause_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean stop_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean ff_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean prev_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean next_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
void vol_slider_callback(GtkRange * range, gpointer user_data);
gboolean fs_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean make_panel_and_mouse_visible(gpointer data);
void menuitem_open_callback(GtkMenuItem * menuitem, void *data);
void menuitem_quit_callback(GtkMenuItem * menuitem, void *data);
void menuitem_about_callback(GtkMenuItem * menuitem, void *data);
void menuitem_play_callback(GtkMenuItem * menuitem, void *data);
void menuitem_pause_callback(GtkMenuItem * menuitem, void *data);
void menuitem_stop_callback(GtkMenuItem * menuitem, void *data);
void menuitem_fs_callback(GtkMenuItem * menuitem, void *data);
void menuitem_showcontrols_callback(GtkCheckMenuItem * menuitem, void *data);
void menuitem_quit_callback(GtkMenuItem * menuitem, void *data);
void menuitem_details_callback(GtkMenuItem * menuitem, void *data);
void menuitem_view_decrease_subtitle_delay_callback(GtkMenuItem * menuitem, void *data);
void menuitem_view_increase_subtitle_delay_callback(GtkMenuItem * menuitem, void *data);
void clear_playlist(GtkWidget * widget, void *data);
void create_playlist_widget();
void add_folder_to_playlist(GtkWidget * widget, void *data);
gboolean playlist_drop_callback(GtkWidget * widget, GdkDragContext * dc,
                                gint x, gint y, GtkSelectionData * selection_data, guint info, guint t, gpointer data);
void create_folder_progress_window();
void destroy_folder_progress_window();
void update_status_icon();
void setup_accelerators();
void set_software_volume(gdouble vol);
gboolean set_adjust_layout(gpointer data);
gboolean get_key_and_modifier(gchar * keyval, guint * key, GdkModifierType * modifier);
gboolean accel_key_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer data);
void assign_default_keys();
void reset_keys_callback(GtkButton * button, gpointer data);
gint get_index_from_key_and_modifier(guint key, GdkModifierType modifier);

GMutex fs_controls_lock;
void show_fs_controls();
void hide_fs_controls();
gboolean set_destroy(gpointer data);

// ==============================================================
// playlist.h
GtkWidget *plclose;
gint window_width, window_height;

void update_gui();
void menuitem_view_playlist_callback(GtkMenuItem * menuitem, void *data);
void add_item_to_playlist_callback(gpointer data, gpointer user_data);
void add_folder_to_playlist_callback(gpointer data, gpointer user_data);
