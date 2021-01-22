/**
 * @file  treev.h
 * @copyright Copyright (C) 2019-2021 Michał Bąbik
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef TREEV_H
#define TREEV_H

#include <gtk/gtk.h>
#include "iminfo.h"
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  void treeview_add_items_glist (GtkWidget   *gw_tview,
 *                                     const GList *gl_files)
 *
 * @brief  Insert data items from GList to GtkTreeView.
 *
 * @param[out] gw_tview  TreeView to add items
 * @param[in]  gl_files  GList list with data in ImageInfo format
 * @return     none
 *
 * @fn  void treeview_add_items_gslist (GtkWidget    *gw_tview,
 *                                      const GSList *gl_files)
 *
 * @brief  Insert data items from GSList to GtkTreeView.
 *
 * @param[out] gw_tview  TreeView to add items
 * @param[in]  gl_files  GSList list with data in ImageInfo format
 * @return     none
 *
 * @fn  void treeview_add_items_setting (GtkWidget *gw_tview,
 *                                       Setting   *st_wallpapers)
 *
 * @brief  Insert data items from Setting list to GtkTreeView.
 *
 * @param[out] gw_tview       TreeView to add items
 * @param[in]  st_wallpapers  List of Setting items with file paths
 * @return     none
 *
 * @fn  ImageInfo * treemodel_get_data (GtkTreeModel *gtm_model,
 *                                      GtkTreeIter gti_iter)
 *
 * @brief  Get data from GtkTreeModel to ImageInfo object.
 *
 * @param[in] gtm_model  GtkTreeModel with data to get
 * @param[in] gti_iter   GtkTreeIter with row adress
 * @return    ImageInfo item with readed data
 *
 * @fn  GSList *treeview_get_data (GtkWidget *gw_tview)
 *
 * @brief  Get data out of TreeView.
 *
 * @param[in] gw_tview   TreeView from which data should be taken
 * @return    List with  ImageInfo data of TreeView's TreeModel items
 *
 * @fn void treeview_get_setting_data (GtkWidget     *gw_tview,
                                       const Setting *st_wallpapers)

 * @brief  Get data out of TreeView to Setting list.
 *
 * @param[in]  gw_tview       TreeView with wallpapers
 * @param[out] st_wallpapers  List to insert Setting items with file paths
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void        treeview_add_items_glist     (GtkWidget      *gw_tview,
                                          const GList    *gl_files);

void        treeview_add_items_gslist    (GtkWidget      *gw_tview,
                                          const GSList   *gl_files);

void        treeview_add_items_setting   (GtkWidget      *gw_tview,
                                          const Setting  *st_wallpapers);

ImageInfo * treemodel_get_data           (GtkTreeModel   *gtm_model,
                                          GtkTreeIter     gti_iter);

GSList    * treeview_get_data            (GtkWidget      *gw_tview);

Setting *   treeview_get_setting_data    (GtkWidget      *gw_tview);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void treeview_remove_duplicates (GtkWidget *gw_tview)
 *
 * @brief  Remove duplicates from TreeView file list.
 *
 * @param[in,out] gw_tview  TreeView to process
 * @return        none
 *
 * @fn  void treeview_remove_selected (GtkWidget *gw_tview)
 *
 * @brief  Remove selected items from TreeView
 *
 * @param[out] gw_tview  GtkTreeView with items to remove
 * @return     none
 *
 * @fn  void treeview_move_up (GtkWidget *gw_tview)
 *
 * @brief  Move up selected items in TreeView.
 *
 * @param[in,out] gw_tview  TreeView to move data up
 * @return        none
 *
 * @fn  void treeview_move_down (GtkWidget *gw_tview)
 *
 * @brief  Move down selected items in TreeView.
 *
 * @param[in,out] gw_tview  TreeView to move data down
 * @return        none
 *
 * @fn  void treeview_find_select_item (GtkWidget *gw_tview,
 *                                      const char *s_file)
 *
 * @brief  Find file on list and select it
 *
 * @param[in] gw_tview  GtkTreeView on which data should be selected
 * @param[in] s_file    File path to find
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void treeview_remove_duplicates (GtkWidget  *gw_tview);

void treeview_remove_selected   (GtkWidget  *gw_tview);

void treeview_move_up           (GtkWidget  *gw_tview);

void treeview_move_down         (GtkWidget  *gw_tview);

void treeview_find_select_item  (GtkWidget  *gw_tview,
                                 const char *s_file)
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
 * @param[in] gw_tview  Treeview with wallpaper list
 * @return    Wallpaper image path. After use it should be freed using free.
 */
char * treeview_get_one_file (GtkWidget *gw_tview);
/*----------------------------------------------------------------------------*/
#endif

