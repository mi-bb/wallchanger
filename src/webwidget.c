/**
 * @file  webwidget.c
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
 * @brief  Web widget
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <err.h>
#include <ctype.h>
#include "urldata.h"
#include "jsfun.h"
#include "searchitem.h"
#include "dlgsmsg.h"
#include "strfun.h"
#include "setts.h"
#include "errs.h"
#include "cfgfile.h"
#include "fdfn.h"
#include "defs.h"
#include "fourstrings.h"
#include "webwidget_common.h"
#include "webpexels.h"
#include "webflickr.h"
#include "webwidget.h"
/**
 * @def   NAME_LEN
 * @brief Lenght of name in download progress window.
 *
 * @def   SEL_NAME_LEN
 * @brief Lenght of name in selected images combobox.
 */
#define NAME_LEN 40
#define SEL_NAME_LEN 60

/*----------------------------------------------------------------------------*/
/**
 * @brief  Services combobox columns
 */
enum e_services_columns {
    WW_COMBO_ID,    /**< Service id */
    WW_COMBO_NAME,  /**< Service name */
    WW_COMBO_STR_1, /**< Some needed string */
    WW_COMBO_STR_2, /**< Another string */
    WW_COMBO_STR_3, /**< Third string */
    WW_COMBO_STR_4, /**< Fourth string */
    WW_COMBO_CNT    /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Selected to download combobox columns
 */
enum e_selected_columns {
    WW_SELCOMBO_PIXBUF,    /**< Thumbnail pixbuf */
    WW_SELCOMBO_WIDTH,     /**< Image width */
    WW_SELCOMBO_HEIGHT,    /**< Image height */
    WW_SELCOMBO_ID,        /**< Image id */
    WW_SELCOMBO_DISP_NAME, /**< Name of image */
    WW_SELCOMBO_FILE_NAME, /**< Name of image */
    WW_SELCOMBO_IMAGE_URL, /**< Full size image url */
    WW_SELCOMBO_CNT        /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * Stop downloading images button pressed
 *
 * @param[out] i_stop  Stop val to set
 * @return     none
 */
static void
event_stop_button_pressed (int *i_stop)
{
    *i_stop = 1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Download images progress window.
 *
 * @param[in] gw_parent     Parent window
 * @param[in] gl_item_list  List with SearchItem image data items
 * @return    List with paths of downloaded images
 */
static GList *
download_progress_window (GtkWindow *gw_parent,
                          GList     *gl_item_list)
{
    GtkWidget  *gw_window;       /* Progress window widget */
    GtkWidget  *gw_label;        /* Label for file name */
    GtkWidget  *gw_progress;     /* Progress bar */
    GtkWidget  *gw_box_main;     /* Box for window widgets */
    GtkWidget  *gw_stop_button;  /* Stop downloading button */
    SearchItem *si_item;         /* For images from list */
    GList      *gl_item  = NULL; /* Pointer to list */
    GList      *gl_res   = NULL; /* Result list with downloaded files */
    const char *s_format = "<span style=\"italic\">%s</span>";
    char       *s_markup = NULL; /* For label's markup */
    char       *s_wpdir  = NULL; /* Path to wallpaper directory */
    char       *s_fn     = NULL; /* File path for wallpaper save */
    char       *s_err    = NULL; /* Error output */
    float       f_step   = 0;    /* Progress bar step */
    float       f_frac   = 0;    /* Fraction for progress bar to set */
    int         i_stop   = 0;    /* To set when stop button is pressed */
    int         i_perm   = 0;    /* For checking save dir permissions */
    size_t      ui_nlen  = 0;    /* Length of file name */
    char s_nbuff[NAME_LEN * 4 + 4];

    memset (s_nbuff, '\0', NAME_LEN * 4 + 4);
    memset (s_nbuff, ' ',  NAME_LEN + 10);

    /* Getting wallpaper save path and checking it's permissions, creating if
     * it doesn't exist */
    s_wpdir = cfgfile_get_app_wallpaper_path ();
    i_perm  = dir_check_permissions (s_wpdir);

    if (i_perm == ERR_FILE ||
       (i_perm == ERR_FILE_EX && dir_create_with_subdirs (s_wpdir) != ERR_OK)) {
        message_dialog_error (NULL, err_get_message (i_perm));
        free (s_wpdir);
        return NULL;
    }

    /* Setting progress step and making a pointer to list */
    f_step  = (float) 1.0 / (float) g_list_length (gl_item_list);
    gl_item = gl_item_list;

    /* Creating progress window with widgets */
    gw_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_transient_for (GTK_WINDOW (gw_window), gw_parent);
    gtk_window_set_modal (GTK_WINDOW (gw_window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (gw_window), FALSE);

    gw_label = gtk_label_new (s_nbuff);
    gw_stop_button = gtk_button_new_with_label ("Cancel");
    g_signal_connect_swapped (gw_stop_button, "clicked",
                              G_CALLBACK (event_stop_button_pressed),
                              &i_stop);

    gw_progress = gtk_progress_bar_new ();
    gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (gw_progress), TRUE);
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gw_progress), NULL);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gw_progress), 0);

    gw_box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width (GTK_CONTAINER (gw_box_main), 10);

    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gtk_label_new ("Downloading file:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_progress,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_stop_button,
                        FALSE, FALSE, 4);

    gtk_container_add (GTK_CONTAINER (gw_window), gw_box_main);
    gtk_widget_show_all (gw_window);

    /* Processing list of wallpapers to download */
    while (gl_item != NULL) {
        if (i_stop == 1)
            break;

        f_frac  += f_step;
        si_item  = gl_item->data;

        ui_nlen = (size_t) g_utf8_strlen (si_item->s_file_name, -1);

        if (ui_nlen > NAME_LEN) {
            g_utf8_strncpy (s_nbuff, si_item->s_file_name, NAME_LEN);
            strcat (s_nbuff, "...");
        }
        else {
            strcpy (s_nbuff, si_item->s_file_name);
        }
        s_fn = str_comb (s_wpdir, "/");
        str_append (&s_fn, si_item->s_file_name);
        s_markup = g_markup_printf_escaped (s_format, s_nbuff);

        gtk_label_set_markup (GTK_LABEL (gw_label), s_markup);
        g_free (s_markup);

        while (gtk_events_pending ())
            gtk_main_iteration ();
#ifdef DEBUG
        printf ("downloading:%s\nto:%s\n", si_item->s_image_url, s_fn);
#endif
        urldata_get_to_file (si_item->s_image_url, s_fn, &s_err);
        if (s_err != NULL) {
            message_dialog_error (NULL, s_err);
            free (s_err);
            free (s_fn);
            break;
        }
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gw_progress), f_frac);

        while (gtk_events_pending ())
            gtk_main_iteration ();

        gl_res  = g_list_prepend (gl_res, s_fn);
        gl_item = gl_item->next;
    }
    free (s_wpdir);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gw_progress), 1.0);

    gtk_widget_destroy (gw_window);

    return g_list_reverse (gl_res);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get integer value from Combobox's active i_col column.
 *
 * @param[in] gw_combo  Combobox
 * @param[in] i_col     Column with data
 * @return    Int value
 */
static int
combo_get_active_int (GtkWidget *gw_combo,
                      const int  i_col)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    int i_ret = 0;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, i_col, &i_ret, -1);
    }
    return i_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get strings with API key values from Combobox.
 *
 * @param[in]  gw_combo  Combobox
 * @param[out] fs_data   Four strings to write API keys
 * @return     none
 */
static void
combo_get_active_strings (GtkWidget   *gw_combo,
                          FourStrings *fs_data)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter,
                            WW_COMBO_STR_1, &fs_data->s_str1,
                            WW_COMBO_STR_2, &fs_data->s_str2,
                            WW_COMBO_STR_3, &fs_data->s_str3,
                            WW_COMBO_STR_4, &fs_data->s_str4,
                            -1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set string in Combobox's active i_col column.
 *
 * @param[out] gw_combo  Combobox
 * @param[in]  i_col     Column to set data
 * @param[in]  s_val     String value
 * @return     none
 */
static void
combo_set_active_str (GtkWidget  *gw_combo,
                      const int   i_col,
                      const char *s_val)
{
    GtkListStore *gls_slstore;
    GtkTreeIter   gti_iter;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &gti_iter)) {

        gls_slstore = GTK_LIST_STORE (gtk_combo_box_get_model (
                    GTK_COMBO_BOX (gw_combo)));

        gtk_list_store_set (gls_slstore, &gti_iter, i_col, s_val, -1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set strings with API key values to Combobox.
 *
 * @param[in]  gw_combo  Combobox
 * @param[out] fs_data   Four strings with API keys
 * @return     none
 */
static void
combo_set_active_strings (GtkWidget         *gw_combo,
                          const FourStrings *fs_data)
{
    GtkListStore *gls_slstore;
    GtkTreeIter   gti_iter;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &gti_iter)) {

        gls_slstore = GTK_LIST_STORE (gtk_combo_box_get_model (
                    GTK_COMBO_BOX (gw_combo)));

        gtk_list_store_set (gls_slstore, &gti_iter,
                            WW_COMBO_STR_1, fs_data->s_str1,
                            WW_COMBO_STR_2, fs_data->s_str2,
                            WW_COMBO_STR_3, fs_data->s_str3,
                            WW_COMBO_STR_4, fs_data->s_str4,
                            -1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update labels and entry with srach results info.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
update_labels (WebWidget *ww_widget)
{
    const char *s_format_count = "Search query: %s,  Found %d images";
    char       *s_markup       = NULL;
    char        s_entry[128];

    s_markup = g_markup_printf_escaped (s_format_count,
                                        ww_widget->s_query,
                                        ww_widget->i_found_cnt);
    gtk_label_set_markup (GTK_LABEL (ww_widget->gw_count_label), s_markup);
    g_free (s_markup);

    snprintf (s_entry, 128, "%d", ww_widget->i_page);
    gtk_entry_set_text (GTK_ENTRY (ww_widget->gw_nav_entry), s_entry);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if image with specified id exists on selected to download list.
 *
 * @param[in] gtm_model  TreeModel to check
 * @param[in] i_id       Image id to check
 * @return    1 if exists, 0 if not
 */
static int
sel_combo_check_exist (GtkTreeModel *gtm_model,
                       const int     i_id)
{
    int         i_read_id = 0;
    gboolean    b_res     = FALSE;
    GtkTreeIter gti_iter;

    b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {

        gtk_tree_model_get (gtm_model, &gti_iter,
                            WW_SELCOMBO_ID, &i_read_id,
                            -1);
        if (i_id == i_read_id)
            return 1;
        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of SearchItem items from selected to download combo box.
 *
 * @param[in] gw_selected_combo  Combobox to get list
 * @return    List of images to download in SearchItem items
 */
static GList *
sel_combo_get_list (GtkWidget *gw_selected_combo)
{
    GList        *gl_res    = NULL;
    SearchItem   *si_item   = NULL;
    GtkTreeModel *gtm_model = NULL;
    gboolean      b_res     = FALSE;
    GtkTreeIter   gti_iter;

    gtm_model = gtk_combo_box_get_model (
            GTK_COMBO_BOX (gw_selected_combo));

    b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {

        si_item = searchitem_new ();
        gtk_tree_model_get (gtm_model, &gti_iter,
                            WW_SELCOMBO_FILE_NAME, &si_item->s_file_name,
                            WW_SELCOMBO_IMAGE_URL, &si_item->s_image_url,
                            -1);
        gl_res = g_list_prepend (gl_res, si_item);
        b_res = gtk_tree_model_iter_next (gtm_model, &gti_iter);
    }
    return g_list_reverse (gl_res);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search web for images.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
search_web (WebWidget *ww_widget)
{
    int i_id = combo_get_active_int (ww_widget->gw_combo, WW_COMBO_ID);

    FourStrings *fs_data = fourstrings_new ();

    combo_get_active_strings (ww_widget->gw_combo, fs_data);

    switch (i_id) {
        case WEB_WIDGET_PEXELS:
            pexels_search (ww_widget, fs_data);
            break;
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
            flickr_search (ww_widget, fs_data);
            break;
#endif
        default:
            break;
    }
    fourstrings_free (fs_data);
    update_labels (ww_widget);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search button pressed.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_search_pressed (WebWidget *ww_widget)
{
    const char *s_query = gtk_entry_get_text (GTK_ENTRY (ww_widget->gw_entry));

    if (check_empty (s_query, "Empty search query"))
        return;

    if (ww_widget->s_query != NULL)
        free (ww_widget->s_query);

    ww_widget->s_query = strdup (s_query);
    ww_widget->i_page = 1;
    search_web (ww_widget);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Next page button pressed.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_next_pressed (WebWidget *ww_widget)
{
    if (ww_widget->s_query != NULL && ww_widget->i_page < INT_MAX) {
        ++ww_widget->i_page;
        search_web (ww_widget);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Previous page button pressed.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_prev_pressed (WebWidget *ww_widget)
{
    if (ww_widget->s_query != NULL && ww_widget->i_page > 1) {
        --ww_widget->i_page;
        search_web (ww_widget);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enter pressed in page number entry.
 *
 * @param[in,out] gw_entry   Page number entry
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_nav_entry_act (GtkWidget *gw_entry,
                     WebWidget *ww_widget)
{
    const char    *s_val = NULL; /* String from entry */
    long long int  i_val = 0;    /* Int value of entry string */

    if (ww_widget->s_query != NULL) {
        s_val = gtk_entry_get_text (GTK_ENTRY (gw_entry));
        i_val = strtoll (s_val, NULL, 10);
        if      (i_val < 1)       i_val = 1;
        else if (i_val > INT_MAX) i_val = INT_MAX;
        ww_widget->i_page = (int) i_val;
        search_web (ww_widget);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Settings button pressed.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_settings_pressed (WebWidget *ww_widget)
{
    FourStrings *fs_data = NULL;
    int          i_err   = 0;    /* Error output */
    int          i_id    = 0;    /* Service id */
    int          i_res   = 0;

    i_id    = combo_get_active_int (ww_widget->gw_combo, WW_COMBO_ID);
    fs_data = fourstrings_new ();

    switch (i_id) {
        case WEB_WIDGET_PEXELS:
            combo_get_active_strings (ww_widget->gw_combo, fs_data);

            i_res = pexels_settings_dialog (fs_data);

            if (i_res == GTK_RESPONSE_ACCEPT) {
                combo_set_active_str (ww_widget->gw_combo, WW_COMBO_STR_1,
                                      fs_data->s_str1);

                i_err = setts_update_pexels_api (ww_widget->s_cfg_file,
                                                 fs_data->s_str1);
                if (i_err != ERR_OK) {
                    message_dialog_error (NULL, err_get_message (i_err));
                }
            }
            break;
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
            combo_get_active_strings (ww_widget->gw_combo, fs_data);

            i_res = flickr_settings_dialog (fs_data);

            if (i_res == GTK_RESPONSE_ACCEPT) {
                combo_set_active_strings (ww_widget->gw_combo, fs_data);

                i_err = setts_update_flickr_api (ww_widget->s_cfg_file,
                                                 fs_data->s_str1,
                                                 fs_data->s_str2,
                                                 fs_data->s_str3,
                                                 fs_data->s_str4);
                if (i_err != ERR_OK) {
                    message_dialog_error (NULL, err_get_message (i_err));
                }
            }
            break;
#endif
        default:
            break;
    }
    fourstrings_free (fs_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add selected images to list button pressed.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_add_selected_pressed (WebWidget *ww_widget)
{
    GtkListStore *gls_slstore;
    GtkTreeModel *gtm_model;
    GtkTreeModel *gtm_combo_model;
    GdkPixbuf    *gp_pbuf;
    GdkPixbuf    *gp_pbuf2;
    GtkTreeIter   gti_iter;
    GtkTreeIter   gti_iters;
    GtkTreePath  *gtp_path;
    GList        *gl_selected = NULL;
    GList        *gl_item     = NULL;
    char         *s_disp_name = NULL;
    char         *s_file_name = NULL;
    char         *s_image_url = NULL;
    int           i_id        = 0;
    int           i_w         = 0;
    int           i_h         = 0;
    float         f_w         = 0;
    float         f_h         = 0;
    char          s_dname[SEL_NAME_LEN * 4 + 4];

    gtm_combo_model = gtk_combo_box_get_model (
            GTK_COMBO_BOX (ww_widget->gw_selected_combo));

    gls_slstore = GTK_LIST_STORE (gtk_combo_box_get_model (
                GTK_COMBO_BOX (ww_widget->gw_selected_combo)));

    gtm_model = gtk_icon_view_get_model (GTK_ICON_VIEW (
                ww_widget->gw_img_view));

    gl_selected = gtk_icon_view_get_selected_items (
            GTK_ICON_VIEW (ww_widget->gw_img_view));

    gl_item = gl_selected;

    while (gl_item != NULL) {

        gtp_path = gl_item->data;

        if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gtp_path)) {

            f_w = SEL_TH_WIDTH;
            f_h = SEL_TH_HEIGHT;

            gtk_tree_model_get (gtm_model, &gti_iter,
                                WEB_COL_PIXBUF,    &gp_pbuf,
                                WEB_COL_WIDTH,     &i_w,
                                WEB_COL_HEIGHT,    &i_h,
                                WEB_COL_ID,        &i_id,
                                WEB_COL_DISP_NAME, &s_disp_name, 
                                WEB_COL_FILE_NAME, &s_file_name, 
                                WEB_COL_IMAGE_URL, &s_image_url, 
                                -1);

            if (sel_combo_check_exist (gtm_combo_model, i_id) == 0) {

                i_w = gdk_pixbuf_get_width (gp_pbuf);
                i_h = gdk_pixbuf_get_height (gp_pbuf);

                if (g_utf8_strlen (s_disp_name, -1) > SEL_NAME_LEN) {
                    g_utf8_strncpy (s_dname, s_disp_name, SEL_NAME_LEN);
                    strcat (s_dname, "...");
                }
                else {
                    strcpy (s_dname, s_disp_name);
                }

                if (i_w > i_h) {
                    f_h = (float) i_h / (float) i_w * f_w;
                }
                else if (i_w < i_h) {
                    f_w = (float) i_w / (float) i_h * f_h;
                }
                gp_pbuf2 = gdk_pixbuf_scale_simple (gp_pbuf,
                                                    (int) f_w,
                                                    (int) f_h,
                                                    GDK_INTERP_HYPER);

                gtk_list_store_append (gls_slstore, &gti_iters);
                gtk_list_store_set (gls_slstore, &gti_iters,
                                    WW_SELCOMBO_PIXBUF,    gp_pbuf2,
                                    WW_SELCOMBO_WIDTH,     i_w,
                                    WW_SELCOMBO_HEIGHT,    i_h,
                                    WW_SELCOMBO_ID,        i_id,
                                    /*WW_SELCOMBO_DISP_NAME, s_disp_name,*/
                                    WW_SELCOMBO_DISP_NAME, s_dname,
                                    WW_SELCOMBO_FILE_NAME, s_file_name,
                                    WW_SELCOMBO_IMAGE_URL, s_image_url,
                                    -1);
                g_object_unref(gp_pbuf2);
            }
            free (s_disp_name);
            free (s_file_name);
            free (s_image_url);
            g_object_unref(gp_pbuf);
        }
        gl_item = gl_item->next;
    }
    g_list_free_full (gl_selected, (GDestroyNotify) gtk_tree_path_free);

    if (gtk_combo_box_get_active (GTK_COMBO_BOX
                (ww_widget->gw_selected_combo)) == -1) {

        gtk_combo_box_set_active (GTK_COMBO_BOX (
                    ww_widget->gw_selected_combo), 0);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create combo box for image search services.
 *
 * @return  Combobox widget
 */
static GtkWidget *
webwidget_combobox_create (Setting *st_settings)
{
    GtkWidget       *gw_combo;             /* Result ComboBox */
    GtkCellRenderer *cell;                 /* CellRenderer */
    GtkListStore    *list_store;           /* ListStore for combobox data */
    GtkTreeIter      iter;                 /* TreeIter */
    const char      *s_pexels_api  = NULL; /* Pexels API key */
    Setting         *st_s1;                /* For individual setting */
#ifdef HAVE_FLICKCURL
    const char      *s_flickr_key  = NULL; /* Flickr key */
    const char      *s_flickr_sec  = NULL; /* Flickr secret */
    const char      *s_flickr_acct = NULL; /* Flickr access token */
    const char      *s_flickr_accs = NULL; /* Flickr access token secret */
    Setting         *st_s2;                /* For individual setting */
    Setting         *st_s3;                /* For individual setting */
    Setting         *st_s4;                /* For individual setting */
#endif

    /* Get Pexels API string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETTING_PEXELS_API));

    s_pexels_api = st_s1 == NULL ? "" : setting_get_string (st_s1);

    /* Liststore for service data */
    list_store = gtk_list_store_new (WW_COMBO_CNT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        WW_COMBO_ID,    WEB_WIDGET_PEXELS,
                        WW_COMBO_NAME,  "Pexels",
                        WW_COMBO_STR_1, s_pexels_api,
                        WW_COMBO_STR_2, "",
                        WW_COMBO_STR_3, "",
                        WW_COMBO_STR_4, "",
                        -1);

#ifdef HAVE_FLICKCURL
    /* Get Flickr API key string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETTING_FLICKR_CLKEY));
    /* Get Flickr API secret string */
    st_s2 = setting_find_child (st_settings,
                                get_setting_name (SETTING_FLICKR_CLSEC));
    /* Get Flickr API access token */
    st_s3 = setting_find_child (st_settings,
                                get_setting_name (SETTING_FLICKR_ACTOK));
    /* Get Flickr API access token */
    st_s4 = setting_find_child (st_settings,
                                get_setting_name (SETTING_FLICKR_ACSEC));

    s_flickr_key  = st_s1 == NULL ? "" : setting_get_string (st_s1);
    s_flickr_sec  = st_s2 == NULL ? "" : setting_get_string (st_s2);
    s_flickr_acct = st_s3 == NULL ? "" : setting_get_string (st_s3);
    s_flickr_accs = st_s4 == NULL ? "" : setting_get_string (st_s4);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        WW_COMBO_ID,    WEB_WIDGET_FLICKR,
                        WW_COMBO_NAME,  "Flickr",
                        WW_COMBO_STR_1, s_flickr_key,
                        WW_COMBO_STR_2, s_flickr_sec,
                        WW_COMBO_STR_3, s_flickr_acct,
                        WW_COMBO_STR_4, s_flickr_accs,
                        -1);
#endif

    gw_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "text", WW_COMBO_NAME);

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter))
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX (gw_combo), &iter);

    return gw_combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create combo box for images selected for download.
 *
 * @return  Combobox widget
 */
static GtkWidget *
webwidget_selected_combobox_create (void)
{
    GtkWidget       *gw_combo;      /* Result ComboBox */
    GtkCellRenderer *cell;          /* CellRenderer */
    GtkListStore    *list_store;    /* ListStore for combobox data */
    GtkTreeIter      iter;          /* TreeIter */

    list_store = gtk_list_store_new (WW_SELCOMBO_CNT,
                                     GDK_TYPE_PIXBUF,
                                     G_TYPE_INT,
                                     G_TYPE_INT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gw_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_pixbuf_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, FALSE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "pixbuf", WW_SELCOMBO_PIXBUF);

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "text", WW_SELCOMBO_DISP_NAME);

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter))
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX (gw_combo), &iter);

    return gw_combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create icon view for wallpaper image search result.
 *
 * @return  IconView widget
 */
static GtkWidget *
webwidget_imgview_create (void)
{
    GtkWidget    *gw_icon_view;
    GtkListStore *list_store;

    /* Image list */
    list_store = gtk_list_store_new (WEB_NUM_CNT,
                                     GDK_TYPE_PIXBUF,
                                     G_TYPE_INT,
                                     G_TYPE_INT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gw_icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (list_store));

    gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (gw_icon_view),
                                      GTK_SELECTION_MULTIPLE);

    gtk_icon_view_set_text_column (GTK_ICON_VIEW (gw_icon_view),
                                   WEB_COL_DISP_NAME);
    //gtk_icon_view_set_markup_column (GTK_ICON_VIEW (gw_icon_view),
    //                                 WEB_COL_MARKUP);
    gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (gw_icon_view),
                                     WEB_COL_PIXBUF);
    gtk_icon_view_set_item_width (GTK_ICON_VIEW (gw_icon_view), 210);
    gtk_icon_view_set_columns (GTK_ICON_VIEW (gw_icon_view), 3);

    return gw_icon_view;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free WebWidget item.
 */
void
webwidget_free (WebWidget *ww_widget)
{
    free (ww_widget->s_cfg_file);
    free (ww_widget->s_query);
    free (ww_widget);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create WebWidget item with widgets and data for wallpaper search.
 */
WebWidget *
webwidget_create (Setting    *st_settings,
                  const char *s_cfg_file)
{
    WebWidget *ww_widget;
    GtkWidget *gw_search_box;
    GtkWidget *gw_web_combo;
    GtkWidget *gw_img_view;
    GtkWidget *gw_sett_button;
    GtkWidget *gw_search_entry;
    GtkWidget *gw_search_button;
    GtkWidget *gw_nav_box;
    GtkWidget *gw_nav_prev;
    GtkWidget *gw_nav_next;
    GtkWidget *gw_nav_entry;
    GtkWidget *gw_count_label;
    GtkWidget *gw_add_sltd_box;
    GtkWidget *gw_add_sltd_button;
    GtkWidget *gw_selected_combo;

    if ((ww_widget = malloc (sizeof (WebWidget))) == NULL)
        err (EXIT_FAILURE, NULL);

    ww_widget->s_cfg_file = strdup (s_cfg_file);

    gw_web_combo = webwidget_combobox_create (st_settings);
    gw_img_view  = webwidget_imgview_create ();

    gw_count_label = gtk_label_new ("Search query:   Found 0 results");

    /* Search box */
    gw_search_box    = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_sett_button   = gtk_button_new_with_label ("Settings");
    gw_search_entry  = gtk_search_entry_new ();
    gw_search_button = gtk_button_new_with_label ("Search");
    g_signal_connect_swapped (gw_sett_button,   "clicked",
            G_CALLBACK (event_settings_pressed), ww_widget);
    g_signal_connect_swapped (gw_search_entry,  "activate",
            G_CALLBACK (event_search_pressed),   ww_widget);
    g_signal_connect_swapped (gw_search_button, "clicked",
            G_CALLBACK (event_search_pressed),   ww_widget);

    gtk_box_pack_start (GTK_BOX (gw_search_box),
                        gw_web_combo,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_search_box),
                        gw_sett_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_search_box),
                        gw_search_entry,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (gw_search_box),
                        gw_search_button,
                        FALSE, FALSE, 4);

    /* Navigation box */
    gw_nav_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign (gw_nav_box, GTK_ALIGN_CENTER);
    gw_nav_prev  = gtk_button_new_with_label ("Prev page");
    gw_nav_next  = gtk_button_new_with_label ("Next page");
    gw_nav_entry = gtk_entry_new ();
    gtk_entry_set_width_chars (GTK_ENTRY (gw_nav_entry), 4);
    gtk_entry_set_text (GTK_ENTRY (gw_nav_entry), "1");
    gtk_box_pack_start (GTK_BOX (gw_nav_box),
                        gw_nav_prev,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_nav_box),
                        gw_nav_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_nav_box),
                        gw_nav_next,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_nav_prev, "clicked",
            G_CALLBACK (event_prev_pressed), ww_widget);
    g_signal_connect_swapped (gw_nav_next, "clicked",
            G_CALLBACK (event_next_pressed), ww_widget);
    g_signal_connect (gw_nav_entry, "activate",
            G_CALLBACK (event_nav_entry_act), ww_widget);

    /* Add selected images box */
    gw_add_sltd_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_add_sltd_button = gtk_button_new_with_label (
            "Add selected images to list");
    gw_selected_combo = webwidget_selected_combobox_create ();

    gtk_box_pack_start (GTK_BOX (gw_add_sltd_box),
                        gw_add_sltd_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_add_sltd_box),
                        gw_selected_combo,
                        TRUE, TRUE, 4);
    g_signal_connect_swapped (gw_add_sltd_button, "clicked",
            G_CALLBACK (event_add_selected_pressed), ww_widget);

    ww_widget->gw_search_box     = gw_search_box;
    ww_widget->gw_combo          = gw_web_combo;
    ww_widget->gw_entry          = gw_search_entry;
    ww_widget->gw_nav_entry      = gw_nav_entry;
    ww_widget->gw_img_view       = gw_img_view;
    ww_widget->gw_nav_box        = gw_nav_box;
    ww_widget->gw_selected_combo = gw_selected_combo;
    ww_widget->gw_selected_box   = gw_add_sltd_box;
    ww_widget->gw_count_label    = gw_count_label;
    ww_widget->s_query           = NULL;
    ww_widget->i_page            = 0;
    ww_widget->i_per_page        = IMGS_ON_PAGE;
    ww_widget->i_found_cnt       = 0;

    return ww_widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save selected wallpapers in user's app share folder.
 */
GList *
save_selected_wallpapers (GtkWidget *gw_dialog,
                          WebWidget *ww_widget)
{
    GList *gl_items = NULL; /* List of images selected to download */
    GList *gl_res   = NULL; /* Result list od downloaded wallpapers */

    gl_items = sel_combo_get_list (ww_widget->gw_selected_combo);
    gl_res = download_progress_window (GTK_WINDOW (gw_dialog), gl_items);
    g_list_free_full (gl_items, (GDestroyNotify) searchitem_free);

    return gl_res;
}
/*----------------------------------------------------------------------------*/

