/*
 * gm_log.c
 * Copyright (C) Kevin DeKorte 2012 <kdekorte@gmail.com> and Hans Ecke
 * 
 * gm_log.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gm_log.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 */

#include "gmlib.h"

/* How to control the GLib logging system: with environment variables

G_MESSAGES_PREFIXED.  A list of log levels for which messages should be prefixed by the
program name and PID of the application. The default is to prefix everything except
G_LOG_LEVEL_MESSAGE and G_LOG_LEVEL_INFO. The possible values are error, warning, critical,
message, info and debug. You can also use the special values all and help.

G_MESSAGES_PREFIXED="" disables all prefixing

G_MESSAGES_DEBUG. Since glib 2.31
A space-separated list of log domains for which informational and
debug messages should be printed. By default, these messages are not printed.

G_MESSAGES_DEBUG=GMLIB enables a lot of output from gmlib

*/

#include <string.h>

#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN ((gchar*) "GMLIB")

static void fixup_loglevel(gboolean force_info_to_message, GLogLevelFlags * log_level)
{
    // by default, all messages G_LOG_LEVEL_MESSAGE or above are shown
    // if our own debug flag is set, force G_LOG_LEVEL_INFO messages to MESSAGE
    //
    // GLib loglevels are bitmasks, so we need to do bitmask operations here
    if (force_info_to_message && ((*log_level) & G_LOG_LEVEL_INFO)) {
        (*log_level) &= ~G_LOG_LEVEL_INFO;
        (*log_level) |= G_LOG_LEVEL_MESSAGE;
    }
}

static GHashTable *ptr2str = NULL;

static gboolean key_equal_func(gconstpointer a, gconstpointer b)
{
    if (a == b) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
   This function operates on static data structures, thus it needs to be
   locked by the ptr2strmutex mutex.
   
   It returns a string representation of the current thread number.
   Unfortunately, in GLib you can only access g_thread_self(), which is a
   pointer. Using printf("%p", g_thread_self()) is certainly possible, but
   difficult to understand at a glance, and hard to compare traces from
   different runs.
*/
static const gchar *threadid_core(gchar const *name)
{
    void *key;
    void *value;

    if (ptr2str == NULL) {
        ptr2str = g_hash_table_new(g_direct_hash, key_equal_func);
    }
    key = g_thread_self();
    value = g_hash_table_lookup(ptr2str, key);
    if (value == NULL) {
        if (name == NULL || name[0] == '\0') {
            name = "th";
        }
        value = g_strdup_printf("[%s%u] ", name, g_hash_table_size(ptr2str));
        g_hash_table_insert(ptr2str, key, value);
    }
    return value;
}

/*
 If GM_DEBUG_THREADS is set, return a string representation of
 the current thread number. If it is not set, return the empty string.
*/
static const gchar *threadid()
{
    const gchar *str;
    if (!g_getenv("GM_DEBUG_THREADS")) {
        return "";
    }
    g_mutex_lock(&ptr2strmutex);
    str = threadid_core(NULL);
    g_mutex_unlock(&ptr2strmutex);
    return str;
}

// this function will only change the thread name if it is the first gm_log* function called within that thread
// there are pros and cons for this behavior, but consistent thread names (that don't change because somebody calls
// this function) are certainly a "pro".
void gm_log_name_this_thread(gchar const *const name)
{
    if (!g_getenv("GM_DEBUG_THREADS")) {
        return;
    }
    g_mutex_lock(&ptr2strmutex);
    (void) threadid_core(name);
    g_mutex_unlock(&ptr2strmutex);
}

// Note that the format should not have a trailing \n - the glib logging system adds it
void gm_logv(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * format, va_list args)
{
    gchar *f = NULL;
    fixup_loglevel(force_info_to_message, &log_level);
    f = g_strdup_printf("%s%s", threadid(), format);
    g_logv(G_LOG_DOMAIN, log_level, f, args);
    g_free(f);

    return;
}

void gm_log(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * format, ...)
{
    va_list args;
    va_start(args, format);
    gm_logv(force_info_to_message, log_level, format, args);
    va_end(args);
}

void gm_logs(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * msg)
{
    size_t len;
    gchar *msg_nonl = NULL;

    fixup_loglevel(force_info_to_message, &log_level);

    // this function should be called if there might be a newline
    // at the end of the string. it will only allocate a copy
    // if necessary.
    len = strlen(msg);

    if (msg[len - 1] != '\n') {
        g_log(G_LOG_DOMAIN, log_level, "%s%s", threadid(), msg);
        return;
    }

    msg_nonl = g_strdup(msg);
    msg_nonl[len - 1] = '\0';
    g_log(G_LOG_DOMAIN, log_level, "%s%s", threadid(), msg_nonl);
    g_free(msg_nonl);
}

void gm_logsp(gboolean force_info_to_message, GLogLevelFlags log_level, const gchar * prefix, const gchar * msg)
{
    gchar **lines = NULL;
    gchar *nl_loc;
    gint linei;

    fixup_loglevel(force_info_to_message, &log_level);
    nl_loc = g_strrstr(msg, "\n");

    if (nl_loc == NULL) {
        g_log(G_LOG_DOMAIN, log_level, "%s%s %s", threadid(), prefix, msg);
        return;
    }
    // slow path
    // there is a \n somewhere, so we have to do an allocation either way
    lines = g_strsplit(msg, "\n", 0);
    linei = 0;
    while (lines[linei] != NULL) {
        g_strchomp(lines[linei]);
        if (lines[linei][0] != '\0') {
            g_log(G_LOG_DOMAIN, log_level, "%s%s %s", threadid(), prefix, lines[linei]);
        }
        linei++;
    }

    g_strfreev(lines);
    lines = NULL;

}
