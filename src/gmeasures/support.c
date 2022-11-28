#include "common.h" 

#include <sys/types.h>
#include <sys/stat.h>
#include "support.h"

GtkWidget*
lookup_widget                          (GtkWidget       *widget,
                                        const gchar     *widget_name)
{
  GtkWidget *parent, *found_widget;

  for (;;)
    {
      if (GTK_IS_MENU (widget))
        parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
      else
        parent = gtk_widget_get_parent (widget);
      if (!parent)
        parent = (GtkWidget*) g_object_get_data (G_OBJECT (widget), "GladeParentKey");
      if (parent == NULL)
        break;
      widget = parent;
    }

  found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (widget),
                                                 widget_name);
  if (!found_widget)
    g_warning ("Widget not found: %s", widget_name);
  return found_widget;
}

static GList *pixmaps_directories = NULL;

/* Use this function to set the directory containing installed pixmaps. */
void add_pixmap_directory (const gchar *directory)
{
  pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));
}

/* This is an internally used function to find pixmap files. */
static gchar* find_pixmap_file (const gchar *filename)
{
  GList *elem;
  /* We step through each of the pixmaps directory to find it. */
  elem = pixmaps_directories;
  while (elem)
  {
      gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
                                         G_DIR_SEPARATOR_S, filename);
#if GLIB_CHECK_VERSION(2,0,0)
      if (g_file_test (pathname, G_FILE_TEST_EXISTS))
#else
      if (access (pathname, F_OK) != -1)
#endif
        return pathname;

      g_free (pathname);
      elem = elem->next;
  }
  return NULL;
}

