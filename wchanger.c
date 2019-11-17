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
 * Wallpaper changing appliation.
 *
 * @date November 17, 2019
 *
 * @version 1.2.2
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h> 
#include <gtk/gtk.h>
#include "setts.h"
#include "settstr.h"
#include "iminfo.h"
#include "wallset.h"
#include "imgs.h"
#include "dlgs.h"
#include "treev.h"
#include "flist.h"
#include "errs.h"
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
    GtkWidget  *gw_interval;    /**< Time interval check button */
    GtkWidget  *gw_inter_combo; /**< Time interval combo box */
    WallSett   *ws_sett;        /**< Program settings */
} DialogData;
/*----------------------------------------------------------------------------*/
static inline const char *
get_win_title (void) {
    return "Wall Changer v1.2.2";
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of extensions supported by GdkPixbuf.
 *
 * @param[out]  fl_exts  List to put extension in
 * @return      none 
 */
/*----------------------------------------------------------------------------*/
static void
get_pbuf_extension_append_to_flist (FList *fl_exts)
{
    GdkPixbufFormat  *gpf          = NULL;
    GSList           *gsl_formats  = NULL;
    char            **exts         = NULL;
    char            **it           = NULL;

    /* Get information aboout image formats supported by GdkPixbuf */
    gsl_formats = gdk_pixbuf_get_formats();

    while (gsl_formats != NULL) {
        gpf = gsl_formats->data;
        /* Get extension list for current format */
        exts = gdk_pixbuf_format_get_extensions (gpf);
        for (it = exts; *it != NULL; it++) {
            /* Insert extension to list */
            flist_insert_data (fl_exts, *it);
        }
        /* Free extensions list */
        g_strfreev (exts);
        gsl_formats = gsl_formats->next;
    }
    g_slist_free (gsl_formats);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get screen size.
 *
 * @param[out]  i_scr_w  Screen width
 * @param[out]  i_scr_h  Screen height
 * @return      none
 */
static void
get_screen_size (int *i_scr_w,
                 int *i_scr_h)
{
    GdkRectangle workarea = {0};

    gdk_monitor_get_workarea (
        gdk_display_get_primary_monitor (gdk_display_get_default ()),
        &workarea);

    *i_scr_w = workarea.width;
    *i_scr_h = workarea.height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Paint a preview screen.
 *
 * @param[out]  gr_rect  GdkRectangle with area where preview image can be 
 *                       painted
 * @return      Pixbuf with preview screeen
 */
static GdkPixbuf *
make_prev_screen_pbuf (GdkRectangle *gr_rect)
{
    GdkPixbuf *gp_bg    = NULL; /* Result preview image (screen + wall) */
    GdkPixbuf *gp_scr_1 = NULL; /* Preview (monitor) part 1 (top) image*/
    GdkPixbuf *gp_scr_2 = NULL; /* Preview (monitor) part 1 (top) image*/
    GdkPixbuf *gp_scr_3 = NULL; /* Preview (monitor) part 1 (top) image*/
    int i_screen_w = 0;   /* Screen width */
    int i_screen_h = 0;   /* Screen height */
    int i_pr_scr_w = 138; /* Preview width (inside screen) */
    int i_pr_scr_h = 0;   /* Preview height (inside screen) */
    int i_img_w    = 0;   /* Preview (monitor) image width */
    int i_img_1_h  = 0;   /* Preview (monitor) part 1 (top) image height */
    int i_img_2_h  = 0;   /* Preview (monitor) part 2 (middle) image height */
    int i_img_3_h  = 0;   /* Preview (monitor) part 3 (bottom) image height */

    get_screen_size (&i_screen_w, &i_screen_h);

    /* Count the height of a inside monitor preview screen */
    i_pr_scr_h = (int) ((double) i_screen_h /
                        (double) i_screen_w * (double) i_pr_scr_w);

    gr_rect->x = 7;               /* Preview left point (inside screen) */
    gr_rect->y = 7;               /* Preview top point (inside screen) */
    gr_rect->width = i_pr_scr_w;  /* Preview width (inside screen) */
    gr_rect->height = i_pr_scr_h; /* Preview height (inside screen) */

    gp_scr_1 = get_image (W_IMG_SCREEN_1);  /* top screen image (152x7) */
    gp_scr_2 = get_image (W_IMG_SCREEN_2);  /* middle screen image (152x1) */
    gp_scr_3 = get_image (W_IMG_SCREEN_3);  /* bottom screen image (152x45) */

    /* Get dimensions of monitor part images */
    i_img_w = gdk_pixbuf_get_width (gp_scr_1);
    i_img_1_h = gdk_pixbuf_get_height (gp_scr_1);
    i_img_2_h = gdk_pixbuf_get_height (gp_scr_2);
    i_img_3_h = gdk_pixbuf_get_height (gp_scr_3);

    /* Create pixbuf for the whole preview image */
    gp_bg = gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                            TRUE,
                            8,
                            i_img_w,
                            i_pr_scr_h + i_img_1_h + i_img_3_h);

    /* Paint on the preview image upper first part of monitor */
    gdk_pixbuf_scale (gp_scr_1,
                      gp_bg,
                      0,
                      0,
                      i_img_w,
                      i_img_1_h,
                      0.0,
                      0.0,
                      1.0,
                      1.0,
                      GDK_INTERP_HYPER);

    /* Paint on preview image middle parts of a monitor */
    for (int i = 0; i < i_pr_scr_h; ++i) {
        gdk_pixbuf_scale (gp_scr_2,
                          gp_bg,
                          0,
                          i_img_1_h + i,
                          i_img_w,
                          i_img_2_h,
                          0.0,
                          i_img_1_h + i,
                          1.0,
                          1.0,
                          GDK_INTERP_HYPER);
    }

    /* Paint on the preview image bottom last part of monitor */
    gdk_pixbuf_scale (gp_scr_3,
                      gp_bg,
                      0,
                      i_img_1_h + i_pr_scr_h,
                      i_img_w,
                      i_img_3_h,
                      0.0,
                      i_img_1_h + i_pr_scr_h,
                      1.0,
                      1.0,
                      GDK_INTERP_HYPER);

    g_object_unref (gp_scr_1);
    g_object_unref (gp_scr_2);
    g_object_unref (gp_scr_3);

    return gp_bg;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Paint pixbuf on another pixbuf.
 *
 * Paint GdkPixbuf image pointed by gp_src on GdkPixbuf pointed by gp_dest.
 * Image will be painted in rectangle described in gr_area GdkRectangle.
 *
 * @param[out]  gp_dest  Destination pixpuf to paint on to
 * @param[in]   gp_src   Source pixpuf to paint on destination one
 * @param[in]   gr_area  GdkRectangle with area where preview image can be 
 *                       painted
 * @return      none
 */
static void
paint_pbuf_on_pbuf (GdkPixbuf    *gp_dest,
                    GdkPixbuf    *gp_src,
                    GdkRectangle *gr_area)
{
    double d_sc_x = 0;  /* x direction scale factor */
    double d_sc_y = 0;  /* y direction scale factor */

    d_sc_x = (double) gr_area->width / (double) gdk_pixbuf_get_width (gp_src);
    d_sc_y = (double) gr_area->height / (double) gdk_pixbuf_get_height (gp_src);

    gdk_pixbuf_scale (gp_src,
                      gp_dest,
                      gr_area->x,
                      gr_area->y,
                      gr_area->width,
                      gr_area->height,
                      gr_area->x,
                      gr_area->y,
                      d_sc_x, 
                      d_sc_y,
                      GDK_INTERP_HYPER);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf preview of image from path.
 *
 * @param[in]  s_fname   Image path
 * @return     Pixbuf with preview image.
 */
static GdkPixbuf *
make_image_preview (const char *s_fname)
{
    GdkPixbuf   *gp_prev_screen = NULL; /* Preview image (screen) */
    GdkPixbuf   *gp_prev        = NULL; /* Wallpaper preview image */
    GdkRectangle gr_area        = {0};  /* Wallpaper preview area */

    /* Create the monitor preview pixbuf */
    gp_prev_screen = make_prev_screen_pbuf (&gr_area);

    if (s_fname != NULL) {
        /* Load wallpaper image to pixbuf */
        gp_prev = gdk_pixbuf_new_from_file (s_fname, NULL);
        /* Paint wallpaper preview on the background (monitor) pixbuf */
        paint_pbuf_on_pbuf (gp_prev_screen,
                            gp_prev,
                            &gr_area);
        /* Unref the wallpaper image pixbuf */
        g_object_unref (gp_prev);
    }
    return gp_prev_screen;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 *
 * @param[out]  gw_img_prev  Preview image widget
 * @param[in]   s_file       File to make preview image
 * @return      none
 */
static void
preview_from_file (GtkWidget  *gw_img_prev,
                   const char *s_file)
{
    /* Create peview pixbuf (monitor + wallpaper inside) */
    GdkPixbuf *gp_prev = make_image_preview (s_file);

    if (gp_prev != NULL) {
        /* Clear the image */
        gtk_image_clear (GTK_IMAGE (gw_img_prev));
        /* Set new pixbuf to the image */
        gtk_image_set_from_pixbuf (GTK_IMAGE (gw_img_prev), gp_prev);
        /* Unref preview pixbuf */
        g_object_unref (gp_prev);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change minutes interval from widgets
 *
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @return         Change interval value
 */
static uint32_t
get_wallpaper_change_interval (DialogData *dd_data)
{
    uint32_t ui_res = 0; /* Wallpaper change interval in minutes */

    /* Get spinbutton value of change time interval */
    ui_res = (uint32_t) gtk_spin_button_get_value (
            GTK_SPIN_BUTTON (dd_data->gw_interval));

    /* Check combobox setting if value is supposed to be minutes or hours.
     * If value is set to hours multiply returned value by 60 */
    if (gtk_combo_box_get_active (GTK_COMBO_BOX (dd_data->gw_inter_combo)) == 1)
        ui_res *= 60;

    return ui_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value to widgets
 *
 * @param[in,out]  dd_data  DialogData object with settings and widget data
 * @param[in]      ui_val   Change interval value to set
 * @return         none
 */
static void
set_wallpaper_change_interval (DialogData    *dd_data,
                               const uint32_t ui_val)
{
    uint32_t ui_tmp = ui_val; /* Temp inteval value */

    /* Check if minutes value can be displayed as hours */
    if ((ui_tmp / 60 >= 1) && (ui_tmp % 60 == 0)) {
        /* If value can be hours set combobox to hours */
        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo), 1);
        /* Divide inteval value to display by 60 */
        ui_tmp /= 60;
    }
    else {
        /* Value is suppose to be set as minutes set combobox to minutes */
        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo), 0);
    }
    /* Set spinbutton value with change interval */
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (dd_data->gw_interval),
                               (double) ui_tmp);
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
    GSList *gsl_iinfo = NULL; /* list of ImageInfo items */

    /* Convert file list to ImageInfo list */
    gsl_iinfo = flist_to_imageinfo (&dd_data->ws_sett->fl_files);

    /* Add ImageInfo list file data to TreeView */
    liststore_add_items (dd_data->gw_view, gsl_iinfo);

    /* Set wallpaper random choose setting */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_random),
                                  settings_get_random (dd_data->ws_sett));

    /* Set last used wallpaper on start setting */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_lastused),
                          settings_get_last_used_setting (dd_data->ws_sett));

    /* Set minutes/hours of wallpaper change interval */
    set_wallpaper_change_interval (dd_data,
            settings_get_interval (dd_data->ws_sett));

    /* Set background set command */
    if (settings_get_command (dd_data->ws_sett) != NULL)
        gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command),
                            settings_get_command (dd_data->ws_sett));

    /* Free ImageInfo list */
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
    GSList     *gsl_iinfo1 = NULL; /* List of ImageInfo data */

    /* get ImageInfo list of TreeView files */
    gsl_iinfo1 = treeview_get_data (dd_data->gw_view);

    /* Clear file list */
    flist_clear (&dd_data->ws_sett->fl_files);
    /* Append file names from ImageInfo list to file list */
    imageinfo_append_to_flist (gsl_iinfo1, &dd_data->ws_sett->fl_files);

    /* Free ImageInfo list */
    g_slist_free_full (gsl_iinfo1, (GDestroyNotify) imageinfo_free);

    /* Get last used wallpaper on start setting */
    settings_set_last_used_setting (dd_data->ws_sett,
            (uint8_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_lastused)));

    /* Get random wallpapet setting */
    settings_set_random (dd_data->ws_sett,
            (uint8_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_random)));

    /* Get wallpaper change interval setting */
    settings_set_interval (dd_data->ws_sett, 
                           get_wallpaper_change_interval (dd_data));

    /* Get wallpaper set command */
    settings_set_command (dd_data->ws_sett,
                          gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command)));
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
    GSList *gsl_files = NULL; /* List with files from dialog */
    GSList *gsl_iinfo = NULL; /* List with ImageInfo file list */

    /* Run image select dialog and get selected files */
    gsl_files = add_images_dialog (dd_data->gw_window);

    /* Convert file list to ImageInfo list */
    gsl_iinfo = file_paths_to_imageinfo (gsl_files);

    /* Add ImageInfo items to TreeView */
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
    GSList *gsl_files = NULL; /* List of ImageInfo items to liststore */
    char   *s_folder  = NULL; /* Selecred directory name */
    FList   fl_files;         /* Files in directory */
    FList   fl_exts;          /* List of extensions of supported image types */

    flist_init (&fl_exts);
    flist_init (&fl_files);

    /* Append to list extensions of image types supported by GdkPixbuf */
    get_pbuf_extension_append_to_flist (&fl_exts);

    /* Run directory select dialog and get selected directory name */
    s_folder = add_images_folder_dialog (dd_data->gw_window);

    if (s_folder != NULL) {
        /* Scan directory for files and append them to file list */
        get_directory_content_append_to_flist (s_folder, &fl_files);

        /* Filter file list of files with extensions that are not
         * supported by GdkPixbuf */
        flist_filter_by_extensions_list (&fl_files, &fl_exts);

        /* Convert file list to ImageInfo list */
        gsl_files = flist_to_imageinfo (&fl_files);

        /* Add ImageInfo items to TreeView */
        liststore_add_items (dd_data->gw_view, gsl_files);

        g_slist_free_full (gsl_files, (GDestroyNotify) imageinfo_free);
        g_free (s_folder);
    }
    flist_free (&fl_files);
    flist_free (&fl_exts);
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
 * @brief  Remove duplicates button pressed.
 *
 * @param          widget    The object on which the signal is emitted
 * @param[in,out]  gw_tview  TreeView to remove duplicates
 * @return         none
 */
static void
event_remove_duplicates_pressed (GtkWidget *widget,
                                 GtkWidget *gw_tview)
{
    treeview_remove_duplicates (gw_tview);
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

        if (wallpaper_dialog_set (dd_data->ws_sett,
                                 imageinfo_get_full_name (ii_info)) != ERR_OK) {
            message_dialog_error (dd_data->gw_window, "Some error occured"); 
        }
        imageinfo_free (ii_info);
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
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
    /* Get settings from widgets to WallSett object */
    gather_settings (dd_data);

    /* Save setting to config file */
    if (settings_write (dd_data->ws_sett) != ERR_OK) {
        message_dialog_error (dd_data->gw_window,
                              "Error while saving settings");
        //g_application_quit (G_APPLICATION (dd_data));
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
        preview_from_file (gw_img_prev, imageinfo_get_full_name (ii_info));
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
    int        i_res2    = 0;
    int        i_res     = 0;
    uint8_t    i_changed = 0;

    int i_w = 0;
    int i_h = 0;

    gtk_window_get_size (GTK_WINDOW (window), &i_w, &i_h);

    gather_settings (dd_data);

    i_res = settings_check_changed (dd_data->ws_sett, &i_changed);
    if (i_res != ERR_OK) {
        message_dialog_error (dd_data->gw_window, "Some error occured"); 
    }

    if (i_changed) {
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

    /* If window dimensions changed */
    if ((settings_get_window_width (dd_data->ws_sett) != i_w) ||
        (settings_get_window_height (dd_data->ws_sett) != i_h)) {
        settings_update_window_size (dd_data->ws_sett, i_w, i_h);
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
    const char *s_str    = "Wall Changer - Program settings";
    const char *s_format = "<span size=\"20000\" weight=\"bold\" \
                            foreground=\"#0099e6\" style=\"italic\">%s</span>";
    char       *s_markup = NULL;
    GtkWidget  *gw_label = gtk_label_new (NULL);

    *gw_title_widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

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
create_image_button (const char   *s_label,
                     const char   *s_hint,
                     const IconImg i_but)
{
    GtkWidget *gw_btn;
    GtkWidget *gw_img;
    GdkPixbuf *gd_pix = NULL;

    gw_btn = gtk_button_new ();

    if (strcmp (s_label, "") != 0)
        gtk_button_set_label (GTK_BUTTON (gw_btn), s_label);

    if (strcmp (s_hint, "") != 0)
        gtk_widget_set_tooltip_text (gw_btn, s_hint);

    gd_pix = get_image (i_but);
    if (gd_pix != NULL) {
        gw_img = gtk_image_new_from_pixbuf (gd_pix);
        gtk_button_set_image (GTK_BUTTON (gw_btn), gw_img);
        g_object_unref (gd_pix);
    }
    return gw_btn;
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

    gw_button = create_image_button ("", "Add images", W_ICON_ADD);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_add_images_pressed),
                      dd_data);
    gw_button = create_image_button ("", "Add images from folder",
                                     W_ICON_ADD_DIR);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_add_images_folder_pressed),
                      dd_data);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    gw_button = create_image_button ("", "Remove images", W_ICON_REMOVE);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_remove_from_list_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Move up", W_ICON_UP);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_move_up_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Move down", W_ICON_DOWN);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_move_down_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Sort images", W_ICON_SORT);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_sort_list_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Remove duplicates", W_ICON_DUPL);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_remove_duplicates_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button ("", "Set wallpaper", W_ICON_SCREEN);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_set_wallpaper_pressed),
                      dd_data);
    gw_button = create_image_button ("", "Save settings", W_ICON_FLOPPY);
    gtk_box_pack_start (GTK_BOX (*gw_buttons_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_save_settings_pressed),
                      dd_data);
    gw_button = create_image_button ("", "Exit app", W_ICON_EXIT);
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
    GtkWidget     *gw_command_label;
    GtkWidget     *gw_interval_label;
    GtkWidget     *gw_time_combo;

    /* Random wallpaper change button */
    gw_button_random = gtk_check_button_new ();
    gtk_button_set_label (GTK_BUTTON (gw_button_random),
                          "Random wallpaper change");

    /* Select last used wallpaper button */
    gw_button_selectlast = gtk_check_button_new ();
    gtk_button_set_label (GTK_BUTTON (gw_button_selectlast),
                          "Select last used wallpaper at start");

    /* Wallpaper set command entry */
    gw_command_entry = gtk_entry_new ();
    gw_command_label = gtk_label_new ("Background set command : ");

    /* Background change interval widgets */
    ga_adjustment = gtk_adjustment_new (5.0, 1.0, 6000.0, 1.0, 5.0, 0.0);
    gw_spinbutton = gtk_spin_button_new (ga_adjustment, 1.0, 0);
    gw_interval_label = gtk_label_new ("Background change interval : ");
    gw_time_combo = gtk_combo_box_text_new ();
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_time_combo),
                               NULL,
                               "minutes");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_time_combo),
                               NULL,
                               "hours");
    gtk_combo_box_set_active (GTK_COMBO_BOX (gw_time_combo), 0);

    /* Setting pointers in DialogData */
    dd_data->gw_random = gw_button_random;
    dd_data->gw_lastused = gw_button_selectlast;
    dd_data->gw_command = gw_command_entry;
    dd_data->gw_interval = gw_spinbutton;
    dd_data->gw_inter_combo = gw_time_combo;

    /* Container for settings widgets */
    *gw_settings_widget = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (*gw_settings_widget), 8);
    gtk_grid_set_row_spacing (GTK_GRID (*gw_settings_widget), 8);

    /* Packing button for random change */
    gtk_grid_attach (GTK_GRID (*gw_settings_widget),
                     gw_button_random, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_button_selectlast, gw_button_random,
                             GTK_POS_BOTTOM, 1, 1);

    /* Packing background set command */
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_command_label, gw_button_random,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_command_entry, gw_command_label,
                             GTK_POS_RIGHT, 1, 1);

    /* Packing time interval widgets */
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_interval_label, gw_button_selectlast,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_spinbutton, gw_interval_label,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_settings_widget),
                             gw_time_combo, gw_spinbutton,
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
    GtkWidget *gw_window;          /* Application window */
    GtkWidget *gw_title_widget;    /* Top title widget */
    GtkWidget *gw_tview;           /* Wallpaper list TreeView */
    GtkWidget *gw_scroll;          /* Scrolled window for wallpaper list */
    GtkWidget *gw_buttons_widget;  /* Buttons widget */
    GtkWidget *gw_img_prev;        /* Wallpaper preview widget */
    GtkWidget *gw_box_prev;        /* Widget for wallpaper preview */
    GtkWidget *gw_box_list_btns;   /* Widget for list, buttons, preview */
    GtkWidget *gw_settings_widget; /* Setings for wallpaper changing */
    GtkWidget *gw_box_main;        /* Main box to pack everything */

    /* Image preview widget */
    gw_img_prev = gtk_image_new ();

    /* Main window */
    gw_window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (gw_window), get_win_title ());
    g_signal_connect (gw_window, "delete-event",
                  G_CALLBACK (event_on_delete), dd_data);
    dd_data->gw_window = GTK_WINDOW (gw_window);

    create_tview (&gw_tview);
    dd_data->gw_view = gw_tview;
    g_signal_connect (gw_tview, "row-activated",
                      G_CALLBACK (event_img_list_activated), gw_img_prev);

    create_title_widget (&gw_title_widget);

    create_buttons_widget (&gw_buttons_widget, dd_data);

    preview_from_file (gw_img_prev, NULL);

    create_settings_widget (&gw_settings_widget, dd_data);

    /* Scrolled window for TreeView */
    gw_scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (gw_scroll), gw_tview);

    /* Box with wallpaper preview */
    gw_box_prev = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_prev),
                        gw_img_prev,
                        FALSE, FALSE, 4);

    /* Pack file list, button box, separatr and wallpaper preview */
    gw_box_list_btns = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
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

    /* Pack in main box title and box with list, buttons and preview */
    gw_box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_title_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_box_list_btns,
                        TRUE, TRUE, 4);

    /* Pack in main box separator and settings */
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_settings_widget,
                        FALSE, FALSE, 4);

    gtk_container_set_border_width (GTK_CONTAINER (gw_box_main), 10);
    gtk_container_add (GTK_CONTAINER (gw_window), gw_box_main);

    if (settings_init (dd_data->ws_sett) != ERR_OK) {
        g_application_quit (G_APPLICATION (app));
        return;
    }

    if (settings_read (dd_data->ws_sett) != ERR_OK) {
        g_application_quit (G_APPLICATION (app));
        return;
    }

    load_settings (dd_data);

    /* Set window dimensions */
    gtk_window_set_default_size (GTK_WINDOW (gw_window), 
            settings_get_window_width (dd_data->ws_sett),
            settings_get_window_height (dd_data->ws_sett));

    /* Check if there is last used wallpaper info, if it is select it on
     * list and make a preview image */
    if (settings_get_last_used_fn (dd_data->ws_sett) != NULL) {
        preview_from_file (gw_img_prev,
                           settings_get_last_used_fn (dd_data->ws_sett));
        treeview_find_select_item (gw_tview,
                settings_get_last_used_fn (dd_data->ws_sett));
    }

    gtk_window_set_application (GTK_WINDOW (gw_window), GTK_APPLICATION (app));
    gtk_widget_show_all (gw_window);
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
    settings_free (dd_data->ws_sett);
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


