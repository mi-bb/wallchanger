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
#include "imgs.h"
#include "fourstrings.h"
#include "webwidget_c.h"
#include "webpexels.h"
#include "webpixbay.h"
#include "webwallhaven.h"
#include "webabyss.h"
#include "webflickr.h"
#include "chquery.h"
#include "webwidget.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   NAME_LEN
 * @brief Lenght of name in download progress window.
 *
 * @def   SEL_NAME_LEN
 * @brief Lenght of name in selected images combobox.
 */
#define NAME_LEN     40
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
    WW_COMBO_LOGO,  /**< Service logo pixbuf */
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
    size_t      ui_nlen  = 0;    /* Length of file name */
    char s_nbuff[NAME_LEN * 4 + 4];

    memset (s_nbuff, '\0', NAME_LEN * 4 + 4);
    memset (s_nbuff, ' ',  NAME_LEN + 10);

    /* Getting wallpaper save path and checking it's permissions, creating if
     * it doesn't exist */
    s_wpdir = cfgfile_get_app_wallpapers_path ();

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
        gtk_tree_model_get (model,          &iter,
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

        gtk_list_store_set (gls_slstore,    &gti_iter,
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
    const char *s_format_count = "Found %d images";
    char       *s_markup       = NULL;
    char        s_entry[128];

    s_markup = g_markup_printf_escaped (s_format_count,
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
 * @param[in] s_id       Image id to check
 * @return    1 if exists, 0 if not
 */
static int
sel_combo_check_exist (GtkTreeModel *gtm_model,
                       const char   *s_id)
{
    char       *s_read_id = NULL;
    gboolean    b_res     = FALSE;
    GtkTreeIter gti_iter;

    b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {

        gtk_tree_model_get (gtm_model,      &gti_iter,
                            WW_SELCOMBO_ID, &s_read_id,
                            -1);
        if (strcmp (s_id, s_read_id) == 0) {
            free (s_read_id);
            return 1;
        }
        free (s_read_id);
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
        gtk_tree_model_get (gtm_model,             &gti_iter,
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
    FourStrings *fs_data = NULL; /* Four strings for API key data */
    int          i_id    = 0;    /* Service id */

    i_id    = ww_widget->i_active_service;
    fs_data = fourstrings_new ();

    combo_get_active_strings (ww_widget->gw_combo, fs_data);

    switch (i_id) {
        case WEB_WIDGET_PEXELS:
            pexels_search (ww_widget, fs_data);
            break;
        case WEB_WIDGET_PIXBAY:
            pixbay_search (ww_widget, fs_data);
            break;
        case WEB_WIDGET_WALLHAVEN:
            wallhaven_search (ww_widget, fs_data);
            break;
        case WEB_WIDGET_WALLABYSS:
            wallpaperabyss_search (ww_widget, fs_data);
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

    if (!check_unicode (s_query, "Search query is not a valid unicode text"))
        return;
    else if (str_is_empty_msg (s_query, "Empty search query"))
        return;
    else if (!str_is_alnum_or_space (
                s_query, "Sorry, only alphanumerich characters allowed"))
        return;

    if (ww_widget->s_query != NULL)
        free (ww_widget->s_query);

    ww_widget->s_query = strdup (s_query);
    ww_widget->i_page = 1;
    search_web (ww_widget);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search options button pressed.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_search_opts_pressed (WebWidget *ww_widget)
{
    char *s_search_opts = NULL; /* Search options string */
    int   i_id          = 0;    /* Service id */

    i_id    = ww_widget->i_active_service;

    switch (i_id) {
        case WEB_WIDGET_PEXELS:
            s_search_opts = pexels_search_opts_dialog (ww_widget);
            break;
        case WEB_WIDGET_PIXBAY:
            s_search_opts = pixbay_search_opts_dialog (ww_widget);
            break;
        case WEB_WIDGET_WALLHAVEN:
            s_search_opts = wallhaven_search_opts_dialog (ww_widget);
            break;
        case WEB_WIDGET_WALLABYSS:
            s_search_opts = wallpaperabyss_search_opts_dialog (ww_widget);
            break;
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
            /* flickr_search_opts_dialog (ww_widget); */
            break;
#endif
        default:
            break;
    }
    if (s_search_opts != NULL &&
        strcmp (s_search_opts, ww_widget->s_search_opts) != 0) {

        free (ww_widget->s_search_opts);
        ww_widget->s_search_opts = strdup (s_search_opts);
    }
#ifdef DEBUG
            printf ("now opts : %s\n", ww_widget->s_search_opts);
#endif
    free (s_search_opts);
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
    FourStrings *fs_data = NULL; /* Four strings for API key data */
    int          i_err   = 0;    /* Error output */
    int          i_id    = 0;    /* Service id */
    int          i_res   = 0;    /* Dialog result */

    i_id    = ww_widget->i_active_service;
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
        case WEB_WIDGET_PIXBAY:
            combo_get_active_strings (ww_widget->gw_combo, fs_data);

            i_res = pixbay_settings_dialog (fs_data);

            if (i_res == GTK_RESPONSE_ACCEPT) {
                combo_set_active_str (ww_widget->gw_combo, WW_COMBO_STR_1,
                                      fs_data->s_str1);

                i_err = setts_update_pixbay_api (ww_widget->s_cfg_file,
                                                 fs_data->s_str1);
                if (i_err != ERR_OK) {
                    message_dialog_error (NULL, err_get_message (i_err));
                }
            }
            break;
        case WEB_WIDGET_WALLHAVEN:
            combo_get_active_strings (ww_widget->gw_combo, fs_data);

            i_res = wallhaven_settings_dialog (fs_data);

            if (i_res == GTK_RESPONSE_ACCEPT) {
                combo_set_active_str (ww_widget->gw_combo, WW_COMBO_STR_1,
                                      fs_data->s_str1);

                i_err = setts_update_wallhaven_api (ww_widget->s_cfg_file,
                                                    fs_data->s_str1);
                if (i_err != ERR_OK) {
                    message_dialog_error (NULL, err_get_message (i_err));
                }
            }
            break;
        case WEB_WIDGET_WALLABYSS:
            combo_get_active_strings (ww_widget->gw_combo, fs_data);

            i_res = wallpaperabyss_settings_dialog (fs_data);

            if (i_res == GTK_RESPONSE_ACCEPT) {
                combo_set_active_str (ww_widget->gw_combo, WW_COMBO_STR_1,
                                      fs_data->s_str1);

                i_err = setts_update_wallabyss_api (ww_widget->s_cfg_file,
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
    GtkListStore *gls_slstore;        /* Combobox ListStore */
    GtkTreeModel *gtm_model;          /* Iconview tree model */
    GtkTreeModel *gtm_combo_model;    /* Combobox tree model */
    GdkPixbuf    *gp_pbuf;            /* Thumbnail pixbuf */
    GdkPixbuf    *gp_pbuf2;           /* Smaller thumbnail pixbuf */
    GtkTreeIter   gti_iter;           /* TreeIter */
    GtkTreeIter   gti_iters;          /* TreeIter */
    GtkTreePath  *gtp_path;           /* TreePath */
    GList        *gl_selected = NULL; /* List of selected items */
    GList        *gl_item     = NULL; /* List item for iteration */
    char         *s_disp_name = NULL; /* Image display name */
    char         *s_file_name = NULL; /* Image file name */
    char         *s_image_url = NULL; /* Image url */
    char         *s_id        = NULL; /* Image id */
    int           i_w         = 0;    /* Thumbnail width */
    int           i_h         = 0;    /* Thumbnail height */
    float         f_w         = 0;    /* Smaller thumbnail width */
    float         f_h         = 0;    /* Smaller thumbnail height */
    char          s_dname[SEL_NAME_LEN * 4 + 4]; /* For shorter name in combo */

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
                                WEB_COL_ID,        &s_id,
                                WEB_COL_DISP_NAME, &s_disp_name, 
                                WEB_COL_FILE_NAME, &s_file_name, 
                                WEB_COL_IMAGE_URL, &s_image_url, 
                                -1);

            if (sel_combo_check_exist (gtm_combo_model, s_id) == 0) {

                i_w = gdk_pixbuf_get_width  (gp_pbuf);
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
                                    WW_SELCOMBO_ID,        s_id,
                                    WW_SELCOMBO_DISP_NAME, s_dname,
                                    WW_SELCOMBO_FILE_NAME, s_file_name,
                                    WW_SELCOMBO_IMAGE_URL, s_image_url,
                                    -1);
                g_object_unref(gp_pbuf2);
            }
            free (s_id);
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
 * @brief  Refresh string with search options for active seach service.
 *
 * @param[in,out] ww_widget  WebWidget item
 * @return        none
 */
static void
refresh_service_opts (WebWidget *ww_widget)
{
    Setting *st_settings   = NULL;
    Setting *st_item       = NULL;
    char    *s_name        = NULL;
    char    *s_search_opts = NULL;
    int      i_err         = 0;

    s_name = combo_get_active_str (ww_widget->gw_combo, WW_COMBO_NAME);
    str_append (&s_name, "_opts");
#ifdef DEBUG
    printf ("%s\n", s_name);
#endif

    st_settings = setts_read (ww_widget->s_cfg_file, &i_err);
    st_item     = setting_get_child (st_settings);
    st_item     = setting_get_child (settings_find (st_item, s_name));

    if (st_item != NULL) {
        switch (ww_widget->i_active_service) {
            case WEB_WIDGET_PEXELS:
                s_search_opts = search_opts_to_str (st_item);
                break;
            case WEB_WIDGET_PIXBAY:
                s_search_opts = search_opts_to_str (st_item);
                break;
            case WEB_WIDGET_WALLHAVEN:
                s_search_opts = search_opts_to_str (st_item);
                break;
            case WEB_WIDGET_WALLABYSS:
                s_search_opts = search_opts_to_str (st_item);
                break;
            default:
                break;
        }
    }
    settings_free_all (st_settings);
    free (s_name);

    if (s_search_opts != NULL &&
        strcmp (s_search_opts, ww_widget->s_search_opts) != 0) {

        free (ww_widget->s_search_opts);
        ww_widget->s_search_opts = strdup (s_search_opts);
    }
    else {
        free (ww_widget->s_search_opts);
        ww_widget->s_search_opts = strdup ("");
    }
#ifdef DEBUG
    printf ("now opts : %s\n", ww_widget->s_search_opts);
#endif
    free (s_search_opts);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed cobo box with image search service, sets active service
 *         i_active_service value in WebWidget.
 *
 * @param[in]  gw_combo   Combobox which sent signal
 * @param[out] ww_widget  WebWidget item
 * @return     none
 */
static void
event_service_combo_changed (GtkComboBox *gw_combo,
                             WebWidget   *ww_widget)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    if (gtk_combo_box_get_active_iter (gw_combo, &iter)) {

        model = gtk_combo_box_get_model (gw_combo);
        gtk_tree_model_get (model, &iter,
                WW_COMBO_ID, &ww_widget->i_active_service, -1);

        refresh_service_opts (ww_widget);

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
    GdkPixbuf       *gp_logo       = NULL; /* Service logo */
    const char      *s_pexels_api  = NULL; /* Pexels API key */
    const char      *s_pixbay_api  = NULL; /* Pixbay API key */
    const char      *s_wallha_api  = NULL; /* Wallhaven API key */
    const char      *s_wallab_api  = NULL; /* Wallpaer Abyss API key */
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

    /* Liststore for service data */
    list_store = gtk_list_store_new (WW_COMBO_CNT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     GDK_TYPE_PIXBUF);
    /* Get Pexels API string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETT_PEXELS_API));

    s_pexels_api = st_s1 == NULL ? "" : setting_get_string (st_s1);

    gp_logo = get_image (W_LOGO_PEXELS);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        WW_COMBO_ID,    WEB_WIDGET_PEXELS,
                        WW_COMBO_NAME,  ww_name (WEB_WIDGET_PEXELS),
                        WW_COMBO_STR_1, s_pexels_api,
                        WW_COMBO_STR_2, "",
                        WW_COMBO_STR_3, "",
                        WW_COMBO_STR_4, "",
                        WW_COMBO_LOGO,  gp_logo,
                        -1);
    g_object_unref (gp_logo);

    /* Get Pixbay API string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETT_PIXBAY_API));

    s_pixbay_api = st_s1 == NULL ? "" : setting_get_string (st_s1);

    gp_logo = get_image (W_LOGO_PIXBAY);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store,     &iter,
                        WW_COMBO_ID,    WEB_WIDGET_PIXBAY,
                        WW_COMBO_NAME,  ww_name (WEB_WIDGET_PIXBAY),
                        WW_COMBO_STR_1, s_pixbay_api,
                        WW_COMBO_STR_2, "",
                        WW_COMBO_STR_3, "",
                        WW_COMBO_STR_4, "",
                        WW_COMBO_LOGO,  gp_logo,
                        -1);
    g_object_unref (gp_logo);

    /* Get Wallhaven API string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETT_WALLHAVEN_API));

    s_wallha_api = st_s1 == NULL ? "" : setting_get_string (st_s1);

    gp_logo = get_image (W_LOGO_WALLHAVEN);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store,     &iter,
                        WW_COMBO_ID,    WEB_WIDGET_WALLHAVEN,
                        WW_COMBO_NAME,  ww_name (WEB_WIDGET_WALLHAVEN),
                        WW_COMBO_STR_1, s_wallha_api,
                        WW_COMBO_STR_2, "",
                        WW_COMBO_STR_3, "",
                        WW_COMBO_STR_4, "",
                        WW_COMBO_LOGO,  gp_logo,
                        -1);
    g_object_unref (gp_logo);

    /* Get Wallpaper Abyss API string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETT_WALLABYSS_API));

    s_wallab_api = st_s1 == NULL ? "" : setting_get_string (st_s1);
    gp_logo = get_image (W_LOGO_WALLABYSS);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store,     &iter,
                        WW_COMBO_ID,    WEB_WIDGET_WALLABYSS,
                        WW_COMBO_NAME,  ww_name (WEB_WIDGET_WALLABYSS),
                        WW_COMBO_STR_1, s_wallab_api,
                        WW_COMBO_STR_2, "",
                        WW_COMBO_STR_3, "",
                        WW_COMBO_STR_4, "",
                        WW_COMBO_LOGO,  gp_logo,
                        -1);
    g_object_unref (gp_logo);
#ifdef HAVE_FLICKCURL
    /* Get Flickr API key string */
    st_s1 = setting_find_child (st_settings,
                                get_setting_name (SETT_FLICKR_CLKEY));
    /* Get Flickr API secret string */
    st_s2 = setting_find_child (st_settings,
                                get_setting_name (SETT_FLICKR_CLSEC));
    /* Get Flickr API access token */
    st_s3 = setting_find_child (st_settings,
                                get_setting_name (SETT_FLICKR_ACTOK));
    /* Get Flickr API access token */
    st_s4 = setting_find_child (st_settings,
                                get_setting_name (SETT_FLICKR_ACSEC));

    s_flickr_key  = st_s1 == NULL ? "" : setting_get_string (st_s1);
    s_flickr_sec  = st_s2 == NULL ? "" : setting_get_string (st_s2);
    s_flickr_acct = st_s3 == NULL ? "" : setting_get_string (st_s3);
    s_flickr_accs = st_s4 == NULL ? "" : setting_get_string (st_s4);

    gp_logo = get_image (W_LOGO_FLICKR);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store,     &iter,
                        WW_COMBO_ID,    WEB_WIDGET_FLICKR,
                        WW_COMBO_NAME,  ww_name (WEB_WIDGET_FLICKR),
                        WW_COMBO_STR_1, s_flickr_key,
                        WW_COMBO_STR_2, s_flickr_sec,
                        WW_COMBO_STR_3, s_flickr_acct,
                        WW_COMBO_STR_4, s_flickr_accs,
                        WW_COMBO_LOGO,  gp_logo,
                        -1);
    g_object_unref (gp_logo);
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
                                     G_TYPE_STRING,
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
 * @brief  Image on preview list activated, get image info and set image
 *         info widgets.
 *
 * @param[in]  iconview   IconView which sent signal
 * @param[in]  path       TreePath of active icon
 * @param[out] ww_widget  WebWidget item
 */
static void
event_imgview_activated (GtkIconView *iconview,
                         GtkTreePath *path,
                         WebWidget   *ww_widget)
{
    GtkTreeModel *gtm_model;
    GdkPixbuf    *gp_logo        = NULL;
    char         *s_disp_name    = NULL;
    char         *s_file_name    = NULL;
    char         *s_image_url    = NULL;
    char         *s_page_url     = NULL;
    char         *s_image_author = NULL;
    int           i_w            = 0;
    int           i_h            = 0;
    GtkTreeIter   gti_iter;
    char s_dim[128];
    s_dim[0] = '\0';

    gtm_model = gtk_icon_view_get_model (iconview);

    if (gtk_tree_model_get_iter (gtm_model, &gti_iter, path)) {
        gtk_tree_model_get (gtm_model, &gti_iter,
                            WEB_COL_WIDTH,     &i_w,
                            WEB_COL_HEIGHT,    &i_h,
                            WEB_COL_DISP_NAME, &s_disp_name,
                            WEB_COL_AUTHOR,    &s_image_author,
                            WEB_COL_FILE_NAME, &s_file_name,
                            WEB_COL_PAGE_URL,  &s_page_url,
                            WEB_COL_IMAGE_URL, &s_image_url,
                            -1);
        gtk_label_set_text (GTK_LABEL (ww_widget->gw_ii_author_label),
                            s_image_author != NULL ? s_image_author : "");
        gtk_link_button_set_uri (GTK_LINK_BUTTON (ww_widget->gw_ii_page_link),
                                 s_page_url != NULL ? s_page_url : "");
        if (i_w && i_h) {
            sprintf (s_dim, "[%dx%d]", i_w, i_h);
        }
        gtk_label_set_text (GTK_LABEL (ww_widget->gw_ii_dim_label), s_dim);

        gtk_image_clear (GTK_IMAGE (ww_widget->gw_ii_simage));
        if (ww_widget->i_active_service == WEB_WIDGET_PEXELS)
            gp_logo = get_image (W_LOGO_PEXELS);
        else if (ww_widget->i_active_service == WEB_WIDGET_PIXBAY)
            gp_logo = get_image (W_LOGO_PIXBAY);
        else if (ww_widget->i_active_service == WEB_WIDGET_WALLHAVEN)
            gp_logo = get_image (W_LOGO_WALLHAVEN);
        else if (ww_widget->i_active_service == WEB_WIDGET_WALLABYSS)
            gp_logo = get_image (W_LOGO_WALLABYSS);
#ifdef HAVE_FLICKCURL
        else if (ww_widget->i_active_service == WEB_WIDGET_FLICKR)
            gp_logo = get_image (W_LOGO_FLICKR);
#endif
        if (gp_logo != NULL) {
            gtk_image_set_from_pixbuf (GTK_IMAGE (ww_widget->gw_ii_simage),
                                       gp_logo);
        }
        g_object_unref (gp_logo);
        free (s_disp_name);
        free (s_image_author);
        free (s_file_name);
        free (s_image_url);
        free (s_page_url);
    }
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
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gw_icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (list_store));

    gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (gw_icon_view),
                                      GTK_SELECTION_MULTIPLE);
    /* gtk_icon_view_set_text_column (GTK_ICON_VIEW (gw_icon_view),
                                   WEB_COL_DISP_NAME); */
    gtk_icon_view_set_markup_column (GTK_ICON_VIEW (gw_icon_view),
                                     WEB_COL_MARKUP);
    gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (gw_icon_view),
                                     WEB_COL_PIXBUF);
    gtk_icon_view_set_item_width (GTK_ICON_VIEW (gw_icon_view), 210);
    gtk_icon_view_set_columns (GTK_ICON_VIEW (gw_icon_view), 3);

    return gw_icon_view;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create widget with image info widgets.
 *
 * @return  Result widget
 */
static GtkWidget *
webwidget_imageinfo_create (WebWidget *ww_widget)
{
    GtkWidget *gw_widget;

    GtkWidget *gw_service_image;
    GtkWidget *gw_author_label;
    GtkWidget *gw_dim_label;
    GtkWidget *gw_image_link;
    GtkWidget *gw_grid;

    gw_grid = gtk_grid_new ();

    gtk_widget_set_halign (gw_grid, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing (GTK_GRID (gw_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_grid), 16);

    gw_service_image = gtk_image_new ();
    gw_author_label = gtk_label_new (NULL);
    gw_dim_label = gtk_label_new (NULL);
    gw_image_link = gtk_link_button_new ("Image page");

    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Image provided by:"),
                     0,0,2,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_service_image,
                     0,1,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_image_link,
                     1,1,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Author:"),
                     2,0,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_author_label,
                     2,1,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Dimensions:"),
                     3,0,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_dim_label,
                     3,1,1,1);

    ww_widget->gw_ii_simage       = gw_service_image;
    ww_widget->gw_ii_author_label = gw_author_label;
    ww_widget->gw_ii_page_link    = gw_image_link;
    ww_widget->gw_ii_dim_label    = gw_dim_label;

    gw_widget = gw_grid;

    return gw_widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free WebWidget item.
 */
void
webwidget_free (WebWidget *ww_widget)
{
    free (ww_widget->s_query);
    free (ww_widget->s_search_opts);
    free (ww_widget->s_cfg_file);
    free (ww_widget->s_wallp_dir);
    free (ww_widget);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init WebWidget item.
 *
 * @param[out] ww_widget  WebWidget item
 * @return     none
 */
static void
webwidget_init (WebWidget *ww_widget)
{
    ww_widget->s_query          = NULL;
    ww_widget->s_search_opts    = NULL;
    ww_widget->s_cfg_file       = NULL;
    ww_widget->s_wallp_dir      = NULL;
    ww_widget->i_page           = 0;
    ww_widget->i_found_cnt      = 0;
    ww_widget->i_active_service = 0;
    ww_widget->i_thumb_quality  = 0;
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
    GtkWidget *gw_search_opts_btn;
    GtkWidget *gw_nav_box;
    GtkWidget *gw_nav_prev;
    GtkWidget *gw_nav_next;
    GtkWidget *gw_nav_entry;
    GtkWidget *gw_count_label;
    GtkWidget *gw_add_sltd_box;
    GtkWidget *gw_add_sltd_button;
    GtkWidget *gw_selected_combo;
    GtkWidget *gw_img;
    GdkPixbuf *gp_pbuf = NULL;
    Setting   *st_sett = NULL;

    if ((ww_widget = malloc (sizeof (WebWidget))) == NULL)
        err (EXIT_FAILURE, NULL);

    webwidget_init (ww_widget);

    cachequery_delete_older_than (ww_name (WEB_WIDGET_PEXELS),    1);
    cachequery_delete_older_than (ww_name (WEB_WIDGET_PIXBAY),    1);
    cachequery_delete_older_than (ww_name (WEB_WIDGET_WALLHAVEN), 1);
    cachequery_delete_older_than (ww_name (WEB_WIDGET_WALLABYSS), 1);
#ifdef HAVE_FLICKCURL
    cachequery_delete_older_than (ww_name (WEB_WIDGET_FLICKR),    1);
#endif

    if ((st_sett = setting_find_child (st_settings,
                   get_setting_name (SETT_THUMB_QUALITY))) != NULL) {
        ww_widget->i_thumb_quality = (int) setting_get_int (st_sett);
    }

    ww_widget->s_cfg_file   = strdup (s_cfg_file);
    ww_widget->s_wallp_dir  = cfgfile_get_app_wallpapers_path ();
    ww_widget->gw_ii_widget = webwidget_imageinfo_create (ww_widget);

    gw_web_combo = webwidget_combobox_create (st_settings);
    gw_img_view  = webwidget_imgview_create ();

    g_signal_connect (gw_web_combo, "changed",
            G_CALLBACK (event_service_combo_changed), ww_widget);
    g_signal_connect (gw_img_view, "item-activated",
            G_CALLBACK (event_imgview_activated), ww_widget);

    gw_count_label = gtk_label_new ("Found 0 images");

    /* Search box */
    gw_search_box      = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_search_entry    = gtk_search_entry_new ();
    gw_search_button   = gtk_button_new_with_label ("Search");
    gw_search_opts_btn = gtk_button_new_with_label ("Search opts");
    //gw_sett_button     = gtk_button_new_with_label ("Settings");
    gw_sett_button     = gtk_button_new ();
    gtk_widget_set_tooltip_text (gw_sett_button, "Service settings");
    gp_pbuf = get_image (W_ICON_SETTING);
    gw_img = gtk_image_new_from_pixbuf (gp_pbuf);
    gtk_button_set_image (GTK_BUTTON (gw_sett_button), gw_img);
    g_object_unref (gp_pbuf);

    g_signal_connect_swapped (gw_sett_button, "clicked",
            G_CALLBACK (event_settings_pressed), ww_widget);
    g_signal_connect_swapped (gw_search_entry, "activate",
            G_CALLBACK (event_search_pressed), ww_widget);
    g_signal_connect_swapped (gw_search_button, "clicked",
            G_CALLBACK (event_search_pressed), ww_widget);
    g_signal_connect_swapped (gw_search_opts_btn, "clicked",
            G_CALLBACK (event_search_opts_pressed), ww_widget);

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
    gtk_box_pack_start (GTK_BOX (gw_search_box),
                        gw_search_opts_btn,
                        FALSE, FALSE, 4);

    /* Navigation box */
    gw_nav_box = gtk_grid_new ();
    /*gw_nav_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);*/
    gtk_widget_set_halign (gw_nav_box, GTK_ALIGN_CENTER);
    gw_nav_prev  = gtk_button_new_with_label ("Prev page");
    gw_nav_next  = gtk_button_new_with_label ("Next page");
    gw_nav_entry = gtk_entry_new ();
    gtk_entry_set_width_chars (GTK_ENTRY (gw_nav_entry), 4);
    gtk_entry_set_text (GTK_ENTRY (gw_nav_entry), "1");
    gtk_grid_set_row_spacing (GTK_GRID (gw_nav_box), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_nav_box), 8);
    gtk_grid_attach (GTK_GRID (gw_nav_box), gw_nav_prev,
                     0,0,1,1);
    gtk_grid_attach (GTK_GRID (gw_nav_box), gw_nav_entry,
                     1,0,1,1);
    gtk_grid_attach (GTK_GRID (gw_nav_box), gw_nav_next,
                     2,0,1,1);

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
    ww_widget->i_active_service  = WEB_WIDGET_PEXELS;
    ww_widget->s_search_opts     = strdup ("");

    refresh_service_opts (ww_widget);

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
    gl_res   = download_progress_window (GTK_WINDOW (gw_dialog), gl_items);
    g_list_free_full (gl_items, (GDestroyNotify) searchitem_free);

    return gl_res;
}
/*----------------------------------------------------------------------------*/

