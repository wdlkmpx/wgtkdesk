// included by gui_main.c

//#define USE_GTK_ACTION 1
#include "w_gtk_menu.h"

static WGtkMenuBar *menubar;
static WGtkMenu *menu_file;
static WGtkMenu *menu_view;
static WGtkMenu *menu_help;

WGtkMenuItem *menuitem_file;
WGtkMenuItem *menuitem_file_open;
WGtkMenuItem *menuitem_file_settings;
WGtkMenuItem *menuitem_file_quit;

WGtkMenuItem *menuitem_view;
static WGtkMenuItem *menuitem_view_back;
static WGtkMenuItem *menuitem_view_forward;
static WGtkMenuItem *menuitem_view_up;
static WGtkMenuItem *menuitem_view_top;
static WGtkMenuItem *menuitem_view_refresh;

WGtkMenuItem *menuitem_help;
WGtkMenuItem *menuitem_help_about;

WGtkActionEntry actions[] = 
{
    //name           icon_name           label            accel_str     tooltip  callback
    { "File",         NULL,              N_("_File"),        NULL,         NULL, NULL,           },
    { "FileOpen",     "gtk-open",        N_("_Open..."),     "<Control>O", NULL, on_action_open, },
    { "FileSettings", "gtk-preferences", N_("_Settings..."), NULL,         NULL, on_action_preferences, },
    { "FileQuit",     "gtk-quit",        N_("_Quit"),        "<Control>O", NULL, on_action_exit, },
    { "View",         NULL,              N_("_View"),        NULL,         NULL, NULL,           },
    { "ViewBack",     "gtk-go-back",     N_("_Back"),        NULL,         NULL, on_action_back, },
    { "ViewForward",  "gtk-go-forward",  N_("_Forward"),     NULL,         NULL, on_action_forward, },
    { "ViewUp",       "gtk-go-up",       N_("_Up"),          NULL,         NULL, on_action_up,   },
    { "ViewTop",      "gtk-goto-top",    N_("_Top"),         NULL,         NULL, on_action_top,  },
    { "ViewRefresh",  "gtk-refresh",     N_("_Refresh"),     NULL,         NULL, on_action_refresh, },
    { "Help",         NULL,              N_("_View"),        NULL,         NULL, NULL,           },
    { "HelpAbout",    "gtk-about",       N_("_About..."),    NULL,         NULL, on_action_about, },
    { NULL,           NULL,              NULL,               NULL,         NULL, NULL, },
};


static void create_menubar (GtkWindow *window)
{
    WGtkMenuItemParams item;
    w_gtk_menuitem_params_init (&item, actions);

    menubar = w_gtk_menu_bar_new ();
    menu_file = w_gtk_menu_new ();
    menu_view = w_gtk_menu_new ();
    menu_help = w_gtk_menu_new ();

    // Menu File ----------------------------------------
    item.parent_menu = menubar;
    item.submenu     = menu_file;
    item.action_name = "File";
    menuitem_file = w_gtk_menuitem_new (&item);

    // File -> Open
    item.parent_menu = menu_file;
    item.action_name = "FileOpen";
    menuitem_file_open = w_gtk_menuitem_new (&item);
    // --
    w_gtk_menuitem_new (&item);
    // File -> Settings
    item.action_name = "FileSettings";
    menuitem_file_settings = w_gtk_menuitem_new (&item);
    // --
    w_gtk_menuitem_new (&item);
    // File -> Quit
    item.action_name = "FileQuit";
    menuitem_file_quit = w_gtk_menuitem_new (&item);

    // Menu View ----------------------------------------
    item.parent_menu = menubar;
    item.submenu     = menu_view;
    item.action_name = "View";
    menuitem_view = w_gtk_menuitem_new (&item);

    // View -> Back
    item.parent_menu = menu_view;
    item.action_name = "ViewBack";
    menuitem_view_back = w_gtk_menuitem_new (&item);
    // View -> Forward
    item.action_name = "ViewForward";
    menuitem_view_forward = w_gtk_menuitem_new (&item);
    // View -> Up
    item.action_name = "ViewUp";
    menuitem_view_up = w_gtk_menuitem_new (&item);
    // View -> Top
    item.action_name = "ViewTop";
    menuitem_view_top = w_gtk_menuitem_new (&item);
    // --
    w_gtk_menuitem_new (&item);
    // View -> Refresh
    item.action_name = "ViewRefresh";
    menuitem_view_refresh = w_gtk_menuitem_new (&item);

    // Menu Help ----------------------------------------
    item.parent_menu = menubar;
    item.submenu     = menu_help;
    item.action_name = "Help";
    menuitem_help = w_gtk_menuitem_new (&item);

    // Help -> About
    item.parent_menu = menu_help;
    item.action_name = "HelpAbout";
    menuitem_help_about = w_gtk_menuitem_new (&item);

    if (window && item.accel_group) {
        gtk_window_add_accel_group (GTK_WINDOW(window), item.accel_group);
    }
}
