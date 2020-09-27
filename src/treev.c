/**
 * @file  treev.c
 * @copyright Copyright (C) 2019-2020 Michał Bąbik
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
#include <stdint.h>
#include "strfun.h"
#include "treev.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  TreeView Columns
 */
enum e_tree_view_columns {
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
 * @fn  static void liststore_set_item (GtkListStore    *gls_list,
 *                                      GtkTreeIter     *gti_iter,
 *                                      const ImageInfo *ii_info)
 *
 * @brief  Set data in GtkListStore.
 *
 * Sets data from ii_info ImageInfo to gls_list GtkListStore row pointed
 * by gti_iter GtkTreeIter.
 *
 * @param[out] gls_list GtkListStore to set data
 * @param[in]  gti_iter GtkTreeIter row adress
 * @param[in]  ii_info  Data in ImageInfo object
 * @return     none
 *
 * @fn  static void treeview_add_item (GtkWidget       *gw_tview,
 *                                     const ImageInfo *ii_info)
 *
 * @brief  Insert single data item to GtkListStore.
 *
 * @param[out] gw_tview    TreeView in which data should be added
 * @param[in]  ii_info  data in ImageInfo object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void liststore_set_item    (GtkListStore    *gls_list,
                                   GtkTreeIter     *gti_iter,
                                   const ImageInfo *ii_info);

static void treeview_add_item     (GtkWidget       *gw_tview,
                                   const ImageInfo *ii_info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set data in GtkListStore.
 */
static void
liststore_set_item (GtkListStore    *gls_list,
                    GtkTreeIter     *gti_iter,
                    const ImageInfo *ii_info)
{
    gtk_list_store_set (gls_list, gti_iter,
                        COL_FULL_FILE_NAME, imageinfo_get_full_name (ii_info),
                        COL_FILE_NAME,      imageinfo_get_file_name (ii_info),
                        COL_FILE_PATH,      imageinfo_get_file_path (ii_info),
                        COL_WIDTH_HEIGHT,   imageinfo_get_wxh       (ii_info),
                        COL_WIDTH,          imageinfo_get_width     (ii_info),
                        COL_HEIGHT,         imageinfo_get_height    (ii_info),
                        -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert single data item to GtkListStore.
 */
static void
treeview_add_item (GtkWidget       *gw_tview,
                   const ImageInfo *ii_info)
{
    GtkListStore *gls_store;
    GtkTreeIter   gti_iter;

    gls_store = GTK_LIST_STORE (gtk_tree_view_get_model (
                GTK_TREE_VIEW (gw_tview)));

    gtk_list_store_append (gls_store, &gti_iter);
    liststore_set_item (gls_store, &gti_iter, ii_info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkTreeView.
 */
void
treeview_add_items_glist (GtkWidget   *gw_tview,
                          const GList *gl_files)
{
    const GList *gl_fl   = NULL; /* Temp ItemInfo list */
    const char  *s_fn    = NULL; /* Strng for file path */
    ImageInfo   *ii_info;        /* Image information */

    gl_fl = gl_files;

    while (gl_fl != NULL) {

        s_fn = (const char *) gl_fl->data;

        if ((ii_info = imageinfo_new_from_file (s_fn)) != NULL) {

            treeview_add_item (gw_tview, ii_info);
            imageinfo_free (ii_info);
        }
        gl_fl = gl_fl->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkTreeView.
 */
void
treeview_add_items_gslist (GtkWidget    *gw_tview,
                           const GSList *gl_files)
{
    const GSList *gl_fl   = NULL; /* Temp ItemInfo list */
    const char   *s_fn    = NULL; /* Strng for file path */
    ImageInfo    *ii_info;        /* Image information */

    gl_fl = gl_files;

    while (gl_fl != NULL) {

        s_fn = (const char *) gl_fl->data;

        if ((ii_info = imageinfo_new_from_file (s_fn)) != NULL) {
            treeview_add_item (gw_tview, ii_info);
            imageinfo_free (ii_info);
        }
        gl_fl = gl_fl->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkTreeView.
 */
void
treeview_add_items_setting (GtkWidget *gw_tview,
                            Setting   *st_wallpapers)
{
    Setting    *st_item;         /* For setting iteration */
    const char *s_fn     = NULL; /* File name to process */
    ImageInfo  *ii_info;         /* ImageInfo wallpaper info */

    st_item = st_wallpapers;

    while (st_item != NULL) {
        if ((s_fn = setting_get_string (st_item)) != NULL) {
            if ((ii_info = imageinfo_new_from_file (s_fn)) != NULL) {
                treeview_add_item (gw_tview, ii_info);
                imageinfo_free (ii_info);
            }
        }
        st_item = st_item->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from GtkTreeModel to ImageInfo object.
 */
ImageInfo *
treemodel_get_data (GtkTreeModel *gtm_model,
                    GtkTreeIter   gti_iter)
{
    ImageInfo *ii_info = imageinfo_new (); 

    gtk_tree_model_get (gtm_model, &gti_iter,
            COL_FULL_FILE_NAME, &ii_info->s_full_path, 
            COL_FILE_NAME,      &ii_info->s_file_name,
            COL_FILE_PATH,      &ii_info->s_file_path,
            COL_WIDTH_HEIGHT,   &ii_info->s_width_height,
            COL_WIDTH,          &ii_info->i_width,
            COL_HEIGHT,         &ii_info->i_height,
            -1);
    return ii_info;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data out of TreeView.
 */
GSList *
treeview_get_data (GtkWidget *gw_tview)
{
    GSList       *gsl_iinfo = NULL; /* ImageInfo return list */
    GtkTreeModel *gtm_model;
    ImageInfo    *ii_info;
    GtkTreeIter   gti_iter;
    gboolean      b_res     = FALSE;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    b_res     = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {
        ii_info = treemodel_get_data (gtm_model, gti_iter);
        gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);

        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
void
treeview_get_setting_data (GtkWidget *gw_tview,
                           Setting   *st_wallpapers)
{
    GtkTreeModel *gtm_model;
    ImageInfo    *ii_info;
    GtkTreeIter   gti_iter;
    gboolean      b_res     = FALSE;
    Setting      *st_data;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    b_res     = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {
        ii_info = treemodel_get_data (gtm_model, gti_iter);
        st_data = setting_new_string (NULL, imageinfo_get_full_name (ii_info));
        setting_add_child (st_wallpapers, st_data);
        imageinfo_free (ii_info);

        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find file on list and select it
 */
void
treeview_find_select_item (GtkWidget  *gw_tview,
                           const char *s_file)
{
    GtkTreeModel     *gtm_model;
    GtkTreePath      *path;
    GtkTreeIter       gti_iter;
    gboolean          b_res     = FALSE;
    GValue            value     = {0,};
    const char       *s_val     = NULL;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    b_res     = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {
        gtk_tree_model_get_value (gtm_model, &gti_iter,
                                  COL_FULL_FILE_NAME, &value);

        s_val = (const char*) g_value_get_string (&value);

        if (str_compare (s_file, s_val) == 0) {
            path = gtk_tree_model_get_path (gtm_model, &gti_iter);
            gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (gw_tview), path, NULL,
                    TRUE, 0.5, 0);
            gtk_tree_view_set_cursor (GTK_TREE_VIEW (gw_tview),
                                      path, NULL, FALSE);
            gtk_tree_path_free (path);
            break;
        }
        g_value_unset(&value);
        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove duplicates from TreeView file list.
 */
void
treeview_remove_duplicates (GtkWidget *gw_tview)
{
    GtkTreeModel   *gtm_model;
    GtkTreeIter     gti_iter;
    GtkTreeIter     gti_next;
    GtkTreeIter     gti_act;
    gboolean        b_res     = FALSE;
    gboolean        b_res2    = FALSE;
    GValue          value     = {0,};
    GValue          value2    = {0,};
    const char     *s_val     = NULL;
    const char     *s_val2    = NULL;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    b_res     = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {

        gti_next = gti_iter;
        b_res2 = gtk_tree_model_iter_next (gtm_model, &gti_next);

        gtk_tree_model_get_value(gtm_model, &gti_iter,
                                 COL_FULL_FILE_NAME, &value);
        s_val = (const char*) g_value_get_string(&value);

        while (b_res2) {

            gti_act = gti_next;
            b_res2 = gtk_tree_model_iter_next (gtm_model, &gti_next);

            gtk_tree_model_get_value(gtm_model, &gti_act,
                                     COL_FULL_FILE_NAME, &value2);
            s_val2 = (const char*) g_value_get_string(&value2);

            if (str_compare (s_val, s_val2) == 0) {

                gtk_list_store_remove (GTK_LIST_STORE (gtm_model), &gti_act);
            }
            g_value_unset(&value2);
        }
        g_value_unset(&value);
        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove selected items from TreeView
 */
void
treeview_remove_selected (GtkWidget *gw_tview)
{
    GList            *gl_list  = NULL;
    GList            *gl_list1 = NULL;
    GtkTreeSelection *gts_sele;
    GtkTreeModel     *gtm_model;
    GtkTreeIter       gti_iter;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    gts_sele  = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview));
    gl_list   = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

    gl_list1  = g_list_last (gl_list);

    while (gl_list1 != NULL) {
        if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gl_list1->data)) {
            gtk_list_store_remove (GTK_LIST_STORE (gtm_model), &gti_iter);
            }
        gl_list1 = gl_list1->prev;
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move up selected items in TreeView.
 */
void
treeview_move_up (GtkWidget *gw_tview)
{
    GList            *gl_list  = NULL; /* TreeView selected rows list */
    GList            *gl_list1 = NULL; /* First selected / temp list */
    GtkTreeSelection *gts_sele;
    GtkTreeModel     *gtm_model;
    GtkTreeIter       gti_iter;
    GtkTreeIter       gti_itern;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    gts_sele  = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview));
    gl_list   = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

    if (gl_list == NULL)
        return;

    gl_list1 = g_list_first (gl_list);

    while (gl_list1 != NULL) {

        if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gl_list1->data)) {

            gti_itern = gti_iter;

            if (gtk_tree_model_iter_previous (gtm_model, &gti_itern)) {
                gtk_list_store_swap (GTK_LIST_STORE (gtm_model),
                                     &gti_iter, &gti_itern);
            }
            else break;
        }
        gl_list1 = gl_list1->next;
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move down selected items in TreeView.
 */
void
treeview_move_down (GtkWidget *gw_tview)
{
    GList            *gl_list  = NULL; /* TreeView selected rows list */
    GList            *gl_list1 = NULL; /* First selected / temp list */
    GtkTreeSelection *gts_sele;
    GtkTreeModel     *gtm_model;
    GtkTreeIter       gti_iter;
    GtkTreeIter       gti_itern;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    gts_sele  = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview));
    gl_list   = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

    if (gl_list == NULL)
        return;

    gl_list1 = g_list_last (gl_list);

    while (gl_list1 != NULL) {

        if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gl_list1->data)) {

            gti_itern = gti_iter;

            if (gtk_tree_model_iter_next (gtm_model, &gti_itern)) {
                gtk_list_store_swap (GTK_LIST_STORE (gtm_model),
                                     &gti_iter, &gti_itern);
            }
            else break;
        }
        gl_list1 = gl_list1->prev;
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create treeview for image list.
 */
void
create_tview (GtkWidget **gw_tview)
{
    GtkCellRenderer   *gcr_render;
    GtkListStore      *gls_list;
    GtkTreeSelection  *gts_sele;
    GtkTreeViewColumn *gtvc_col;

    *gw_tview = gtk_tree_view_new ();
    gts_sele  = gtk_tree_view_get_selection (GTK_TREE_VIEW (*gw_tview));
    gtk_tree_selection_set_mode (gts_sele, GTK_SELECTION_MULTIPLE);

    gcr_render = gtk_cell_renderer_text_new ();
    gtvc_col   = gtk_tree_view_column_new_with_attributes ("File name",
                                                           gcr_render,
                                                           "text",
                                                           COL_FILE_NAME,
                                                           NULL);
    gtk_tree_view_column_set_sort_column_id (gtvc_col, COL_FILE_NAME);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (*gw_tview), gtvc_col, -1);

    gcr_render = gtk_cell_renderer_text_new ();
    gtvc_col   = gtk_tree_view_column_new_with_attributes ("Dimensions",
                                                           gcr_render,
                                                           "text",
                                                           COL_WIDTH_HEIGHT,
                                                           NULL);
    gtk_tree_view_column_set_sort_column_id (gtvc_col, COL_WIDTH_HEIGHT);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (*gw_tview), gtvc_col, -1);
    gcr_render = gtk_cell_renderer_text_new ();
    gtvc_col   = gtk_tree_view_column_new_with_attributes ("Location",
                                                           gcr_render,
                                                           "text",
                                                           COL_FILE_PATH,
                                                           NULL);
    gtk_tree_view_column_set_sort_column_id (gtvc_col, COL_FILE_PATH);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (*gw_tview), gtvc_col, -1);

    gls_list = gtk_list_store_new (NUM_COLS,
                                   G_TYPE_STRING,
                                   G_TYPE_STRING,
                                   G_TYPE_STRING,
                                   G_TYPE_STRING,
                                   G_TYPE_INT,
                                   G_TYPE_INT);

    gtk_tree_view_set_model (GTK_TREE_VIEW (*gw_tview),
                             GTK_TREE_MODEL (gls_list));

    g_object_unref(gls_list);
}
/*----------------------------------------------------------------------------*/

