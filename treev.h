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
#include "miscfun.h"
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
/**
 * @brief  Insert multiple data items to GtkListStore.
 *
 * @param[out]  gw_list     GtkWidget with GtkListStore to insert items
 * @param[in]   gsl_iinfo1  GSList list with data in ImageInfo format
 * @return      none
 */
void liststore_add_items (GtkWidget *gw_list, GSList *gsl_iinfo1);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from GtkTreeModel to ImageInfo object.
 *
 * @param[in]  gtm_model GtkTreeModel with data to get
 * @param[in]  gti_iter  GtkTreeIter with row adress
 * @return     ImageInfo item with readed data
 */
ImageInfo * treemodel_get_data (GtkTreeModel *gtm_model, GtkTreeIter gti_iter);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find file on list and select it
 *
 * @param[in]   gw_tview  GtkTreeView on which data should be selected
 * @param[in]   s_file    File path to find
 * @return      none
 */
void treeview_find_select_item (GtkWidget *gw_tview, const char *s_file);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove selected items from TreeView
 *
 * @param[out]  gw_tview  GtkTreeView with items to remove
 * @return      none
 */
void treeview_remove_selected (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data out of TreeView.
 *
 * @param[in]  gw_tview   TreeView from which data should be taken
 * @return     List with ImageInfo data of TreeView's TreeModel items
 */
GSList *treeview_get_data (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort data in TreeView.
 *
 * @param[in,out]  gw_tview  TreeView to process
 * @return         none
 */
void treeview_sort_list (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove duplicates from TreeView file list.
 *
 * @param[in,out]  gw_tview  TreeView to process
 * @return         none
 */
void treeview_remove_duplicates (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move up selected items in TreeView.
 *
 * @param[in,out]  gw_tview  TreeView to move data up
 * @return         none
 */
void treeview_move_up (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move down selected items in TreeView.
 *
 * @param[in,out]  gw_tview  TreeView to move data down
 * @return         none
 */
void treeview_move_down (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create treeview for image list.
 *
 * @param[out]  gw_tview  Pointer to Treeview to set
 * @return      none
 */
void create_tview (GtkWidget **gw_tview);
/*----------------------------------------------------------------------------*/
#endif

