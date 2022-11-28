/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */

#include "common.h"

#include <math.h>
#include "gui_main.h"
#include "gui_support.h"
#include "tree.h"
#include "colors.h"
#include "utils.h"
#include "preferences.h"

typedef struct _tree_info_t tree_info_t;

struct _tree_info_t
{
    tree_t* node;
    double s[2][2];
    int geo[2][2];
    GList* children;
    tree_info_t* parent;
};

typedef struct
{
    GtkProgressBar* bar;
    GtkButton* abort;
    gboolean cancel;
} progress_data_t;

static GtkWidget* MainWin = NULL;

static cairo_surface_t* Surface = NULL;
static int Width = 0;
static int Height = 0;
static tree_info_t* Mark1 = NULL;
static tree_info_t* Mark2 = NULL;
static int RedrawTimer = 0;

static GtkWidget* Area = NULL;
static tree_info_t* CurrentItem = NULL;

static GtkLabel* FileLabel = NULL;
static GtkLabel* FileSizeLabel = NULL;
static GtkLabel* SubLabel = NULL;
static GtkLabel* SubSizeLabel = NULL;
static GtkLabel* SizeLabel = NULL;
static GtkBox* PathBox = NULL;
static GtkBox* StatusBar = NULL;

static GList* History = NULL;
static GList* HistoryPos = NULL;
static gboolean Loading = FALSE;

static GtkWidget *toolbar          = NULL;
static GtkWidget *btn_toolbar_open     = NULL;
static GtkWidget *btn_toolbar_back     = NULL;
static GtkWidget *btn_toolbar_forward  = NULL;
static GtkWidget *btn_toolbar_up       = NULL;
static GtkWidget *btn_toolbar_top      = NULL;
static GtkWidget *btn_toolbar_refresh  = NULL;
static GtkWidget *btn_toolbar_settings = NULL;

static gboolean LUseColors = TRUE;
static gboolean LUseAverage = TRUE;
static unsigned LMaxDepth = 0;
static unsigned LDisplayMode = DISPLAY_SQUARE_CUSHION;

static void on_action_open();
static void on_action_back();
static void on_action_forward();
static void on_action_up();
static void on_action_top();
static void on_action_exit();
static void on_action_preferences();
static void on_action_about();
static void on_action_refresh();

#include "gui_main-menu.h"

static void gui_tree_display(tree_info_t* info, gboolean destroy_old, gboolean new_history);
static tree_t* gui_tree_load(const char* folder, unsigned depth);


// HISTORY
static void history_clear(void)
{
    g_list_free(History);
    History = NULL;
    HistoryPos = NULL;
}

static void history_back(void)
{
    if (!HistoryPos) return;
    if (!HistoryPos->prev) return;
    
    HistoryPos = HistoryPos->prev;
    
    gui_tree_display(HistoryPos->data, FALSE, FALSE);
}

static void history_forward(void)
{
    if (!History) return;
    if (HistoryPos)
    {
        if (!HistoryPos->next) return;
        HistoryPos = HistoryPos->next;
    }
    else
    {
        HistoryPos = History;
    }
    gui_tree_display(HistoryPos->data, FALSE, FALSE);
}

static void history_append(tree_info_t* info)
{
    if (HistoryPos)
    {
        while (HistoryPos->next)
            History = g_list_delete_link(History, HistoryPos->next);
    }
    History = g_list_append(History, info);
    HistoryPos = g_list_last(History);
}



// TREE INFO
static void tree_info_destroy(tree_info_t* info)
{
    g_list_free_full(info->children, (GDestroyNotify)tree_info_destroy);
    g_free(info);
}

static tree_info_t* tree_info_search(tree_info_t* info, int x, int y)
{
    GList* dlist;
    unsigned depth = LMaxDepth;
    
    if (depth != 0 && depth < info->node->depth) return NULL;
    
    if (x < info->geo[0][0] || x >= info->geo[0][1] ||
        y < info->geo[1][0] || y >= info->geo[1][1])
        return NULL;

    for (dlist = info->children; dlist; dlist = dlist->next)
    {
        tree_info_t* sinfo = dlist->data;
        sinfo = tree_info_search(sinfo, x, y);
        if (sinfo) return sinfo;
    }
    return info;
}

static tree_info_t* tree_info_find_path(tree_info_t* root, tree_info_t* child)
{
    tree_info_t* temp;
    
    for (temp = child; temp; temp = temp->parent)
    {
        if (temp->parent == root) return temp;
    }
    return NULL;
}


static tree_info_t* tree_info_create(tree_t* tree)
{
    tree_info_t* info = g_malloc0(sizeof(*info));
    GList* dlist;
    
    info->node = tree;
    for (dlist = tree->entries; dlist; dlist = dlist->next)
    {
        tree_t* child = dlist->data;
        tree_info_t* sub = tree_info_create(child);
        sub->parent = info;
        info->children = g_list_append(info->children, sub);
    }
    return info;
}

static void on_action_exit(void)
{
    gtk_main_quit();
}

static void on_action_preferences(void)
{
    gui_show_preferences(GTK_WINDOW(MainWin));
}

static void on_action_about(void)
{
    gui_show_about(GTK_WINDOW(MainWin));
}

static gint comp_func(gconstpointer p1, gconstpointer p2)
{
    const tree_t* t1 = (tree_t*)p1;
    const tree_t* t2 = (tree_t*)p2;
  
    if (t1->size < t2->size) return 1;
    if (t1->size > t2->size) return -1;
    return 0;
}

static gint comp_func2(gconstpointer p1, gconstpointer p2)
{
    const tree_info_t* t1 = (tree_info_t*)p1;
    const tree_info_t* t2 = (tree_info_t*)p2;
  
    if (t1->node->size < t2->node->size) return 1;
    if (t1->node->size > t2->node->size) return -1;
    return 0;
}

static void gui_refresh_current(void)
{
    char* folder;
    tree_t* new_tree;

    if (!CurrentItem) return;
    folder = tree_full_name(CurrentItem->node);
    new_tree = gui_tree_load(folder, CurrentItem->node->depth);
    if (new_tree)
    {
        tree_t* tree = CurrentItem->node;
        tree_t* parent = tree->parent;
        tree_info_t* info = tree_info_create(new_tree);
        char* t;

        t = new_tree->name;
        new_tree->name = tree->name;
        tree->name = t;

        if (parent)
        {
            tree_t* temp;

            // remove tree from parent
            parent->entries = g_list_remove(parent->entries, tree);
            CurrentItem->parent->children =
                g_list_remove(CurrentItem->parent->children, CurrentItem);

            // adjust sizes
            for (temp = parent; temp; temp = temp->parent)
            {
                temp->size -= tree->size;
                temp->size += new_tree->size;
            }
            // insert tree in parent
            parent->entries = g_list_insert_sorted(parent->entries, new_tree, comp_func);
            new_tree->parent = parent;

            CurrentItem->parent->children =
                g_list_insert_sorted(CurrentItem->parent->children, info, comp_func2);
            info->parent = CurrentItem->parent;
        }
        tree_destroy(tree);
        tree_info_destroy(CurrentItem);
        CurrentItem = NULL;
        gui_tree_display(info, TRUE, TRUE);
    }
    g_free(folder);
}

static void on_action_refresh(void)
{
    gui_refresh_current();
}

static gboolean _update_progress(void* data)
{
    progress_data_t* pdata = data;

    if (pdata->cancel) return FALSE;

    gtk_progress_bar_pulse(pdata->bar);
    while (gtk_events_pending()) if (gtk_main_iteration()) return FALSE;

    return TRUE;
}

static void on_abort_load(progress_data_t* data)
{
    data->cancel = TRUE;
}

static tree_t* gui_tree_load(const char* folder, unsigned depth)
{
    GtkWidget* item1;
    GtkWidget* item2;
    tree_t* tree;
    progress_data_t* data = g_malloc0(sizeof(*data));

    data->cancel = FALSE;
    Loading = TRUE;

    gtk_widget_set_sensitive (btn_toolbar_open, FALSE);
    gtk_widget_set_sensitive (btn_toolbar_refresh, FALSE);
    w_gtk_menuitem_set_enabled (menuitem_file_open, FALSE);
    w_gtk_menuitem_set_enabled (menuitem_view_refresh, FALSE);

    item1 = gtk_progress_bar_new();
    data->bar = GTK_PROGRESS_BAR(item1);
    gtk_widget_show(item1);
    gtk_box_pack_start(StatusBar, item1, FALSE, FALSE, 0);
    gtk_progress_bar_set_pulse_step(data->bar, 0.05);
    gtk_progress_bar_set_text(data->bar, _("scanning..."));

    item2 = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    data->abort = GTK_BUTTON(item2);
    gtk_widget_show(item2);
    gtk_box_pack_start(StatusBar, item2, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(item2), "clicked",
                             G_CALLBACK(on_abort_load), data);

    tree = tree_load(folder, _update_progress, data, depth);
  
    gtk_widget_destroy(item1);
    gtk_widget_destroy(item2);
    g_free(data);

    gtk_widget_set_sensitive (btn_toolbar_open, TRUE);
    gtk_widget_set_sensitive (btn_toolbar_refresh, TRUE);
    w_gtk_menuitem_set_enabled (menuitem_file_open, TRUE);
    w_gtk_menuitem_set_enabled (menuitem_view_refresh, TRUE);

    Loading = FALSE;

    return tree;
}

void gui_tree_load_and_display(const char* folder)
{
    tree_t* tree = gui_tree_load(folder, 0);
    if (tree)
    {
        tree_info_t* info = tree_info_create(tree);
        gui_tree_display(info, TRUE, TRUE);
    }
}


static void on_action_open(void)
{
    GtkWidget* dialog;
    char* res;

    dialog = gtk_file_chooser_dialog_new
        (_("Choose folder"), GTK_WINDOW(MainWin),
         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        res = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
    }
    else
    {
        res = NULL;
    }
    gtk_widget_destroy(dialog);

    if (res)
    {
        gui_tree_load_and_display(res);
        g_free(res);
    }
}

static void gui_buttons_update(void)
{
    gtk_widget_set_sensitive (btn_toolbar_back,    HistoryPos && HistoryPos->prev);
    gtk_widget_set_sensitive (btn_toolbar_forward, HistoryPos && HistoryPos->next);
    gtk_widget_set_sensitive (btn_toolbar_up,  CurrentItem && CurrentItem->parent);
    gtk_widget_set_sensitive (btn_toolbar_top, CurrentItem && CurrentItem->parent);
    gtk_widget_set_sensitive (btn_toolbar_refresh, CurrentItem != NULL && !Loading);

    w_gtk_menuitem_set_enabled (menuitem_view_back,    HistoryPos && HistoryPos->prev);
    w_gtk_menuitem_set_enabled (menuitem_view_forward, HistoryPos && HistoryPos->next);
    w_gtk_menuitem_set_enabled (menuitem_view_up,  CurrentItem && CurrentItem->parent);
    w_gtk_menuitem_set_enabled (menuitem_view_top, CurrentItem && CurrentItem->parent);
    w_gtk_menuitem_set_enabled (menuitem_view_refresh, CurrentItem != NULL && !Loading);
}

static void on_action_top(void)
{
    tree_info_t* info = CurrentItem;

    if (!info || !info->parent) return;
    while (info->parent) info = info->parent;
    gui_tree_display(info, FALSE, TRUE);
}

static void on_action_up(void)
{
    tree_info_t* info = CurrentItem;

    if (!info || !info->parent) return;
    gui_tree_display(info->parent, FALSE, TRUE);
}

static void on_action_back(void)
{
    history_back();
}

static void on_action_forward(void)
{
    history_forward();
}



/* static void on_depth_change(GtkSpinButton* button) { */
/*   int val = gtk_spin_button_get_value_as_int(button); */
/*   if (val != MaxDepth) { */
/*     MaxDepth = val; */
/*     gui_tree_display(CurrentTree); */
/*   } */
/* } */

#if GTK_CHECK_VERSION(3,0,0)
static gboolean on_pathbox_scroll(GtkWidget* scroll, GdkEventScroll event, G_GNUC_UNUSED gpointer user_data)
{
    if (event.state || event.direction == GDK_SCROLL_LEFT || event.direction == GDK_SCROLL_RIGHT ||  event.delta_x)
        return FALSE;

    GtkAdjustment* adj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    // See https://gitlab.gnome.org/GNOME/gtk/blob/gtk-3-24/gtk/gtkscrolledwindow.c
#ifdef GDK_WINDOWING_QUARTZ
    gdouble unit = 1;
#else
    gdouble page = gtk_adjustment_get_page_size(adj);
    gdouble unit = fmin(page / 2, pow(page, 2.0 / 3));
#endif
    gdouble value = gtk_adjustment_get_value(adj);
    double delta_y;
    switch (event.direction) {
        case GDK_SCROLL_SMOOTH:
            delta_y = event.delta_y * unit;
            break;
        case GDK_SCROLL_UP:
            delta_y = -unit;
            break;
        case GDK_SCROLL_DOWN:
            delta_y = unit;
            break;
        default: // Unreachable
            delta_y = 0;
    }
    value += delta_y;
    gdouble lower = gtk_adjustment_get_lower(adj);
    gdouble upper = gtk_adjustment_get_upper(adj);
    value = fmin(fmax(value, lower), upper);
    gtk_adjustment_set_value(adj, value);
    return TRUE;
}
#endif

static GtkWidget* gui_create_path_box(void)
{
    GtkWidget* scroll;
    GtkWidget* hbox;
    GtkWidget* hbox2;
    GtkWidget* label;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(hbox);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scroll);
    gtk_container_add(GTK_CONTAINER(hbox), scroll);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scroll), TRUE);
    // We use "external" (scrollable but the scrollbar is never shown) because
    // otherwise the widget would expand vertically to make room for the
    // scrollbar
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_EXTERNAL, GTK_POLICY_NEVER);
#if GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_hexpand(scroll, TRUE);
    g_signal_connect(scroll, "scroll-event", G_CALLBACK(on_pathbox_scroll), NULL);
#endif

    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    PathBox = GTK_BOX(hbox2);
    gtk_widget_show(hbox2);
    gtk_container_add(GTK_CONTAINER(scroll), hbox2);

    label = gtk_label_new("");
    gtk_widget_show(label);
    gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, 5);
    SizeLabel = GTK_LABEL(label);

    return hbox;
}

static gboolean on_drawingarea_configure(GtkWidget* area)
{
    if (Surface) cairo_surface_destroy(Surface);
    Width = gtk_widget_get_allocated_width (area);
    Height = gtk_widget_get_allocated_height (area);

    Surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, Width, Height);
    if (CurrentItem) gui_tree_display(CurrentItem, FALSE, FALSE);

    return TRUE;
}

static void gui_show_pix(G_GNUC_UNUSED GtkWidget* area, cairo_t* cr)
{
    if (Surface)
    {
        cairo_set_source_surface(cr, Surface, 0, 0);
        cairo_paint(cr);
    }

    cairo_set_line_width(cr, 1);
    const GdkColor* color;

    if (Mark1) {
        color = color_get_by_type(COLOR_MARK1);
        cairo_set_source_rgb(cr, color->red / 255.0, color->green / 255.0, color->blue / 255.0);
        cairo_rectangle(cr,
            Mark1->geo[0][0], Mark1->geo[1][0],
            Mark1->geo[0][1]-Mark1->geo[0][0]-1, Mark1->geo[1][1]-Mark1->geo[1][0]-1);
        cairo_stroke(cr);
    }
    if (Mark2) {
        color = color_get_by_type(COLOR_MARK2);
        cairo_set_source_rgb(cr, color->red / 255.0, color->green / 255.0, color->blue / 255.0);
        cairo_rectangle(cr,
            Mark2->geo[0][0], Mark2->geo[1][0],
            Mark2->geo[0][1]-Mark2->geo[0][0]-1, Mark2->geo[1][1]-Mark2->geo[1][0]-1);
        cairo_stroke(cr);
    }
}


static gboolean on_area_draw (GtkWidget *w, gpointer compat, gpointer user_data)
{
#if GTK_CHECK_VERSION (3, 0, 0)
    cairo_t * cr = (cairo_t *) compat;
#else // gtk2
    //GdkEventExpose * event = (GdkEventExpose *) compat;
    cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (w));
#endif
    gui_show_pix(w, cr);
#if GTK_MAJOR_VERSION == 2
    cairo_destroy (cr);
#endif
    return TRUE;
}


static void gui_tree_mark1(tree_info_t* info)
{
    if (info != Mark1)
    {
        char* temp;
        char* text = g_strdup(info->node->name);
        Mark1 = info;
        while (info != CurrentItem && info->parent != CurrentItem &&
               info->parent->parent != CurrentItem)
        {
            info = info->parent;
            temp = g_strdup_printf("%s/%s", info->node->name, text);
            g_free(text);
            text = temp;
        }
        gtk_label_set_text(GTK_LABEL(FileLabel), to_utf8(text));
        g_free(text);

        temp = print_filesize(Mark1->node->size);
        text = g_strdup_printf("<b>%s</b>", temp);
        gtk_label_set_markup(FileSizeLabel, text);
        g_free(temp);
        g_free(text);
    }

}

static void gui_tree_unmark1(G_GNUC_UNUSED tree_info_t* info)
{
    Mark1 = NULL;
}

static void gui_tree_unmark2(G_GNUC_UNUSED tree_info_t* info)
{
    Mark2 = NULL;
}

static void gui_tree_mark2(tree_info_t* info)
{
    if (info != Mark2)
    {
        Mark2 = info;
        if (Mark2 != Mark1)
        {
            char* temp;
            char* text;
      
            gtk_label_set_text(SubLabel, to_utf8(info->node->name));
      
            temp = print_filesize(Mark2->node->size);
            text = g_strdup_printf("<b>%s</b>", temp);
            gtk_label_set_markup(SubSizeLabel, text);
            g_free(temp);
            g_free(text);
        }
        else
        {
            gtk_label_set_markup(SubSizeLabel, "<b>-</b>");
            gtk_label_set_text(SubLabel, "-");
        }
    }

}

static void gui_tree_mark(tree_info_t* info)
{
    tree_info_t* tree;

    if (!info || info == Mark1) return;
    if (Mark1) gui_tree_unmark1(Mark1);
    gui_tree_mark1(info);

    tree = tree_info_find_path(CurrentItem, info);

    if (tree == Mark2) return;
    if (Mark2) gui_tree_unmark2(Mark2);
    if (!tree) return;
    gui_tree_mark2(tree);
}

static gboolean on_area_motion(GtkWidget* widget, GdkEventMotion* event)
{
    tree_info_t* info;

    (void)widget;
    if (!CurrentItem) return FALSE;

    info = tree_info_search(CurrentItem, (int)(event->x), (int)(event->y));
    if (info) {
        gui_tree_mark(info);
        gtk_widget_queue_draw(Area);
    }
    return TRUE;
}


static gboolean on_area_button_press(GtkWidget* widget, GdkEventButton* event)
{
    tree_info_t* info;
/*   GtkWidget* pop; */

    (void)widget;
    if (!CurrentItem) return FALSE;

    info = tree_info_search(CurrentItem, (int)(event->x), (int)(event->y));

    if (info)
    {
        if (event->button == 1 && event->type == GDK_2BUTTON_PRESS)
        {
            tree_info_t* sub = tree_info_find_path(CurrentItem, info);
            if (sub) gui_tree_display(sub, FALSE, TRUE);
/*     } else if (event->button == 3) { */
/*       pop = create_right_popup(sub_tree); */
/*       gtk_menu_popup(GTK_MENU(pop), NULL, NULL, NULL, NULL, */
/* 		     event->button, event->time); */
        }
    }

    return FALSE;
}

static GtkWidget* gui_create_display_box(void)
{
    GtkWidget* da;

    da = gtk_drawing_area_new();
    Area = da;
    gtk_widget_show(da);
    gtk_widget_set_events(da, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);
    // no need for offscreen support of gtk, we do it manually
    gtk_widget_set_double_buffered(da, FALSE);

    g_signal_connect(G_OBJECT(da), "configure_event",
                     G_CALLBACK(on_drawingarea_configure), NULL);
    g_signal_connect(G_OBJECT(da), GTKCOMPAT_DRAW_SIGNAL,
                     G_CALLBACK(on_area_draw), NULL);
    g_signal_connect(G_OBJECT(da), "motion_notify_event",
                     G_CALLBACK(on_area_motion), NULL);
    g_signal_connect(G_OBJECT(da), "button_press_event",
                     G_CALLBACK(on_area_button_press), NULL);
    return da;
}

static GtkWidget* gui_create_status_bar(void)
{
    GtkWidget* bar;
    GtkWidget* grid;
    GtkWidget* label;

    bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    StatusBar = GTK_BOX(bar);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_margin_start(grid, 5);
    gtk_widget_set_margin_end(grid, 5);
    gtk_box_pack_start(GTK_BOX(bar), grid, TRUE, TRUE, 0);

    label = gtk_label_new("");
    SubSizeLabel = GTK_LABEL(label);
    gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    label = gtk_label_new("");
    SubLabel = GTK_LABEL(label);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
/*   gtk_widget_set_size_request(label, 50, -1); */
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    label = gtk_label_new("");
    FileSizeLabel = GTK_LABEL(label);
    gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    label = gtk_label_new("");
    FileLabel = GTK_LABEL(label);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
    gtk_widget_set_size_request(label, 50, -1);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 1, 1, 1);

    gtk_widget_show_all (bar);
    return bar;
}

static gboolean on_main_win_delete_event(void)
{
    gtk_main_quit();
    return TRUE;
}


static gboolean on_redraw(tree_info_t* item)
{
    gui_tree_display(item, FALSE, FALSE);
    if (RedrawTimer) g_source_remove(RedrawTimer);
    RedrawTimer = 0;
    return TRUE;
}

static void gui_tree_redraw(void)
{
    LUseColors = pref_get_use_colors();
    LMaxDepth = pref_get_max_depth();
    LDisplayMode = pref_get_display_mode();
    LUseAverage = pref_get_use_average();
    
    if (CurrentItem) 
    {
        if (RedrawTimer) g_source_remove(RedrawTimer);
        RedrawTimer = g_timeout_add(300, (GSourceFunc)on_redraw, CurrentItem);
    }
}

static GdkPixbuf* create_pixbuf(const gchar *filename)
{
    char* temp;
    GdkPixbuf* pix;

#ifdef PACKAGE_SOURCE_DIR
    temp = g_strdup_printf("%s/data/%s", PACKAGE_SOURCE_DIR, filename);
#else
    temp = g_strdup_printf("%s/pixmaps/%s", PACKAGE_DATADIR, filename);
#endif
    pix = gdk_pixbuf_new_from_file(temp, NULL);
    g_free(temp);

    return pix;
}


GtkWidget* gui_create_main_win(void)
{
    GtkWidget* win;
    GtkWidget* vbox;
    GtkWidget* item;
  
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    MainWin = win;
    gtk_window_set_title(GTK_WINDOW(win), "Graphical Disk Map "VERSION);
    gtk_window_set_role(GTK_WINDOW(win), "Main window");
    //gtk_window_set_default_icon(create_pixbuf("gdmap_icon.png"));
    gtk_window_set_default_size(GTK_WINDOW(win), 700, 450);
    g_object_add_weak_pointer(G_OBJECT(win), (void*)&MainWin);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(win), vbox);
  
    create_menubar (GTK_WINDOW(win));
    gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

    toolbar = w_gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, vbox);

    btn_toolbar_open = w_gtk_button_new_from_actions (actions, "FileOpen", toolbar);
    w_gtk_toolbar_separator_new (toolbar);
    btn_toolbar_back = w_gtk_button_new_from_actions (actions, "ViewBack", toolbar);
    btn_toolbar_forward = w_gtk_button_new_from_actions (actions, "ViewForward", toolbar);
    btn_toolbar_up      = w_gtk_button_new_from_actions (actions, "ViewUp", toolbar);
    btn_toolbar_top     = w_gtk_button_new_from_actions (actions, "ViewTop", toolbar);
    w_gtk_toolbar_separator_new (toolbar);
    btn_toolbar_refresh = w_gtk_button_new_from_actions (actions, "ViewRefresh", toolbar);
    w_gtk_toolbar_separator_new (toolbar);
    btn_toolbar_settings = w_gtk_button_new_from_actions (actions, "FileSettings", toolbar);
    if ((item = gui_create_path_box()) != NULL)
        gtk_box_pack_start(GTK_BOX (vbox), item, FALSE, FALSE, 0);

    if ((item = gui_create_display_box()) != NULL)
        gtk_box_pack_start(GTK_BOX (vbox), item, TRUE, TRUE, 0);

    if ((item = gui_create_status_bar()) != NULL)
        gtk_box_pack_start(GTK_BOX (vbox), item, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT (win), "delete_event",
                     G_CALLBACK(on_main_win_delete_event), NULL);
  
    gui_buttons_update();

    gtk_widget_show_all (win);

    pref_set_redraw_callback(gui_tree_redraw);
  
    return win;
}





GtkWidget* gui_get_main_win(void)
{
    return MainWin;
}

static void _get_average_color_rec(tree_t* tree, int* r, int* g, int* b)
{

    if (tree->entries)
    {
        GList* dlist;
        double dr=0, dg=0, db=0;

        for (dlist = tree->entries; dlist; dlist = dlist->next)
        {
            tree_t* sub = dlist->data;
            int lr=0, lg=0, lb=0;
            _get_average_color_rec(sub, &lr, &lg, &lb);
            dr += lr/(double)tree->size*(double)sub->size;
            dg += lg/(double)tree->size*(double)sub->size;
            db += lb/(double)tree->size*(double)sub->size;
        }
        *r += dr;
        *g += dg;
        *b += db;
    }
    else
    {
        const GdkColor* color = color_get_by_file(tree->name);
        *r += color->red;
        *g += color->green;
        *b += color->blue;
    }
}

static const GdkColor* _get_average_color(tree_t* tree)
{
    static GdkColor res;
    int r=0, g=0, b=0;

    _get_average_color_rec(tree, &r, &g, &b);
    res.red = r;
    res.green = g;
    res.blue = b;
    return &res;
}

static void gui_tree_show_item(tree_info_t* info)
{
    static int Ia = 40;
    static int Is = 215;
    static double Lx = 0.09759;
    static double Ly = 0.19518;
    static double Lz = 0.9759;
    int ix, iy;
    double nx, ny, cosa, val;
    long pos;
    const GdkColor* color;

    int x1 = info->geo[0][0];
    int y1 = info->geo[1][0];
    int x2 = info->geo[0][1];
    int y2 = info->geo[1][1];
  
    if (x1 >= x2) return;
    if (y1 >= y2) return;

    if (LUseColors)
    {
        if (info->node->entries)
        {
            if (LUseAverage) color = _get_average_color(info->node); 
            else color = color_get_by_type(COLOR_FOLDER);
        }
        else
        {
            color = color_get_by_file(info->node->name);
        }
    }
    else
    {
        color = color_get_by_type(COLOR_DEFAULT);
    }
  
    unsigned char* buffer = cairo_image_surface_get_data(Surface);
    pos = (y1*Width+x1)*4;
    for (iy = y1; iy < y2; iy++)
    {
        for (ix = x1; ix < x2; ix++)
        {
            nx = -(2*info->s[0][1]*(ix+0.5)+info->s[0][0]);
            ny = -(2*info->s[1][1]*(iy+0.5)+info->s[1][0]);
            cosa = (nx*Lx + ny*Ly + Lz)/sqrt(nx*nx + ny*ny +1.0);
            val = (Ia+MAX(0, Is*cosa));
            buffer[pos++] = (unsigned char)(color->blue  * val/65535.);
            buffer[pos++] = (unsigned char)(color->green * val/65535.);
            buffer[pos++] = (unsigned char)(color->red   * val/65535.);
            pos++;
        }
        pos -= (x2-x1)*4;  // cr
        pos += Width*4;    // lf
    }
}

void AddRidge(double x1, double x2, double h, double* s1, double* s2)
{
    *s1 += 4*h*(x2+x1)/(x2-x1);
    *s2 -= 4*h/(x2-x1);
}


/* void AddRidge(tree_info_t* node, int geo[2][2], int d, double h) { */
/*   int x1 = geo[d][0]; */
/*   int x2 = geo[d][1]; */

/*   node->s[d][0] += 4*h*(x2+x1)/(x2-x1); */
/*   node->s[d][1] -= 4*h/(x2-x1); */
/* } */

static void gui_tree_evaluate_childs1(tree_info_t* info, double h, double f)
{
    tree_t* tree = info->node;
    int d = (tree->depth % 2);
    int pos = info->geo[d][0];
    int width = (info->geo[d][1] - info->geo[d][0]);
    gint64 size = 0;
    GList* dlist;

/*   g_message("%d evaluate '%s' with %u children", tree->depth, tree->name, */
/*             g_list_length(tree->entries)); */
/*   printf("  s: [%.1f,%.1f][%.1f,%.1f]\n", */
/*          info->s[0][0],info->s[0][1],info->s[1][0],info->s[1][1]); */

    for (dlist = info->children; dlist; dlist = dlist->next)
    {
        tree_info_t* sinfo = dlist->data;
        tree_t* sub = sinfo->node;
        int end_pos;
    
        sinfo->geo[0][0] = sinfo->geo[0][1] = 0;
        sinfo->geo[1][0] = sinfo->geo[1][1] = 0;

        if (sub->size <= 0) continue;

        size += sub->size;
        end_pos = info->geo[d][0] + size*width/tree->size;

        if (end_pos > pos+1 || (!dlist->next && end_pos > pos))
        {
            sinfo->geo[d][0] = pos;
            sinfo->geo[d][1] = end_pos;
            sinfo->geo[1-d][0] = info->geo[1-d][0];
            sinfo->geo[1-d][1] = info->geo[1-d][1];
      
            memcpy(sinfo->s, info->s, sizeof(sinfo->s));
            AddRidge(sinfo->geo[d][0], sinfo->geo[d][1], h,
                     &sinfo->s[d][0], &sinfo->s[d][1]);

            pos = end_pos;
            if (sub->entries) gui_tree_evaluate_childs1(sinfo, h*f, f);
        }
    }

}

static gint64
_find_next_size(GList* children, gint64 rsize, int width, int height)
{
    GList* dlist;
    gint64 size = 0;
    double best_ratio = 0.0;
    gint64 best_size = 0;

    g_assert(width >= height);

/*   g_message("   have %ld in (%d,%d) (%u children)", rsize, width, height, */
/*             g_list_length(children)); */

    for (dlist = children; dlist; dlist = dlist->next)
    {
        tree_info_t* sinfo = dlist->data;
        tree_t* sub = sinfo->node;
        int dx, dy;
        double ratio;

        size += sub->size;
        if (size == 0) continue;

        // get the width of the col
        dx = (gint64)width * size / rsize;
        // get the height of the last item.
        dy = (gint64)height * sub->size / size;
        // calculate the aspect ratio
/*     g_message("   -size is %ld (%d %d)", sub->size, dx, dy); */
        if (dx == 0 && dy == 0) continue;

  
        if (dx < dy) ratio = (double)dx/(double)dy;
        else ratio = (double)dy/(double)dx;

/*     g_message("   -ratio (%d,%d) %.1f (size %ld)", dx, dy, ratio, size); */

        if (ratio >= best_ratio)
        {
            best_ratio = ratio;
            best_size = size;
        }
        else
        {
            break;
        }
    }
    return best_size;
}

static void gui_tree_evaluate_childs2(tree_info_t* info, double h, double f)
{
    tree_t* tree = info->node;
    GList* children = info->children;
    gint64 rsize, size;
    int d;
    int geo[2][2];
    double s[2][2];
    
    memcpy(geo, info->geo, sizeof(geo));
    rsize = tree->size;
    
    while (children)
    {
        int width, height;
        int dx, pos;
        gint64 ssize;
        
        if (geo[0][1]-geo[0][0] < geo[1][1]-geo[1][0]) d = 1;
        else d = 0;
        
        width = geo[d][1]-geo[d][0];
        height = geo[1-d][1]-geo[1-d][0];
        
        size = _find_next_size(children, rsize, width, height);
        
        if (rsize) dx = (gint64)width * size / rsize;
        else dx = 0;
        
        ssize = 0;
        pos = geo[1-d][0];
        
        memcpy(s, info->s, sizeof(s));
        AddRidge(geo[d][0], geo[d][0]+dx, h, &s[d][0], &s[d][1]);
        
        for ( ; children; children = children->next)
        {
            tree_info_t* sinfo = children->data;
            tree_t* sub = sinfo->node;
            int end_pos;
            
            sinfo->geo[0][0] = sinfo->geo[0][1] = 0;
            sinfo->geo[1][0] = sinfo->geo[1][1] = 0;
            
            if (sub->size <= 0) continue;
            ssize += sub->size;
            if (ssize > size) break;
            
            end_pos = geo[1-d][0] + ssize*height/size;
            
            if (end_pos > pos+1 || (ssize == size && end_pos > pos))
            {
                sinfo->geo[1-d][0] = pos;
                sinfo->geo[1-d][1] = end_pos;
                sinfo->geo[d][0] = geo[d][0];
                sinfo->geo[d][1] = geo[d][0]+dx;
                
                memcpy(sinfo->s, s, sizeof(sinfo->s));
                AddRidge(sinfo->geo[1-d][0], sinfo->geo[1-d][1], h,
                         &sinfo->s[1-d][0], &sinfo->s[1-d][1]);
                
                pos = end_pos;
                if (sub->entries) gui_tree_evaluate_childs2(sinfo, h*f, f);
            }
        }
        geo[d][0] += dx;
        rsize -= size;
    }
}
  
  
static tree_info_t* gui_tree_evaluate(tree_info_t* info)
{
    int Border = 1;
    
    info->geo[0][0] = Border;
    info->geo[0][1] = Width-Border;
    info->geo[1][0] = Border;
    info->geo[1][1] = Height-Border;
    info->s[0][0] = .0;
    info->s[0][1] = .0;
    info->s[1][0] = .0;
    info->s[1][1] = .0;
    
    if (LDisplayMode == DISPLAY_STANDARD_CUSHION)
    {
        gui_tree_evaluate_childs1(info, pref_get_h(), pref_get_f());
    }
    else
    {
        gui_tree_evaluate_childs2(info, pref_get_h(), pref_get_f());
    }
  
    return info;
}

static void gui_tree_show(tree_info_t* info)
{
    unsigned depth = LMaxDepth;
    
    if (info->geo[0][0] >= info->geo[0][1]) return;
    if (info->geo[1][0] >= info->geo[1][1]) return;
    
    if (info->children && (depth == 0 || info->node->depth < depth))
    {
        GList* dlist;
        for (dlist = info->children; dlist; dlist = dlist->next)
        {
            tree_info_t* sub = dlist->data;
            gui_tree_show(sub);
        }
    }
    else
    {
        gui_tree_show_item(info);
    }
}

static void on_path_toggled(GtkToggleButton* button, tree_info_t* info)
{
    if (gtk_toggle_button_get_active(button))
    {
        gui_tree_display(info, FALSE, TRUE/*FALSE*/);
    }
}

static void gui_paths_update(void)
{
    static GList* Paths = NULL;
    static GSList* Group = NULL;
    GtkWidget* button;
    GList* dlist;
    GList* temp;
    tree_info_t* dtemp;
    tree_info_t* item;

    if (!CurrentItem) return;

    for (item = CurrentItem; item; item = item->parent)
    {

        for (dlist = Paths; dlist; dlist = dlist->next)
        {
            GtkWidget* button = dlist->data;
            tree_info_t* info = g_object_get_data(G_OBJECT(button), "info");
      
            if (info == item)
            {
                if (item == CurrentItem)
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
                break;
            }
        }
        if (dlist) break;
    }

    if (dlist)
    {
        // only delete if we have to add a new end path.
        if (item != CurrentItem) temp = dlist->next;
        else temp = NULL;
/*     temp = dlist->next; */
    }
    else
    {
        // delete all
        temp = Paths;
    }

    for (dlist = temp; dlist; )
    {
        GtkWidget* button = dlist->data;
        dlist = dlist->next;
        Paths = g_list_remove(Paths, button);
        gtk_widget_destroy(button);
    }

    // update radio button group
    if (Paths)
        Group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(Paths->data));
    else
        Group = NULL;

    temp = NULL;
    dtemp = CurrentItem;
    while (dtemp != item)
    {
        temp = g_list_prepend(temp, dtemp);
        dtemp = dtemp->parent;
    }
  
    for (dlist = temp; dlist; dlist = dlist->next)
    {
        tree_info_t* info = dlist->data;
        button = gtk_radio_button_new_with_label(Group, info->node->name);
        Group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
        gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(button), FALSE);
        g_object_set_data(G_OBJECT(button), "info", info);
        gtk_widget_show(button);
        gtk_box_pack_start(PathBox, button, FALSE, FALSE, 0);
        Paths = g_list_append(Paths, button);
        if (info == CurrentItem)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        g_signal_connect(G_OBJECT(button), "toggled",
                         G_CALLBACK(on_path_toggled), info);
    }
}

static void gui_tree_display(tree_info_t* info, gboolean destroy_old, gboolean new_history)
{
    if (destroy_old)
    {
        if (CurrentItem)
        {
            while (CurrentItem->parent) CurrentItem = CurrentItem->parent;
            tree_destroy(CurrentItem->node);
            tree_info_destroy(CurrentItem);
        }
        CurrentItem = NULL;
        history_clear();
    }


    if (CurrentItem != info)
    {
        if (new_history) history_append(info);
        CurrentItem = info;
        gui_paths_update();
    }

    gui_tree_evaluate(info);
    gui_tree_show(info);
    Mark1 = NULL;
    Mark2 = NULL;
    gtk_widget_queue_draw(Area);

    gui_buttons_update();
  
    {
        char* temp1 = print_filesize(info->node->size);
        char* temp2 = g_strdup_printf("<b>%s</b>", temp1);
        gtk_label_set_markup(SizeLabel, temp2);
        g_free(temp1);
        g_free(temp2);
    }

}

