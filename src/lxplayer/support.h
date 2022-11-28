/*
 * support.h
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 * 
 * support.h is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * support.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 */

#ifndef _SUPPORT_H
#define _SUPPORT_H

#include "common.h"

gint get_bitrate(gchar * name);
void free_metadata(MetaData * data);
MetaData *get_metadata(gchar * uri);
MetaData *get_basic_metadata(gchar * uri);
void strip_unicode(gchar * data, gsize len);
gint play_iter(GtkTreeIter * playiter, gint restart_second);
gboolean detect_playlist(gchar * filename);
gchar *metadata_to_utf8(gchar * string);
gboolean parse_playlist(gchar * uri);
gboolean parse_asx(gchar * uri);
gboolean parse_basic(gchar * filename);
gboolean parse_ram(gchar * filename);
gboolean parse_cdda(gchar * filename);
gboolean parse_dvd(gchar * filename);
gboolean parse_vcd(gchar * filename);
gboolean save_playlist_pls(gchar * filename);
gboolean save_playlist_m3u(gchar * filename);
gchar *get_path(gchar * filename);
gboolean streaming_media(gchar * filename);
gboolean device_name(gchar * filename);
gboolean add_item_to_playlist(const gchar * uri, gboolean playlist);
GtkTreeIter *find_iter_by_uri(const gchar * uri);
gboolean is_first_item_in_playlist(GtkTreeIter * iter);
gboolean first_item_in_playlist(GtkListStore * playliststore, GtkTreeIter * iter);
gboolean prev_item_in_playlist(GtkTreeIter * iter);
gboolean next_item_in_playlist(GtkTreeIter * iter);
void randomize_playlist(GtkListStore * store);
void reset_playlist_order(GtkListStore * store);
gchar *seconds_to_string(gfloat seconds);

gchar *get_localfile_from_uri(gchar * uri);
gboolean is_uri_dir(gchar * uri);
gboolean uri_exists(gchar * uri);
gchar *switch_protocol(const gchar * uri, gchar * new_protocol);

gboolean map_af_export_file(gpointer data);
gboolean unmap_af_export_file(gpointer data);

gboolean add_to_playlist_and_play(gpointer data);
gboolean clear_playlist_and_play(gpointer data);

#endif                          // _SUPPORT_H
