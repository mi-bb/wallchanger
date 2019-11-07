/**
 * @file  treev.c
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
#include "treev.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set data in GtkListStore.
 *
 * Sets data from ii_info ImageInfo to gls_list GtkListStore row pointed
 * by gti_iter GtkTreeIter.
 *
 * @param[out]  gls_list GtkListStore to set data
 * @param[in]   gti_iter GtkTreeIter row adress
 * @param[in]   ii_info  Data in ImageInfo object
 * @return      none
 */
static void
liststore_set_item (GtkListStore    *gls_list,
                    GtkTreeIter     *gti_iter,
                    const ImageInfo *ii_info)
{
    gtk_list_store_set (gls_list, gti_iter,
                        COL_FULL_FILE_NAME, ii_info->s_full_path,
                        COL_FILE_NAME,      ii_info->s_file_name,
                        COL_FILE_PATH,      ii_info->s_file_path,
                        COL_WIDTH_HEIGHT,   ii_info->s_width_height,
                        COL_WIDTH,          ii_info->i_width,
                        COL_HEIGHT,         ii_info->i_height,
                        -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert single data item to GtkListStore.
 *
 * @param[out]  gw_list  GtkWidget with GtkListStore to insert data
 * @param[in]   ii_info  data in ImageInfo object
 * @return      none
 */
static void
liststore_add_item (GtkWidget       *gw_list,
                    const ImageInfo *ii_info)
{
    GtkListStore *gls_store;
    GtkTreeIter   gti_iter;

    gls_store = GTK_LIST_STORE (gtk_tree_view_get_model (
                GTK_TREE_VIEW (gw_list) ) );
    gtk_list_store_append (gls_store, &gti_iter);
    liststore_set_item (gls_store, &gti_iter, ii_info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace data in TreeView.
 *
 * @param[out]  gw_tview     TreeView in which data should be replaced
 * @param[in]   gsl_iinfo1   List with ImageInfo objects with data.
 * @return      none
 */
static void
treeview_replace_data (GtkWidget *gw_tview,
                       GSList    *gsl_iinfo1)
{
    GSList       *gsl_iinfo = NULL;
    GtkTreeModel *gtm_model;
    GtkListStore *gls_list;
    GtkTreeIter   gti_iter;
    uint32_t      ui_icnt   = 0; /* ItemInfo list items count */
    uint32_t      ui_mcnt   = 0; /* TreeModel items count */
    gboolean      b_res     = FALSE;

    gsl_iinfo = g_slist_copy (gsl_iinfo1);

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    gls_list = GTK_LIST_STORE (gtm_model);
    ui_icnt = g_slist_length (gsl_iinfo);
    ui_mcnt = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (gls_list), NULL);
    
    if (ui_icnt == ui_mcnt) {
        b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);
        while (b_res && gsl_iinfo != NULL) {
            liststore_set_item (gls_list, &gti_iter, gsl_iinfo->data);
            gsl_iinfo = gsl_iinfo->next;
            b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
        }
    }
    g_slist_free (gsl_iinfo);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert multiple data items to GtkListStore.
 *
 * @param[out]  gw_list     GtkWidget with GtkListStore to insert items
 * @param[in]   gsl_iinfo1  GSList list with data in ImageInfo format
 * @return      none
 */
void
liststore_add_items (GtkWidget  *gw_list,
                     GSList     *gsl_iinfo1)
{
    GSList *gsl_iinfo = NULL; /* Temp ItemInfo list */

    gsl_iinfo = g_slist_copy (gsl_iinfo1);

    while (gsl_iinfo != NULL) {
        liststore_add_item (gw_list, gsl_iinfo->data);
        gsl_iinfo = gsl_iinfo->next;
    }
    g_slist_free (gsl_iinfo);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from GtkTreeModel to ImageInfo object.
 *
 * @param[in]  gtm_model GtkTreeModel with data to get
 * @param[in]  gti_iter  GtkTreeIter with row adress
 * @return     ImageInfo item with readed data
 */
ImageInfo *
treemodel_get_data (GtkTreeModel *gtm_model,
                    GtkTreeIter   gti_iter)
{
    ImageInfo *ii_info = g_malloc (sizeof (ImageInfo));

    gtk_tree_model_get (gtm_model, &gti_iter,
            COL_FULL_FILE_NAME, &ii_info->s_full_path, 
            COL_FILE_NAME,      &ii_info->s_file_name,
            COL_FILE_PATH,      &ii_info->s_file_path,
            COL_WIDTH_HEIGHT,   &ii_info->s_width_height,
            COL_WIDTH,          &ii_info->i_width,
            COL_HEIGHT,         &ii_info->i_height, -1);
    return ii_info;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find file on list and select it
 *
 * @param[in]   gw_tview  GtkTreeView on which data should be selected
 * @param[in]   s_file    File path to find
 * @return      none
 */
void
find_select_item (GtkWidget  *gw_tview,
                  const char *s_file)
{
    GtkTreeModel     *gtm_model;
    GtkTreeSelection *gts_sele;
    GtkTreeIter       gti_iter;
    gboolean          b_res     = FALSE;
    GValue            value     = {0,};

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);
    while (b_res) {
        gtk_tree_model_get_value(gtm_model, &gti_iter,
                                 COL_FULL_FILE_NAME, &value);
        const char *s_val = (const char*) g_value_get_string(&value);
        if (compare_strings (s_file, s_val) == 0) {
            gts_sele = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview)); gtk_tree_selection_select_iter (gts_sele, &gti_iter);
            break;
        }
        g_value_unset(&value);
        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove selected items from TreeView
 *
 * @param[out]  gw_tview  GtkTreeView from which data whould be removed
 * @return      none
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
    gts_sele = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview));
    gl_list = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);
    gl_list1 = g_list_last (gl_list);

    while (gl_list1 != NULL) {
        if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gl_list1->data)) {
            gtk_list_store_remove (GTK_LIST_STORE (gtm_model), &gti_iter);
            }
        gl_list1 = gl_list1->prev;
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
    g_list_free (gl_list1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data out of TreeView.
 *
 * @param[in]  gw_tview   TreeView from which data should be taken
 * @return     List with ImageInfo data of TreeView's TreeModel items
 */
GSList *
treeview_get_data (GtkWidget *gw_tview)
{
    GSList       *gsl_iinfo = NULL; /* ImageInfo return list */
    GtkTreeModel *gtm_model;
    GtkTreeIter   gti_iter;
    gboolean      b_res     = FALSE;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (gw_tview));
    b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);
    while (b_res) {
        ImageInfo *ii_info = treemodel_get_data (gtm_model, gti_iter);
        gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);
        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort data in TreeView.
 *
 * @param[in,out]  gw_tview  TreeView in which data should be sorted
 * @return         none
 */
void
treeview_sort_list (GtkWidget *gw_tview)
{
    GSList *gsl_files1 = NULL; /* TreeView item list */
    GSList *gsl_files  = NULL; /* TreeView item list temp copy */

    gsl_files1 = treeview_get_data (gw_tview);
    gsl_files = g_slist_copy (gsl_files1);
    gsl_files = g_slist_sort (gsl_files, (GCompareFunc) compare_imageitems);
    treeview_replace_data (gw_tview, gsl_files);
    g_slist_free_full (gsl_files1, (GDestroyNotify) imageinfo_free);
    g_slist_free (gsl_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move up selected items in TreeView.
 *
 * @param[in,out]  gw_tview  TreeView to move data up
 * @return         none
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
    gts_sele = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview));
    gl_list = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

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
 *
 * @param[in,out]  gw_tview  TreeView to move data down
 * @return         none
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
    gts_sele = gtk_tree_view_get_selection (GTK_TREE_VIEW (gw_tview));
    gl_list = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

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
 *
 * @param[out]  gw_tview  Pointer to Treeview to set
 * @return      none
 */
void
create_tview (GtkWidget **gw_tview)
{
    GtkCellRenderer  *gcr_render;
    GtkListStore     *gls_list;
    GtkTreeSelection *gts_sele;

    *gw_tview = gtk_tree_view_new ();
    gts_sele = gtk_tree_view_get_selection (GTK_TREE_VIEW (*gw_tview));
    gtk_tree_selection_set_mode (gts_sele, GTK_SELECTION_MULTIPLE);
    gcr_render = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (*gw_tview),
                                                 -1,
                                                 "File name",
                                                 gcr_render,
                                                 "text",
                                                 COL_FILE_NAME,
                                                 NULL);
    gcr_render = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (*gw_tview),
                                                 -1,
                                                 "Dim",
                                                 gcr_render,
                                                 "text",
                                                 COL_WIDTH_HEIGHT,
                                                 NULL);
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

