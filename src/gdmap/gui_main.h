/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */

#ifndef _GUI_MAIN_H_
#define _GUI_MAIN_H_

#include "tree.h"

GtkWidget* gui_create_main_win (void);
GtkWidget* gui_get_main_win();
void gui_tree_load_and_display(const char* folder);

#endif
