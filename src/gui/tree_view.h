/**
 * @file  tree_view.h
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef TREE_VIEW_H
#define TREE_VIEW_H

#include <gtk/gtk.h>
#include "image_info.h"
#include "settings/setting_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  void treeview_add_items_glist (GtkWidget   *tree_view,
 *                                     const GList *files)
 *
 * @brief  Insert data items from GList to GtkTreeView.
 *
 * @param[out] tree_view  TreeView to add items
 * @param[in]  files      GList list with data in ImageInfo format
 * @return     none
 *
 * @fn  void treeview_add_items_gslist (GtkWidget    *tree_view,
 *                                      const GSList *files)
 *
 * @brief  Insert data items from GSList to GtkTreeView.
 *
 * @param[out] tree_view  TreeView to add items
 * @param[in]  files      GSList list with data in ImageInfo format
 * @return     none
 *
 * @fn  void treeview_add_items_setting (GtkWidget *tree_view,
 *                                       Setting   *wallpapers)
 *
 * @brief  Insert data items from Setting list to GtkTreeView.
 *
 * @param[out] tree_view   TreeView to add items
 * @param[in]  wallpapers  List of Setting items with file paths
 * @return     none
 *
 * @fn  ImageInfo * treemodel_get_data (GtkTreeModel *model,
 *                                      GtkTreeIter iter)
 *
 * @brief  Get data from GtkTreeModel to ImageInfo object.
 *
 * @param[in] model  GtkTreeModel with data to get
 * @param[in] iter   GtkTreeIter with row address
 * @return    ImageInfo item with read data
 *
 * @fn  GSList *treeview_get_data (GtkWidget *tree_view)
 *
 * @brief  Get data out of TreeView.
 *
 * @param[in] tree_view  TreeView from which data should be taken
 * @return    List with  ImageInfo data of TreeView's TreeModel items
 *
 * @fn  Setting * treeview_get_setting_data (GtkWidget *tree_view)
 *
 * @brief  Get data out of TreeView to Setting list.
 *
 * @param[in] tree_view  TreeView with wallpapers
 * @return    New Setting list with file paths of items in TreeView
 */
/*----------------------------------------------------------------------------*/
void        treeview_add_items_glist     (GtkWidget      *tree_view,
                                          const GList    *files);

void        treeview_add_items_gslist    (GtkWidget      *tree_view,
                                          const GSList   *files);

void        treeview_add_items_setting   (GtkWidget      *tree_view,
                                          const Setting  *wallpapers);

ImageInfo * treemodel_get_data           (GtkTreeModel   *model,
                                          GtkTreeIter     iter);

GSList    * treeview_get_data            (GtkWidget      *tree_view);

Setting *   treeview_get_setting_data    (GtkWidget      *tree_view);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void treeview_remove_duplicates (GtkWidget *tree_view)
 *
 * @brief  Remove duplicates from TreeView file list.
 *
 * @param[in,out] tree_view  TreeView to process
 * @return        none
 *
 * @fn  void treeview_remove_selected (GtkWidget *tree_view)
 *
 * @brief  Remove selected items from TreeView
 *
 * @param[out] tree_view  GtkTreeView with items to remove
 * @return     none
 *
 * @fn  void treeview_move_up (GtkWidget *tree_view)
 *
 * @brief  Move up selected items in TreeView.
 *
 * @param[in,out] tree_view  TreeView to move data up
 * @return        none
 *
 * @fn  void treeview_move_down (GtkWidget *tree_view)
 *
 * @brief  Move down selected items in TreeView.
 *
 * @param[in,out] tree_view  TreeView to move data down
 * @return        none
 *
 * @fn  void treeview_find_select_item (GtkWidget *tree_view,
 *                                      const char *file)
 *
 * @brief  Find file on list and select it
 *
 * @param[in] tree_view  GtkTreeView on which data should be selected
 * @param[in] file       File path to find
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void treeview_remove_duplicates (GtkWidget  *tree_view);

void treeview_remove_selected   (GtkWidget  *tree_view);

void treeview_move_up           (GtkWidget  *tree_view);

void treeview_move_down         (GtkWidget  *tree_view);

void treeview_find_select_item  (GtkWidget  *tree_view,
                                 const char *file)
                                 __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create treeview for image list.
 *
 * @return Result TreeView
 */
GtkWidget * create_tview (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get one file path from wallpaper list.
 *
 * @param[in] tree_view  Treeview with wallpaper list
 * @return    Wallpaper image path. After use it should be freed using free.
 */
char * treeview_get_one_file (GtkWidget *tree_view);
/*----------------------------------------------------------------------------*/
#endif

