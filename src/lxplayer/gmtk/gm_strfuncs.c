/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gm_strfuncs.c
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 * 
 * gm_strfuncs.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gm_strfuncs.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 */

#include "gmlib.h"

gchar *gm_tempname(gchar * path, const gchar * name_template)
{
    gchar *result;
    gchar *replace;
    gchar *basename;
    gchar *localpath;
    const char *tmpdir_env = g_getenv ("TMPDIR");

    basename = g_strdup(name_template);

    if (!path && !tmpdir_env) {
        localpath = g_strdup ("/tmp");
    } else if (!path && tmpdir_env) {
        localpath = g_strdup (tmpdir_env);
    } else {
        localpath = g_strdup (path);
    }

    while ((replace = g_strrstr(basename, "X"))) {
        replace[0] = (gchar) g_random_int_range((gint) 'a', (gint) 'z');
    }

    result = g_strdup_printf("%s/%s", localpath, basename);
    g_free(basename);
    g_free(localpath);

    return result;
}


gchar *gm_get_path(gchar * uri)
{
    gchar *path = NULL;
    gchar cwd[1024];
    gchar *tmp = NULL;

    if (g_strrstr(uri, "/") != NULL) {
        path = g_strdup(uri);
        tmp = g_strrstr(path, "/");
        tmp[0] = '\0';
    } else {
        if (getcwd(cwd, 1024) != NULL) {
            path = g_strdup(cwd);
        }
    }

    return path;
}

//============

void gm_str_replace_amp(gchar * data)
{
    gchar *pos;
    pos = g_strrstr(data, "&");
    while (pos) {
        pos[0] = '\x01';
        pos = g_strrstr(data, "&");
    }
}


void gm_str_unreplace_amp(gchar * data)
{
    gchar *pos;

    pos = g_strrstr(data, "\x01");
    while (pos) {
        pos[0] = '&';
        pos = g_strrstr(data, "\x01");
    }
}


void gm_str_strip_unicode(gchar * data, gsize len)
{
    gsize i = 0;
    if (data != NULL) {
        for (i = 0; i < len; i++) {
            if (!g_unichar_validate(data[i])) {
                data[i] = ' ';
            }
        }
    }
}

gchar **g_strv_new(guint size)
{
    gchar **strv = g_new0(gchar *, size + 1);
    strv[size] = NULL;
    return strv;
}


gchar *gm_seconds_to_string(gfloat seconds)
{
    guint hour = 0, min = 0, sec = 0;
    gchar *result = NULL;

    if (seconds >= 3600) {
        hour = seconds / 3600;
        seconds = seconds - (hour * 3600);
    }
    if (seconds >= 60) {
        min = seconds / 60;
        seconds = seconds - (min * 60);
    }
    sec = seconds;

    if (hour == 0) {
        result = g_strdup_printf("%2i:%02i", min, sec);
    } else {
        result = g_strdup_printf("%i:%02i:%02i", hour, min, sec);
    }
    return result;
}
