/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gmlib.h
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 * 
 * gmlib.h is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gmlib.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 */
 
#ifndef __GMLIB_H__
#define __GMLIB_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gi18n.h>
#include "w_gtk.h"

#if GTK_MAJOR_VERSION == 3
#include <gtk/gtkx.h> // otherwise: undefined reference to `GTK_SOCKET' / gtk_socket_new, etc
#endif

//========================================================
// #include "gm_audio.h"

typedef struct _AudioDevice {
    gchar *alsa_mixer; // this is not looked up
    gdouble volume;
    gboolean muted;
    gchar *mplayer_ao;
} AudioDevice;

//========================================================
//#include "gm_log.h"

void gm_logv(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * format, va_list args);
__attribute__ ((format(printf, 3, 4))) void gm_log(gboolean force_info_to_message, GLogLevelFlags log_level,
                                                       const gchar * format, ...);
void gm_logs(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * msg);
void gm_logsp(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * prefix, const gchar * msg);
void gm_log_name_this_thread(gchar const *const name);
GMutex ptr2strmutex;

//========================================================
//#include "gm_pref_store.h"

typedef struct _GmPrefStore GmPrefStore;

GmPrefStore *gm_pref_store_new(const gchar * context);
void gm_pref_store_free(GmPrefStore * store);

gboolean gm_pref_store_get_boolean(GmPrefStore * store, const gchar * key);
gboolean gm_pref_store_get_boolean_with_default(GmPrefStore * store, const gchar * key, gboolean default_value);
void gm_pref_store_set_boolean(GmPrefStore * store, const gchar * key, gboolean value);

gint gm_pref_store_get_int(GmPrefStore * store, const gchar * key);
gint gm_pref_store_get_int_with_default(GmPrefStore * store, const gchar * key, gint default_value);
void gm_pref_store_set_int(GmPrefStore * store, const gchar * key, gint value);

gfloat gm_pref_store_get_float(GmPrefStore * store, const gchar * key);
gfloat gm_pref_store_get_float_with_default(GmPrefStore * store, const gchar * key, gfloat default_value);
void gm_pref_store_set_float(GmPrefStore * store, const gchar * key, gfloat value);

gchar *gm_pref_store_get_string(GmPrefStore * store, const gchar * key);
char *gm_pref_store_get_string_with_default (GmPrefStore * store, const char * key, char *default_value);
void gm_pref_store_set_string(GmPrefStore * store, const gchar * key, gchar * value);

void gm_pref_store_unset(GmPrefStore * store, const gchar * key);

//========================================================
//#include "gm_strfuncs.h"

#ifndef gm_bool_to_string
#define gm_bool_to_string(x) ((x) ? "true" : "false")
#endif
gchar *gm_tempname(gchar * path, const gchar * name_template);
gchar *gm_get_path(gchar * uri);
void gm_str_strip_unicode(gchar * data, gsize len);
void gm_str_replace_amp(gchar * data);
void gm_str_unreplace_amp(gchar * data);
gchar **g_strv_new(guint size);
gchar *gm_seconds_to_string(gfloat seconds);

#ifdef __cplusplus
}
#endif

#endif /* __GMLIB_H__ */

