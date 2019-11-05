/**
 * @file  treev.h
 * @copyright Copyright (C) 2019 Michał Bąbik
 *
 * This file is part of Wall Changer.
 *
 * Wall Changer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wall Changer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wall Changer.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @brief  TreeView related functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef TREEV_H
#define TREEV_H
#include <gtk/gtk.h>
#include "iminfo.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  TreeView Columns
 */
enum {
    COL_FULL_FILE_NAME,  /**< Full file name with path */
    COL_FILE_NAME,       /**< Only file name */
    COL_FILE_PATH,       /**< Only file path */
    COL_WIDTH_HEIGHT,    /**< String with width x height */
    COL_WIDTH,           /**< Image width */
    COL_HEIGHT,          /**< Image height */
    NUM_COLS             /**< Number of columns */
};
/*----------------------------------------------------------------------------*/
void liststore_add_items (GtkWidget *gw_list, GSList *gsl_iinfo1);
/*----------------------------------------------------------------------------*/
ImageInfo * treemodel_get_data (GtkTreeModel *gtm_model, GtkTreeIter gti_iter);
/*----------------------------------------------------------------------------*/
void find_select_item (GtkWidget *gw_tview, const char *s_file);
/*----------------------------------------------------------------------------*/
void treeview_remove_selected (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
GSList *treeview_get_data (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
void treeview_sort_list (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
void treeview_move_up (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
void treeview_move_down (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
void create_tview (GtkWidget **gw_tview);
/*----------------------------------------------------------------------------*/
#endif
