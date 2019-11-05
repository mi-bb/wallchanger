/** 
 * @file  wchanger.c
 * @copyright Copyright (C) 2019 Michał Bąbik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @brief Wallpaper change dialog
 *
 * Program to change wallpapers.
 *
 * @date November 4, 2019
 *
 * @version 1.0.1
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h> 
#include <dirent.h> 
#include <gtk/gtk.h>
#include "setts.h"
#include "iminfo.h"
#include "wallset.h"
#include "imgs.h"
#include "dlgs.h"
#include "treev.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Structore to pass widgets and settings to callback
 */
typedef struct
DialogData {
    GtkWindow  *gw_window;      /**< App window pointer */
    GtkWidget  *gw_view;        /**< TreeView file list widget */
    GtkWidget  *gw_random;      /**< Random background select check box */
    GtkWidget  *gw_lastused;    /**< Set last used wallpaper check box */
    GtkWidget  *gw_command;     /**< Set wallpaper command entry */
    GtkWidget  *gw_interval;    /**< Set wallpaper command entry */
    WallSett   *ws_sett;        /**< Program settings */
} DialogData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of GdkPixbuf format extensions.
 *
 * @return    List of strings with extensions for GdkPixbuf graphics.
 */
static GSList *
get_pbuf_extension_list (void)
{
    GdkPixbufFormat  *gpf          = NULL;
    GSList           *gsl_res      = NULL;
    GSList           *gsl_formats  = NULL;
    GSList           *gsl_formats1 = NULL;
    char            **exts         = NULL;
    char            **it           = NULL;

    gsl_formats1 = gdk_pixbuf_get_formats();
    gsl_formats = g_slist_copy (gsl_formats1);
    while (gsl_formats != NULL) {
        gpf = gsl_formats->data;
        exts = gdk_pixbuf_format_get_extensions (gpf);
        for (it = exts; *it != NULL; it++) {
            gsl_res = g_slist_append (gsl_res, g_strdup (*it));
        }
        g_strfreev (exts);
        gsl_formats = gsl_formats->next;
    }
    g_slist_free (gsl_formats1);
    g_slist_free (gsl_formats);
    return gsl_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file extenstion.
 *
 * @param[in] s_fn String with file path
 * @return    String with file extension
 */
static char *
get_file_ext (char *s_fn)
{
    char *s_ext = NULL;
    char *s_p   = NULL;

    s_p = strrchr (s_fn, '.');
    if (s_p != NULL) {
        s_p++;
        s_ext = g_strdup (s_p);
    }
    return s_ext;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check list with file names for GdkPixbuf images.
 *
 * @param[in] gsl_files1 List with files to proccess
 * @return    List with files that may contain GdkPixbuf images
 */
static GSList *
check_files_for_pixbuf (GSList *gsl_files1)
{
    GSList  *gsl_res   = NULL;
    GSList  *gsl_files = NULL;
    GSList  *gsl_exts  = NULL;
    char    *s_fn      = NULL;
    char    *s_ext     = NULL;

    gsl_exts = get_pbuf_extension_list ();
    gsl_files = g_slist_copy (gsl_files1);

    while (gsl_files != NULL) {
        s_fn = g_strdup ((char*) gsl_files->data);
        s_ext = get_file_ext (s_fn);
        if (s_ext != NULL) {
            GSList *gsl_fnd = g_slist_find_custom (gsl_exts, s_ext, 
                    (GCompareFunc) compare_strings);
            if (gsl_fnd != NULL) {
                gsl_res = g_slist_append (gsl_res, s_fn);
            g_free (s_ext);
            }
        }
        gsl_files = gsl_files->next;
    }
    g_slist_free_full (gsl_exts, g_free);
    return gsl_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check list with file names for GdkPixbuf images.
 *
 * @param[in] s_path1 Path to directory
 * @return    List with files in directory
 */
static GSList *
get_directory_content (const char *s_path1)
{
    GSList *gsl_files = NULL;
    char   *s_pthfn   = NULL;
    char   *s_path    = NULL;
    char   *s_fn      = NULL;
    char   *s_p       = NULL;
    int     i_dlen    = 0;
    int     i_pos     = 0;
    DIR    *dr;
    struct  dirent *de;

    i_dlen = strlen (s_path1);
    s_path = g_malloc0 ((i_dlen + 1) * sizeof (char));
    strcpy (s_path, s_path1);
    s_p = strrchr (s_path, '/');

    if (s_p != NULL) {
        i_pos = (int) (s_p - s_path);
        if (i_pos != i_dlen - 1) {
            i_dlen++;
            s_path = g_realloc (s_path, (i_dlen + 1) * sizeof (char));
            strcat (s_path, "/");
        }
    }
    dr = opendir (s_path); 
    if (dr == NULL) {
        printf ("Could not open current directory\n"); 
        g_free (s_path);
        return NULL; 
    } 
    while ((de = readdir(dr)) != NULL) 
        //if (de->d_type == DT_REG) {
        if (de->d_type == 8) {
            s_fn = g_strdup (de->d_name);
            s_pthfn = g_malloc0 ( 
                    (i_dlen + strlen (s_fn) + 1) * sizeof (char));
            strcpy (s_pthfn, s_path);
            strcat (s_pthfn, s_fn);
            gsl_files = g_slist_append (gsl_files, s_pthfn);
            g_free (s_fn);
        }
    g_free (s_path);
    closedir(dr);     
    return gsl_files;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add images button pressed.
 *
 * @param          widget   The object on which the signal is emitted
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         none
 */
static void
event_add_images_pressed (GtkWidget  *widget,
                          DialogData *dd_data)
{
    GSList *gsl_files = NULL;
    GSList *gsl_iinfo = NULL;

    gsl_files = add_images_dialog (dd_data->gw_window);
    gsl_iinfo = file_paths_to_imageinfo (gsl_files);

    liststore_add_items (dd_data->gw_view, gsl_iinfo);
    g_slist_free_full (gsl_files, g_free);
    g_slist_free_full (gsl_iinfo, (GDestroyNotify) imageinfo_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add images from folder button pressed.
 *
 * @param          widget   The object on which the signal is emitted
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         none
 */
static void
event_add_images_folder_pressed (GtkWidget  *widget,
                                 DialogData *dd_data)
{
    GSList *gsl_files1 = NULL;
    GSList *gsl_files2 = NULL;
    char   *s_folder   = NULL;

    s_folder = add_images_folder_dialog (dd_data->gw_window);

    if (s_folder != NULL) {
        gsl_files1 = get_directory_content (s_folder);
        gsl_files2 = check_files_for_pixbuf (gsl_files1);
        g_slist_free_full (gsl_files1, g_free);
        gsl_files1 = file_paths_to_imageinfo (gsl_files2);
        liststore_add_items (dd_data->gw_view, gsl_files1);
        g_slist_free_full (gsl_files1, (GDestroyNotify) imageinfo_free);
        g_slist_free_full (gsl_files2, g_free);
        g_free (s_folder);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove items from list pressed.
 *
 * @param          widget    The object on which the signal is emitted
 * @param[in,out]  gw_tview  TreeView to remove items
 * @return         none
 */
static void
event_remove_from_list_pressed (GtkWidget *widget,
                                GtkWidget *gw_tview)
{
    treeview_remove_selected (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move up items pressed.
 *
 * @param          widget    The object on which the signal is emitted
 * @param[in,out]  gw_tview  TreeView to move items up
 * @return         none
 */
static void
event_move_up_pressed (GtkWidget *widget,
                       GtkWidget *gw_tview)
{
    treeview_move_up (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move down items pressed.
 *
 * @param          widget    The object on which the signal is emitted
 * @param[in,out]  gw_tview  TreeView to move items down
 * @return         none
 */
static void
event_move_down_pressed (GtkWidget *widget,
                         GtkWidget *gw_tview)
{
    treeview_move_down (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort wallpaper list button pressed.
 *
 * @param          widget    The object on which the signal is emitted
 * @param[in,out]  gw_tview  TreeView to sort items
 * @return         none
 */
static void
event_sort_list_pressed (GtkWidget *widget,
                         GtkWidget *gw_tview)
{
    treeview_sort_list (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper button pressed.
 *
 * @param          widget   The object on which the signal is emitted
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         none
 */
static void
event_set_wallpaper_pressed (GtkWidget  *widget,
                             DialogData *dd_data)
{
    GList            *gl_list  = NULL;
    GtkTreeSelection *gts_sele;
    GtkTreeModel     *gtm_model;
    GtkTreeIter       gti_iter;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (dd_data->gw_view));
    gts_sele = gtk_tree_view_get_selection (GTK_TREE_VIEW (dd_data->gw_view));
    gl_list = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

    if (gl_list == NULL)
        return;

    if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gl_list->data)) {
        ImageInfo *ii_info = treemodel_get_data (gtm_model, gti_iter);
        printf ("%s\n", ii_info->s_full_path);

        if (wallpaper_dialog_set (dd_data->ws_sett,ii_info->s_full_path) > 0) {
            printf ("Some error occured\n");
        }
        imageinfo_free (ii_info);
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loading data from DialogData object to program window.
 *
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         none
 */
static void
load_settings (DialogData *dd_data)
{
    GSList *gsl_iinfo = file_paths_to_imageinfo (dd_data->ws_sett->gsl_files);

    liststore_add_items (dd_data->gw_view, gsl_iinfo);

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_random),
                                  dd_data->ws_sett->i_random);

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_lastused),
                                  dd_data->ws_sett->i_lastsett);

    gtk_spin_button_set_value (GTK_SPIN_BUTTON (dd_data->gw_interval),
            (double) dd_data->ws_sett->i_chinterval);

    gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command),
                        dd_data->ws_sett->s_bgcmd);

    g_slist_free_full (gsl_iinfo, (GDestroyNotify) imageinfo_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read settings from widgets and store them in WallSett object.
 *
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         none
 */
static void
gather_settings (DialogData *dd_data)
{
    GSList *gsl_iinfo1 = treeview_get_data (dd_data->gw_view);
    g_slist_free_full (dd_data->ws_sett->gsl_files, g_free);
    dd_data->ws_sett->gsl_files = imageinfo_to_file_paths (gsl_iinfo1);
    g_slist_free_full (gsl_iinfo1, (GDestroyNotify) imageinfo_free);

    dd_data->ws_sett->i_lastsett = (uint8_t) gtk_toggle_button_get_active (
            GTK_TOGGLE_BUTTON (dd_data->gw_lastused));
    dd_data->ws_sett->i_random = (uint8_t) gtk_toggle_button_get_active (
            GTK_TOGGLE_BUTTON (dd_data->gw_random));
    dd_data->ws_sett->i_chinterval = (uint32_t) gtk_spin_button_get_value (
            GTK_SPIN_BUTTON (dd_data->gw_interval));

    const char *s_cmd = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));
    settings_set_command (dd_data->ws_sett, s_cmd);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save settings button pressed.
 *
 * @param          widget   The object on which the signal is emitted
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return none
 */
static void
event_save_settings_pressed (GtkWidget  *widget,
                             DialogData *dd_data)
{
    gather_settings (dd_data);
    settings_write (dd_data->ws_sett);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make pixbuf preview of image from path.
 *
 * @param[in]  s_fname   Image path
 * @param[in]  i_max_wh  Max preview width/height 
 * @return     Pixbuf with preview image.
 */
static GdkPixbuf *
make_image_preview (const char *s_fname,
                    const int   i_max_wh)
{
    GdkPixbuf *gp_prev        = NULL;
    GdkPixbuf *gp_prev_scaled = NULL;
    float      f_w            = 0;
    float      f_h            = 0;
    float      f_nw           = 0;
    float      f_nh           = 0;

    gp_prev = gdk_pixbuf_new_from_file (s_fname, NULL);

    if (gp_prev != NULL) {
        f_w = (float) gdk_pixbuf_get_width (gp_prev);
        f_h = (float) gdk_pixbuf_get_height (gp_prev);
        if (f_w > f_h) {
            f_nw = (float) i_max_wh; 
            f_nh = f_h / f_w * f_nw;
        }
        else {
            f_nh = (float) i_max_wh; 
            f_nw = f_w / f_h * f_nh;
        }
        gp_prev_scaled = gdk_pixbuf_scale_simple (gp_prev,
                (int) f_nw, (int) f_nh, GDK_INTERP_HYPER);
    }
    g_object_unref (gp_prev);
    return gp_prev_scaled;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 *
 * @param[in]   s_file       File to make preview image
 * @param[out]  gw_img_prev  Preview image widget
 * @return      none
 */
static void
preview_from_file (const char *s_file,
                   GtkWidget  *gw_img_prev)
{
    GdkPixbuf *gp_prev = make_image_preview (s_file, 150);
    if (gp_prev != NULL) {
        gtk_image_set_from_pixbuf (GTK_IMAGE (gw_img_prev), gp_prev);
        g_object_unref (gp_prev);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 *
 * @param[in]   tree_view    The object on which the signal is emitted
 * @param[in]   path         The GtkTreePath for the activated row
 * @param       column       The GtkTreeViewColumn in which the activation
 *                           occurred
 * @param[out]  gw_img_prev  Preview image widget
 * @return none
 */
static void
event_img_list_activated (GtkTreeView       *tree_view,
                          GtkTreePath       *path,
                          GtkTreeViewColumn *column,
                          GtkWidget         *gw_img_prev)
{
    GtkTreeModel *gtm_model;
    GtkTreeIter   gti_iter;

    gtm_model = gtk_tree_view_get_model (tree_view);

    if (gtk_tree_model_get_iter (gtm_model, &gti_iter, path)) {
        ImageInfo *ii_info = treemodel_get_data (gtm_model, gti_iter);
        preview_from_file (ii_info->s_full_path, gw_img_prev);
        imageinfo_free (ii_info);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  On main window delete, check settings.
 *
 * @param[in]  window   Window that received the signal
 * @param      event    Event that triggered signal
 * @param[in]  dd_data  DialogData object with widgets and settings info
 * @return     Stop or propagate event further 
 */
static gboolean
event_on_delete (GtkWidget  *window,
                 GdkEvent   *event,
                 DialogData *dd_data)
{
    GtkWidget *dialog;
    int        i_res2 = 0;
    int        i_res  = 0;

    gather_settings (dd_data);
    i_res = settings_check_changed (dd_data->ws_sett);
    if (i_res == -1) {
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_QUESTION,
                                         GTK_BUTTONS_YES_NO,
            "Settings changed, do you want to save them ?");
        gtk_window_set_title (GTK_WINDOW (dialog), "Settings changed");
        i_res2 = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        if (i_res2 == GTK_RESPONSE_YES) {
            settings_write (dd_data->ws_sett);
        } 
    }
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create top window title text.
 *
 * @param[out]  gw_title_widget  Widget to write data
 * @return      none
 */
static void
create_title_widget (GtkWidget **gw_title_widget)
{
    *gw_title_widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

    GtkWidget *gw_label = gtk_label_new (NULL);
    const char *s_str = "Wall Changer - Program settings";
    const char *s_format = "<span size=\"20000\" weight=\"bold\" \
                          foreground=\"#0099e6\" style=\"italic\">\%s</span>";
    char *s_markup;
    s_markup = g_markup_printf_escaped (s_format, s_str);
    gtk_label_set_markup (GTK_LABEL (gw_label), s_markup);
    g_free (s_markup);

    gtk_box_pack_start (GTK_BOX (*gw_title_widget),
                        gw_label,
                        FALSE, FALSE, 24);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create button with icon/label/hint.
 *
 * @param[in]  s_label  Button label
 * @param[in]  s_hint   Button hint
 * @param[in]  i_but    Icon number
 * @return     Button
 */
static GtkWidget *
create_image_button (const char *s_label,
                     const char *s_hint,
                     const int   i_but)
{
    GtkWidget *gw_btn;
    GtkWidget *gw_img;
    GdkPixbuf *gd_pix;

    gw_btn = gtk_button_new ();

    if (strcmp (s_label, "") != 0)
        gtk_button_set_label (GTK_BUTTON (gw_btn), s_label);
    if (strcmp (s_hint, "") != 0)
        gtk_widget_set_tooltip_text (gw_btn, s_hint);
    if (i_but > 0) {
        gd_pix = get_image (i_but);
        gw_img = gtk_image_new_from_pixbuf (gd_pix);
        gtk_button_set_image (GTK_BUTTON (gw_btn), gw_img);
        g_object_unref (gd_pix);
    }
    return gw_btn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make default empty preview image.
 *
 * @param[out]  gw_img  Pointer to image widget
 * @return      none
 */
static void
create_preview (GtkWidget **gw_img)
{
    GdkPixbuf *gp_prev = gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                                         TRUE,
                                         8,
                                         150,
                                         150);
    if (gp_prev != NULL) {
        *gw_img = gtk_image_new_from_pixbuf (gp_prev);
        g_object_unref (gp_prev);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create side buttons widget.
 *
 * @param[out]     gw_buttons_widget  Pointer to widget where to set buttons
 * @param[in,out]  dd_data            DialogData object with widgets and
 *                                    settings info
 * @return         none
 */
static void
create_buttons_widget (GtkWidget **gw_buttons_widget,
                       DialogData *dd_data)
{
    GtkWidget *gw_button;
    *gw_buttons_widget = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    gw_button = create_image_button ("", "Add images", 2);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_add_images_pressed),
                      dd_data);
    gw_button = create_image_button ("", "Add images from folder", 3);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_add_images_folder_pressed),
                      dd_data);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    gw_button = create_image_button ("", "Remove images", 4);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_remove_from_list_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Move up", 5);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_move_up_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Move down", 6);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_move_down_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Sort images", 7);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_sort_list_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Set wallpaper", 8);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_set_wallpaper_pressed),
                      dd_data);
    gw_button = create_image_button ("", "Save settings", 9);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_save_settings_pressed),
                      dd_data);
    gw_button = create_image_button ("", "Exit app", 1);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (gtk_window_close),
                              dd_data->gw_window);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates widget with settings for wallpaper changing.
 *
 * @param[out]     gw_settings_widget  Pointer to destination widget
 * @param[in,out]  dd_data             DialogData object with settings and
 *                                     widget data
 * @return         none
 */
static void
create_settings_widget (GtkWidget **gw_settings_widget,
                        DialogData *dd_data)
{
    GtkWidget     *gw_button_random;
    GtkWidget     *gw_button_selectlast;
    GtkWidget     *gw_command_entry;
    GtkWidget     *gw_spinbutton;
    GtkAdjustment *ga_adjustment;

    gw_button_random = gtk_check_button_new ();
    gw_button_selectlast = gtk_check_button_new ();

    gw_command_entry = gtk_entry_new ();
    GtkWidget *gw_command_label = gtk_label_new ("Background set command : ");

    ga_adjustment = gtk_adjustment_new (5.0, 1.0, 100.0, 1.0, 5.0, 0.0);
    gw_spinbutton = gtk_spin_button_new (ga_adjustment, 1.0, 0);
    GtkWidget *gw_interval_label = gtk_label_new (
            "Background change interval : ");
    GtkWidget *gw_interval_label2 = gtk_label_new ("minutes");

    gtk_button_set_label (GTK_BUTTON (gw_button_random),
                          "Random wallpaper change");
    gtk_button_set_label (GTK_BUTTON (gw_button_selectlast),
                          "Select last used wallpaper at start");
    dd_data->gw_random = gw_button_random;
    dd_data->gw_lastused = gw_button_selectlast;
    dd_data->gw_command = gw_command_entry;
    dd_data->gw_interval = gw_spinbutton;

    *gw_settings_widget = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (*gw_settings_widget), 8);
    gtk_grid_set_row_spacing (GTK_GRID (*gw_settings_widget), 8);
    gtk_grid_attach (GTK_GRID (*gw_settings_widget), gw_button_random, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_button_selectlast, gw_button_random,
                             GTK_POS_BOTTOM, 1, 1);

    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_command_label, gw_button_random,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_command_entry, gw_command_label,
                             GTK_POS_RIGHT, 1, 1);

    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_interval_label, gw_button_selectlast,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_spinbutton, gw_interval_label,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_interval_label2, gw_spinbutton,
                             GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 *
 * @param[in,out]  app      Pointer to GtkApplication
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         none
 */
static void
activate (GtkApplication *app,
          DialogData     *dd_data)
{
    GtkWidget     *gw_window;
    GtkWidget     *gw_title_widget;
    GtkWidget     *gw_tview;
    GtkWidget     *gw_scroll;
    GtkWidget     *gw_buttons_widget;
    GtkWidget     *gw_box_list_btns;
    GtkWidget     *gw_box_prev;
    GtkWidget     *gw_img_prev;
    GtkWidget     *gw_settings_widget;

    gw_window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (gw_window), "Wall Changer v1.0.1");
    gtk_window_set_default_size (GTK_WINDOW (gw_window), 1024, 768);
    g_signal_connect (gw_window, "delete-event",
                  G_CALLBACK (event_on_delete), dd_data);
    dd_data->gw_window = GTK_WINDOW (gw_window);

    create_title_widget (&gw_title_widget);

    create_preview (&gw_img_prev);

    create_tview (&gw_tview);
    dd_data->gw_view = gw_tview;

    g_signal_connect (gw_tview, "row-activated",
                      G_CALLBACK (event_img_list_activated), gw_img_prev);

    gw_scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (gw_scroll), gw_tview);

    gw_box_list_btns = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

    create_buttons_widget (&gw_buttons_widget, dd_data);

    create_settings_widget (&gw_settings_widget, dd_data);

    gw_box_prev = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_prev),
                        gw_img_prev,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gw_scroll,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gw_buttons_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gw_box_prev,
                        FALSE, FALSE, 4);

    GtkWidget *gw_box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_title_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_box_list_btns,
                        TRUE, TRUE, 4);

    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_settings_widget,
                        FALSE, FALSE, 4);

    gtk_container_set_border_width (GTK_CONTAINER (gw_box_main), 10);
    gtk_container_add (GTK_CONTAINER (gw_window), gw_box_main);

    if (settings_init (dd_data->ws_sett)) {
        g_application_quit (G_APPLICATION (app));
        return;
    }

    if (settings_read (dd_data->ws_sett) != 0) {
        g_application_quit (G_APPLICATION (app));
        return;
    }

    load_settings (dd_data);

    if (dd_data->ws_sett->s_lastused != NULL) {
        preview_from_file (dd_data->ws_sett->s_lastused, gw_img_prev);
    }

    find_select_item (gw_tview, dd_data->ws_sett->s_lastused);

    gtk_window_set_application (GTK_WINDOW (gw_window), GTK_APPLICATION (app));
    gtk_widget_show_all (gw_window);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out]  dd_data  Pointer to DialogData with all settings
 * @return      none
 */
static void
free_stuff (DialogData *dd_data)
{
    g_slist_free_full (dd_data->ws_sett->gsl_files, g_free);
    g_free (dd_data->ws_sett->s_bgcmd);
    g_free (dd_data->ws_sett->s_cfgfile);
    g_free (dd_data->ws_sett->s_lastused);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 *
 * @param       app      Pointer to GtkApplication
 * @param[out]  dd_data  Pointer to DialogData object
 * @return      none
 */
static void
shutdown (GtkApplication *app,
          DialogData     *dd_data)
{
    free_stuff (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Main function.
 *
 * @param[in] argc Arguments passed to the program from the environment in which
 *                 the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */  
int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int             status;
    DialogData      dd_data;
    WallSett        ws_sett;

    dd_data.ws_sett = &ws_sett;
    app = gtk_application_new ("pl.pomorze.init6.wallchanger",
                               G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), &dd_data);
    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), &dd_data);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/


