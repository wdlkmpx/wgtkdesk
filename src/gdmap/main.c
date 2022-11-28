/* Copyright (C) 2005-2006 sgop@users.sourceforge.net
 * This is free software distributed under the terms of the
 * GNU Public License.  See the file COPYING for details.
 */
 
#include "common.h"

#include "gui_main.h"
#include "gui_support.h"
#include "colors.h"
#include "preferences.h"

#define NL "\n"

static const char* Folder = NULL;

static GOptionEntry Options[] =
{
  { "folder", 'f', 0, G_OPTION_ARG_STRING, &Folder, "Inspect folder", "dir" },
/*   { "mount", 'm', 0, G_OPTION_ARG_NONE, &AllowLeaveDevice, "Descend directories on other filesystems", NULL }, */
/*   { "reportedsize", 'r', 0, G_OPTION_ARG_NONE, &ReportedSize, "Don't show disk size of files but reported size", NULL }, */
  { NULL, 0, 0, 0, NULL, NULL, NULL }
};

static gboolean on_load(G_GNUC_UNUSED gpointer unused) {
  if (Folder) gui_tree_load_and_display(Folder);
  return FALSE;
}


int main (int argc, char *argv[])
{
  w_gtk_check_version (GTK_MAJOR_VERSION);

  GOptionContext* context = g_option_context_new("- Graphical Disk Map");
  GError* error = NULL;

#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

  g_option_context_set_ignore_unknown_options(context, FALSE);
  g_option_context_set_help_enabled(context, TRUE);
  g_option_context_add_main_entries(context, Options, NULL);
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_warning("%s", error->message);
    g_option_context_free(context);
    return 0;
  }
  g_option_context_free(context);
  
  gtk_init(&argc, &argv);
  
  gui_create_main_win();

  colors_init();
  pref_init();

  g_idle_add(on_load, NULL);
  
  gtk_main();

  return 0;
}

