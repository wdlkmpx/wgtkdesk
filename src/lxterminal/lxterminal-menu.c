#include "w_gtk_menu.h"

static GtkWidget * vtemenu;


static GtkWidget * create_popup_menu (void *user_data)
{
    GtkWidget * menu = gtk_menu_new();

    WGtkMenuItemParams menuitem;
    memset (&menuitem, 0, sizeof(menuitem));

    menuitem.parent_menu = menu;
    menuitem.label       = _("New _Window");
    menuitem.icon_name   = "list-add";
    menuitem.icon_alt    = "gtk-add";
    menuitem.action_name = "NewWindow";
    menuitem.activate_cb = terminal_new_window_activate_event;
    menuitem.cb_data_all = user_data;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("New _Tab");
    menuitem.icon_name   = "list-add";
    menuitem.icon_alt    = "gtk-add";
    menuitem.action_name = "NewTab";
    menuitem.activate_cb = terminal_new_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    /* sep */
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Open _URL");
    menuitem.action_name = "OpenURL";
    menuitem.activate_cb = terminal_open_url_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Copy _URL");
    menuitem.action_name = "CopyURL";
    menuitem.activate_cb = terminal_copy_url_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Cop_y");
    menuitem.icon_name   = "edit-copy";
    menuitem.icon_alt    = "gtk-copy";
    menuitem.action_name = "Copy";
    menuitem.activate_cb = terminal_copy_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("_Paste");
    menuitem.icon_name   = "edit-paste";
    menuitem.icon_alt    = "gtk-paste";
    menuitem.action_name = "Paste";
    menuitem.activate_cb = terminal_paste_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Cl_ear scrollback");
    menuitem.action_name = "Clear";
    menuitem.activate_cb = terminal_clear_activate_event;
    w_gtk_menuitem_new (&menuitem);

    /* sep */
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Preference_s");
    menuitem.icon_name   = "system-run";
    menuitem.icon_alt    = "gtk-preferences";
    menuitem.action_name = "Preferences";
    menuitem.activate_cb = terminal_preferences_dialog;
    w_gtk_menuitem_new (&menuitem);

    /* sep */
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Na_me Tab");
    menuitem.icon_name   = "dialog-information";
    menuitem.icon_alt    = "gtk-dialog-info";
    menuitem.action_name = "NameTab";
    menuitem.activate_cb = terminal_name_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Pre_vious Tab");
    menuitem.icon_name   = "go-previous";
    menuitem.icon_alt    = "gtk-go-back";
    menuitem.action_name = "PreviousTab";
    menuitem.activate_cb = terminal_previous_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Ne_xt Tab");
    menuitem.icon_name   = "go-next";
    menuitem.icon_alt    = "gtk-go-forward";
    menuitem.action_name = "NextTab";
    menuitem.activate_cb = terminal_next_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Move Tab _Left");
    menuitem.action_name = "Tabs_MoveTabLeft";
    menuitem.activate_cb = terminal_move_tab_left_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Move Tab _Right");
    menuitem.action_name = "Tabs_MoveTabRight";
    menuitem.activate_cb = terminal_move_tab_right_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("_Close Tab");
    menuitem.icon_name   = "window-close";
    menuitem.icon_alt    = "gtk-close";
    menuitem.action_name = "CloseTab";
    menuitem.activate_cb = terminal_close_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    return menu;
}

// ===============================================================


static void terminal_menubar_initialize (LXTerminal * terminal)
{
    GtkWidget * menubar   = gtk_menu_bar_new ();
    GtkWidget * menu_file = gtk_menu_new ();
    GtkWidget * menu_edit = gtk_menu_new ();
    GtkWidget * menu_view = gtk_menu_new ();
    GtkWidget * menu_tabs = gtk_menu_new ();
    GtkWidget * menu_help = gtk_menu_new ();

    GtkAccelGroup  *accel_group  = gtk_accel_group_new ();

    WGtkMenuItemParams menuitem;
    memset (&menuitem, 0, sizeof(menuitem));

    Setting * setting = get_setting ();

    terminal->menu = menubar;
    terminal->accel_group  = accel_group;

    /* ### Menu File ### */
    menuitem.parent_menu = menubar;
    menuitem.submenu     = menu_file;
    menuitem.label       = _("_File");
    menuitem.action_name = "File";
    menuitem.accel_group = accel_group; 
    menuitem.cb_data_all = (void*)terminal;
    w_gtk_menuitem_new (&menuitem);

    menuitem.parent_menu = menu_file;
    menuitem.label       = _("_New Window");
    menuitem.icon_name   = "list-add";
    menuitem.icon_alt    = "gtk-add";
    menuitem.action_name = "File_NewWindow";
    menuitem.accel_str   = setting->new_window_accel;
    menuitem.accel_path  = ACCEL_PATH_FILE_NEWWINDOW;
    menuitem.activate_cb = terminal_new_window_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("New T_ab");
    menuitem.icon_name   = "list-add";
    menuitem.icon_alt    = "gtk-add";
    menuitem.action_name = "File_NewTab";
    menuitem.activate_cb = terminal_new_tab_activate_event;
    menuitem.accel_str   = setting->new_tab_accel;
    menuitem.accel_path  = ACCEL_PATH_FILE_NEWTAB;
    w_gtk_menuitem_new (&menuitem);

    /* sep */
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("_Close Tab");
    menuitem.icon_name   = "window-close";
    menuitem.icon_alt    = "gtk-close";
    menuitem.action_name = "File_CloseTab";
    menuitem.accel_str   = setting->close_tab_accel;
    menuitem.accel_path  = ACCEL_PATH_FILE_CLOSETAB;
    menuitem.activate_cb = terminal_close_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Close _Window");
    menuitem.icon_name   = "application-exit";
    menuitem.icon_alt    = "gtk-quit";
    menuitem.action_name = "File_CloseWindow";
    menuitem.accel_str   = setting->close_window_accel;
    menuitem.accel_path  = ACCEL_PATH_FILE_CLOSEWINDOW;
    menuitem.activate_cb = terminal_close_window_activate_event;
    w_gtk_menuitem_new (&menuitem);

    /* ### Menu Edit ### */
    menuitem.parent_menu = menubar;
    menuitem.submenu     = menu_edit;
    menuitem.label       = _("_Edit");
    menuitem.action_name = "Edit";
    w_gtk_menuitem_new (&menuitem);

    menuitem.parent_menu = menu_edit;
    menuitem.label       = _("Cop_y");
    menuitem.icon_name   = "edit-copy";
    menuitem.icon_alt    = "gtk-copy";
    menuitem.action_name = "Edit_Copy";
    menuitem.accel_str   = setting->copy_accel;
    menuitem.accel_path  = ACCEL_PATH_EDIT_COPY;
    menuitem.activate_cb = terminal_copy_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("_Paste");
    menuitem.icon_name   = "edit-paste";
    menuitem.icon_alt    = "gtk-paste";
    menuitem.action_name = "Edit_Paste";
    menuitem.accel_str   = setting->paste_accel;
    menuitem.accel_path  = ACCEL_PATH_EDIT_PASTE;
    menuitem.activate_cb = terminal_paste_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Clear scr_ollback");
    menuitem.action_name = "Edit_Clear";
    menuitem.activate_cb = terminal_clear_activate_event;
    w_gtk_menuitem_new (&menuitem);

    /* sep */
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Preference_s");
    menuitem.icon_name   = "system-run";
    menuitem.icon_alt    = "gtk-preferences";
    menuitem.action_name = "Edit_Preferences";
    menuitem.activate_cb = terminal_preferences_dialog;
    w_gtk_menuitem_new (&menuitem);

    /* ### Menu View ### */
    menuitem.parent_menu = menubar;
    menuitem.submenu     = menu_view;
    menuitem.label       = _("_View");
    menuitem.action_name = "View";
    w_gtk_menuitem_new (&menuitem);

    menuitem.parent_menu = menu_view;
    menuitem.label       = _("_Fullscreen");
    menuitem.action_name = "View_Fullscreen";
    menuitem.checkbox    = TRUE;
    menuitem.accel_str   = setting->fullscreen_accel;
    menuitem.accel_path  = ACCEL_PATH_VIEW_FULLSCREEN;
    menuitem.activate_cb = terminal_window_toggle_fullscreen;
    w_gtk_menuitem_new (&menuitem);

    /* sep */
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Zoom _In");
    menuitem.icon_name   = "zoom-in";
    menuitem.icon_alt    = "gtk-zoom-in";
    menuitem.action_name = "View_ZoomIn";
    menuitem.accel_str   = setting->zoom_in_accel;
    menuitem.accel_path  = ACCEL_PATH_VIEW_ZOOMIN;
    menuitem.activate_cb = terminal_zoom_in_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Zoom O_ut");
    menuitem.icon_name   = "zoom-out";
    menuitem.icon_alt    = "gtk-zoom-out";
    menuitem.action_name = "View_ZoomOut";
    menuitem.accel_str   = setting->zoom_out_accel;
    menuitem.accel_path  = ACCEL_PATH_VIEW_ZOOMOUT;
    menuitem.activate_cb = terminal_zoom_out_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Zoom _Reset");
    menuitem.icon_name   = "zoom-fit-best";
    menuitem.icon_alt    = "gtk-zoom-fit";
    menuitem.action_name = "View_ZoomReset";
    menuitem.accel_str   = setting->zoom_reset_accel;
    menuitem.accel_path  = ACCEL_PATH_VIEW_ZOOMRESET;
    menuitem.activate_cb = terminal_zoom_reset_activate_event;
    w_gtk_menuitem_new (&menuitem);

    /* ### Menu Tabs ### */
    menuitem.parent_menu = menubar;
    menuitem.submenu     = menu_tabs;
    menuitem.label       = _("_Tabs");
    menuitem.action_name = "Tabs";
    w_gtk_menuitem_new (&menuitem);

    menuitem.parent_menu = menu_tabs;
    menuitem.label       = _("Na_me Tab");
    menuitem.icon_name   = "dialog-information";
    menuitem.icon_alt    = "gtk-dialog-info";
    menuitem.action_name = "Tabs_NameTab";
    menuitem.accel_str   = setting->name_tab_accel;
    menuitem.accel_path  = ACCEL_PATH_TABS_NAMETAB;
    menuitem.activate_cb = terminal_name_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Pre_vious Tab");
    menuitem.icon_name   = "go-previous";
    menuitem.icon_alt    = "gtk-go-back";
    menuitem.action_name = "Tabs_PreviousTab";
    menuitem.accel_str   = setting->previous_tab_accel;
    menuitem.accel_path  = ACCEL_PATH_TABS_PREVIOUSTAB;
    menuitem.activate_cb = terminal_previous_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Ne_xt Tab");
    menuitem.icon_name   = "go-next";
    menuitem.icon_alt    = "gtk-go-forward";
    menuitem.action_name = "Tabs_NextTab";
    menuitem.accel_str   = setting->next_tab_accel;
    menuitem.accel_path  = ACCEL_PATH_TABS_NEXTTAB;
    menuitem.activate_cb = terminal_next_tab_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Move Tab _Left");
    menuitem.action_name = "Tabs_MoveTabLeft";
    menuitem.accel_str   = setting->move_tab_left_accel;
    menuitem.accel_path  = ACCEL_PATH_TABS_MOVELEFT;
    menuitem.activate_cb = terminal_move_tab_left_activate_event;
    w_gtk_menuitem_new (&menuitem);

    menuitem.label       = _("Move Tab _Right");
    menuitem.action_name = "Tabs_MoveTabRight";
    menuitem.accel_str   = setting->move_tab_right_accel;
    menuitem.accel_path  = ACCEL_PATH_TABS_MOVERIGHT;
    menuitem.activate_cb = terminal_move_tab_right_activate_event;
    w_gtk_menuitem_new (&menuitem);

    /* ### Menu Help ### */
    menuitem.parent_menu = menubar;
    menuitem.submenu     = menu_help;
    menuitem.label       = _("_Help");
    menuitem.action_name = "Help";
    w_gtk_menuitem_new (&menuitem);

    menuitem.parent_menu = menu_help;
    menuitem.label       = _("_About");
    menuitem.icon_name   = "help-about";
    menuitem.icon_alt    = "gtk-about";
    menuitem.action_name = "Help_About";
    menuitem.activate_cb = terminal_about_activate_event;
    w_gtk_menuitem_new (&menuitem);

    gtk_window_add_accel_group (GTK_WINDOW(terminal->window), accel_group);

    return;
}

