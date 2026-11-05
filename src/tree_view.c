/**
 * @file  tree_view.c
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
#include <stdint.h>
#include "string_functions.h"
#include "tree_view.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  TreeView Columns
 */
enum e_tree_view_columns {
    COL_FILE_PATH,     /**< Full file name with path */
    COL_FILE_NAME,     /**< Only file name */
    COL_FILE_DIR,      /**< Only file path */
    COL_WIDTH_HEIGHT,  /**< String with width x height */
    COL_WIDTH,         /**< Image width */
    COL_HEIGHT,        /**< Image height */
    NUM_COLS           /**< Number of columns */
};
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void liststore_set_item (GtkListStore    *store,
 *                                      GtkTreeIter     *iter,
 *                                      const ImageInfo *info)
 *
 * @brief  Set data in GtkListStore.
 *
 * Sets data from info ImageInfo to store GtkListStore row pointed
 * by iter GtkTreeIter.
 *
 * @param[out] store    GtkListStore to set data
 * @param[in]  iter     GtkTreeIter row address
 * @param[in]  info  Data in ImageInfo object
 * @return     none
 *
 * @fn  static void treeview_add_item (GtkWidget       *tree_view,
 *                                     const ImageInfo *info)
 *
 * @brief  Insert single data item to GtkListStore.
 *
 * @param[out] tree_view   TreeView in which data should be added
 * @param[in]  info  data in ImageInfo object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void liststore_set_item    (GtkListStore    *store,
                                   GtkTreeIter     *iter,
                                   const ImageInfo *info);

static void treeview_add_item     (GtkWidget       *tree_view,
                                   const ImageInfo *info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set data in GtkListStore.
 */
static void
liststore_set_item (GtkListStore    *store,
                    GtkTreeIter     *iter,
                    const ImageInfo *info)
{
    gtk_list_store_set (store, iter,
                        COL_FILE_PATH,    image_info_get_file_path (info),
                        COL_FILE_NAME,    image_info_get_file_name (info),
                        COL_FILE_DIR,     image_info_get_file_dir  (info),
                        COL_WIDTH_HEIGHT, image_info_get_wxh       (info),
                        COL_WIDTH,        image_info_get_width     (info),
                        COL_HEIGHT,       image_info_get_height    (info),
                        -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert single data item to GtkListStore.
 */
static void
treeview_add_item (GtkWidget       *tree_view,
                   const ImageInfo *info)
{
    GtkListStore *store;
    GtkTreeIter   iter;

    store = GTK_LIST_STORE (gtk_tree_view_get_model (
                GTK_TREE_VIEW (tree_view)));

    gtk_list_store_append (store, &iter);
    liststore_set_item (store, &iter, info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkTreeView.
 */
void
treeview_add_items_glist (GtkWidget   *tree_view,
                          const GList *files)
{
    const char *file    = nullptr; /* String for file path */
    ImageInfo  *info = nullptr; /* Image information */

    while (files != nullptr) {

        file = (const char *) files->data;

        if ((info = image_info_new_from_file (file)) != nullptr) {

            treeview_add_item (tree_view, info);
            image_info_free (info);
        }
        files = files->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkTreeView.
 */
void
treeview_add_items_gslist (GtkWidget    *tree_view,
                           const GSList *files)
{
    const char *file    = nullptr; /* String for file path */
    ImageInfo  *info = nullptr; /* Image information */

    while (files != nullptr) {

        file = (const char *) files->data;

        if ((info = image_info_new_from_file (file)) != nullptr) {
            treeview_add_item (tree_view, info);
            image_info_free (info);
        }
        files = files->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkTreeView.
 */
void
treeview_add_items_setting (GtkWidget     *tree_view,
                            const Setting *wallpapers)
{
    const char *file    = nullptr; /* File name to process */
    ImageInfo  *info = nullptr; /* ImageInfo wallpaper info */

    while (wallpapers != nullptr) {
        if ((file = setting_get_string (wallpapers)) != nullptr) {
            if ((info = image_info_new_from_file (file)) != nullptr) {
                treeview_add_item (tree_view, info);
                image_info_free (info);
            }
        }
        wallpapers = wallpapers->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from GtkTreeModel to ImageInfo object.
 */
ImageInfo *
treemodel_get_data (GtkTreeModel *model,
                    GtkTreeIter   iter)
{
    ImageInfo *info = image_info_new ();

    gtk_tree_model_get (model, &iter,
            COL_FILE_PATH,     &info->file_path, 
            COL_FILE_NAME,     &info->file_name,
            COL_FILE_DIR,      &info->file_dir,
            COL_WIDTH_HEIGHT,  &info->width_height,
            COL_WIDTH,         &info->width,
            COL_HEIGHT,        &info->height,
            -1);
    return info;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data out of TreeView.
 */
GSList *
treeview_get_data (GtkWidget *tree_view)
{
    GtkTreeModel *model;         /* TreeModel */
    ImageInfo    *info;           /* Wallpaper image info */
    GtkTreeIter   iter;          /* TreeIter */
    GSList       *info_list = nullptr;  /* ImageInfo return list */
    gboolean      valid     = FALSE; /* Find item result */

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    valid = gtk_tree_model_get_iter_first (model, &iter);

    while (valid) {
        info = treemodel_get_data (model, iter);
        info_list = g_slist_append (info_list, info);

        valid = gtk_tree_model_iter_next (model, &iter);
    }
    return info_list;
}
/*----------------------------------------------------------------------------*/
Setting *
treeview_get_setting_data (GtkWidget *tree_view)
{
    GtkTreeModel *model;           /* TreeModel */
    GtkTreeIter   iter;            /* TreeIter */
    Setting      *list  = nullptr; /* Setting item to return */
    ImageInfo    *info  = nullptr; /* Info with wallpaper data */
    gboolean      valid = FALSE;   /* Find item result */

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    valid = gtk_tree_model_get_iter_first (model, &iter);

    while (valid) {
        info = treemodel_get_data (model, iter);
        list = settings_append (list,
                setting_new_string (nullptr, image_info_get_file_path (info)));
        image_info_free (info);

        valid = gtk_tree_model_iter_next (model, &iter);
    }
    return list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find file on list and select it
 */
void
treeview_find_select_item (GtkWidget  *tree_view,
                           const char *file)
{
    GtkTreeModel *model;             /* TreeModel */
    GtkTreePath  *path;              /* TreePath */
    GtkTreeIter   iter;              /* TreeIter */
    gboolean      valid     = FALSE; /* Find item result */
    GValue        value     = {0,};  /* Item value */
    const char   *file_path = nullptr;  /* String with value */

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    valid = gtk_tree_model_get_iter_first (model, &iter);

    while (valid) {
        gtk_tree_model_get_value (model, &iter,
                                  COL_FILE_PATH, &value);

        file_path = (const char*) g_value_get_string (&value);

        if (str_compare (file, file_path) == 0) {
            path = gtk_tree_model_get_path (model, &iter);
            gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (tree_view), path,
                                          nullptr, TRUE, 0.5, 0);
            gtk_tree_view_set_cursor (GTK_TREE_VIEW (tree_view),
                                      path, nullptr, FALSE);
            gtk_tree_path_free (path);
            break;
        }
        g_value_unset(&value);
        valid = gtk_tree_model_iter_next (model, &iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove duplicates from TreeView file list.
 */
void
treeview_remove_duplicates (GtkWidget *tree_view)
{
    GtkTreeModel *model;              /* TreeModel */
    GtkTreeIter   iter;               /* TreeIter */
    GtkTreeIter   next;               /* TreeIter */
    GtkTreeIter   current;            /* TreeIter */
    gboolean      valid      = FALSE; /* Getting iter result */
    gboolean      next_valid = FALSE; /* Getting iter result */
    GValue        value      = {0,};  /* Value to get */
    GValue        value2     = {0,};  /* Value to get */
    const char   *file_path  = nullptr; /* First string to compare */
    const char   *other_path = nullptr; /* Second string to compare*/

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    valid = gtk_tree_model_get_iter_first (model, &iter);

    while (valid) {

        next = iter;
        next_valid = gtk_tree_model_iter_next (model, &next);

        gtk_tree_model_get_value (model, &iter,
                                  COL_FILE_PATH, &value);
        file_path = (const char*) g_value_get_string (&value);

        while (next_valid) {

            current = next;
            next_valid = gtk_tree_model_iter_next (model, &next);

            gtk_tree_model_get_value (model, &current,
                                      COL_FILE_PATH, &value2);
            other_path = (const char*) g_value_get_string (&value2);

            if (str_compare (file_path, other_path) == 0) {

                gtk_list_store_remove (GTK_LIST_STORE (model), &current);
            }
            g_value_unset (&value2);
        }
        g_value_unset (&value);
        valid = gtk_tree_model_iter_next (model, &iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove selected items from TreeView
 */
void
treeview_remove_selected (GtkWidget *tree_view)
{
    GtkTreeSelection *selection;       /* TreeSelection */
    GtkTreeModel     *model;           /* TreeModel */
    GtkTreeIter       iter;            /* TreeIter */
    GList            *rows = nullptr;     /* List of selected items */
    GList            *row  = nullptr;     /* Last of selected items */

    model     = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
    rows      = gtk_tree_selection_get_selected_rows (selection, &model);

    row       = g_list_last (rows);

    while (row != nullptr) {
        if (gtk_tree_model_get_iter (model, &iter, row->data)) {
            gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
            }
        row = row->prev;
    }
    g_list_free_full (rows, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move up selected items in TreeView.
 */
void
treeview_move_up (GtkWidget *tree_view)
{
    GtkTreeSelection *selection;       /* TreeSelection */
    GtkTreeModel     *model;           /* TreeModel */
    GtkTreeIter       iter;            /* TreeIter */
    GtkTreeIter       other;           /* TreeIter */
    GList            *rows = nullptr;     /* TreeView selected rows list */
    GList            *row  = nullptr;     /* First selected / temp list */

    model     = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
    rows      = gtk_tree_selection_get_selected_rows (selection, &model);

    if (rows == nullptr)
        return;

    row = g_list_first (rows);

    while (row != nullptr) {

        if (gtk_tree_model_get_iter (model, &iter, row->data)) {

            other = iter;

            if (gtk_tree_model_iter_previous (model, &other)) {
                gtk_list_store_swap (GTK_LIST_STORE (model),
                                     &iter, &other);
            }
            else break;
        }
        row = row->next;
    }
    g_list_free_full (rows, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move down selected items in TreeView.
 */
void
treeview_move_down (GtkWidget *tree_view)
{
    GtkTreeSelection *selection;       /* TreeSelection */
    GtkTreeModel     *model;           /* TreeModel */
    GtkTreeIter       iter;            /* TreeIter */
    GtkTreeIter       other;           /* TreeIter */
    GList            *rows = nullptr;     /* TreeView selected rows list */
    GList            *row  = nullptr;     /* First selected / temp list */

    model     = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
    rows      = gtk_tree_selection_get_selected_rows (selection, &model);

    if (rows == nullptr)
        return;

    row = g_list_last (rows);

    while (row != nullptr) {

        if (gtk_tree_model_get_iter (model, &iter, row->data)) {

            other = iter;

            if (gtk_tree_model_iter_next (model, &other)) {
                gtk_list_store_swap (GTK_LIST_STORE (model),
                                     &iter, &other);
            }
            else break;
        }
        row = row->prev;
    }
    g_list_free_full (rows, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create treeview for image list.
 */
GtkWidget *
create_tview (void)
{
    GtkWidget         *tree_view;  /* Widget to return */
    GtkCellRenderer   *renderer;   /* CellRenderer*/
    GtkListStore      *store;      /* ListStore*/
    GtkTreeSelection  *selection;  /* TreeSelection*/
    GtkTreeViewColumn *column;     /* TreeViewColumn*/

    tree_view = gtk_tree_view_new ();
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
    gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);

    renderer = gtk_cell_renderer_text_new ();
    column   = gtk_tree_view_column_new_with_attributes ("File name",
                                                         renderer,
                                                         "text",
                                                         COL_FILE_NAME,
                                                         nullptr);
    gtk_tree_view_column_set_sort_column_id (column, COL_FILE_NAME);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view), column, -1);

    renderer = gtk_cell_renderer_text_new ();
    column   = gtk_tree_view_column_new_with_attributes ("Dimensions",
                                                         renderer,
                                                         "text",
                                                         COL_WIDTH_HEIGHT,
                                                         nullptr);
    gtk_tree_view_column_set_sort_column_id (column, COL_WIDTH_HEIGHT);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view), column, -1);

    renderer = gtk_cell_renderer_text_new ();
    column   = gtk_tree_view_column_new_with_attributes ("Location",
                                                         renderer,
                                                         "text",
                                                         COL_FILE_DIR,
                                                         nullptr);
    gtk_tree_view_column_set_sort_column_id (column, COL_FILE_DIR);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view), column, -1);

    gtk_tree_view_set_search_column (GTK_TREE_VIEW (tree_view), COL_FILE_NAME);

    store = gtk_list_store_new (NUM_COLS,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_INT,
                                G_TYPE_INT);

    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view),
                             GTK_TREE_MODEL (store));

    g_object_unref (store);

    return tree_view;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get one file path from wallpaper list.
 */
char *
treeview_get_one_file (GtkWidget *tree_view)
{
    GtkTreeModel *model;        /* TreeModel */
    GtkTreeIter   iter;         /* TreeIter */
    int           count = 0;    /* Number of items */
    int           pick  = 0;    /* Random item number */
    char         *file  = nullptr; /* File name to return */

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    count = gtk_tree_model_iter_n_children (model, nullptr);
    pick  = g_random_int_range (0, count);

    if (gtk_tree_model_iter_nth_child (model, &iter, nullptr, pick))
        gtk_tree_model_get (model, &iter, COL_FILE_PATH, &file, -1);
    return file;
}
/*----------------------------------------------------------------------------*/

