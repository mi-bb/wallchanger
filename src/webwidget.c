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

#include "../config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif

#include "webwidget.h"

/*----------------------------------------------------------------------------*/
/**
 * @brief  Services combobox columns
 */
enum e_services_columns {
    WW_COMBO_ID,    /**< Service id */
    WW_COMBO_NAME,  /**< Service name */
    WW_COMBO_STR_1, /**< Some needed string */
    WW_COMBO_CNT    /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Image list columns
 */
enum e_img_list_columns {
    WEB_COL_NAME,      /**< Name of image */
    WEB_COL_MARKUP,    /**< Markup string */
    WEB_COL_PIXBUF,    /**< Thumbnail pixbuf */
    WEB_COL_ID,        /**< Image id */
    WEB_COL_URL,       /**< Full size image url */
    WEB_COL_URL_THUMB, /**< Thumbnail url */
    WEB_COL_WIDTH,     /**< Image width */
    WEB_COL_HEIGHT,    /**< Image height */
    WEB_NUM_CNT        /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Selected to download combobox columns
 */
enum e_selected_columns {
    WW_SELCOMBO_ID,     /**< Image id */
    WW_SELCOMBO_PIXBUF, /**< Thumbnail pixbuf */
    WW_SELCOMBO_NAME,   /**< Name of image */
    WW_SELCOMBO_URL,    /**< Full size image url */
    WW_SELCOMBO_WIDTH,  /**< Image width */
    WW_SELCOMBO_HEIGHT, /**< Image height */
    WW_SELCOMBO_CNT     /**< Column count */
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
 * @brief  Prepare file name from directory path and Pexels photo url.
 *
 * @param[in] s_dir   Directory for file write
 * @param[in] si_item SearchItem with url
 * @result    File path
 */
static char *
pexels_prepare_file_name (const char *s_dir,
                          SearchItem *si_item)
{
    char *s_res = NULL; /* Result name */
    char *s_ext = NULL; /* Extension search */

    s_ext = strrchr (si_item->url, '.');
    s_res = str_comb (s_dir, "/");
    str_append (&s_res, si_item->name);
    str_append (&s_res, s_ext);

    return s_res;
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
    GtkWidget  *gw_window;
    GtkWidget  *gw_label;
    GtkWidget  *gw_progress;
    GtkWidget  *gw_box_main;
    GtkWidget  *gw_stop_button;
    SearchItem *si_item;
    GList      *gl_item  = NULL;
    GList      *gl_res   = NULL;
    const char *s_format = "<span style=\"italic\">%s</span>";
    char       *s_markup = NULL;
    char       *s_wpdir  = NULL;
    char       *s_fn     = NULL;
    char       *s_err    = NULL;
    float       f_step   = 0;
    float       f_frac   = 0;
    int         i_stop   = 0;
    int         i_perm   = 0;

    s_wpdir = cfgfile_get_app_wallpaper_path ();
    i_perm  = dir_check_permissions (s_wpdir);

    if (i_perm == ERR_FILE ||
       (i_perm == ERR_FILE_EX && dir_create_with_subdirs (s_wpdir) != ERR_OK)) {
        message_dialog_error (NULL, err_get_message (i_perm));
        free (s_wpdir);
        return NULL;
    }

    f_step  = (float) 1.0 / (float) g_list_length (gl_item_list);
    f_frac  = 0;
    gl_item = gl_item_list;

    gw_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_transient_for (GTK_WINDOW (gw_window), gw_parent);
    gtk_window_set_modal (GTK_WINDOW (gw_window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (gw_window), FALSE);

    gw_label = gtk_label_new (NULL);
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

    while (gl_item != NULL) {
        if (i_stop == 1)
            break;

        f_frac  += f_step;
        si_item  = gl_item->data;
        s_fn     = pexels_prepare_file_name (s_wpdir, si_item);
        s_markup = g_markup_printf_escaped (s_format, si_item->name);

        gtk_label_set_markup (GTK_LABEL (gw_label), s_markup);
        g_free (s_markup);

        while (gtk_events_pending ())
            gtk_main_iteration ();
#ifdef DEBUG
        printf ("downloading:%s\nto:%s\n", si_item->url, s_fn);
#endif
        urldata_get_to_file (si_item->url, s_fn, &s_err);
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
 * @brief  Create pixbuf from raw data.
 *
 * @param[in] s_data  Buffer with image data
 * @param[in] i_size  Buffer size
 * @return    Pixbuf with image
 */
static GdkPixbuf *
pixbuf_from_data (const unsigned char *s_data,
                  const gssize         i_size)
{
    GdkPixbuf    *g_pbuf = NULL; /* Return pixbuf */
    GInputStream *stream;        /* Stream for passing data to pixbuf */

    stream = g_memory_input_stream_new ();
    g_memory_input_stream_add_data (G_MEMORY_INPUT_STREAM (stream),
                                    s_data, i_size, NULL);
    g_pbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
    g_object_unref (stream);
    return g_pbuf;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf from raw data
 *
 * @param[in] s_url  Url where should be image for pixbuf
 * @return    Pixbuf with image
 */
static GdkPixbuf *
pixbuf_from_url (const char *s_url)
{
    GdkPixbuf *g_pbuf  = NULL; /* Pixbuf to return */
    UrlData   *ud_data = NULL; /* Urldata for getting image data */
    char      *s_txt   = NULL; /* For error text */

    ud_data = urldata_get_data (s_url);

    if (ud_data->errbuf != NULL) {
        s_txt = str_comb ("Getting image error:\n",
                          ud_data->errbuf);
        message_dialog_error (NULL, s_txt);
        free (s_txt);
    }
    else if (urldata_full (ud_data)) {
        g_pbuf = pixbuf_from_data ((unsigned char *) ud_data->buffer,
                                   (gssize) ud_data->size);
    }
    urldata_free (ud_data);
    return g_pbuf;
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
 * @brief  Get string value from Combobox's active i_col column.
 *
 * @param[in] gw_combo  Combobox
 * @param[in] i_col     Column with data
 * @return    String value
 */
static char *
combo_get_active_str (GtkWidget *gw_combo,
                      const int  i_col)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    char *s_ret = NULL;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, i_col, &s_ret, -1);
    }
    return s_ret;
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

        gtk_list_store_append (gls_slstore, &gti_iter);
        gtk_list_store_set (gls_slstore, &gti_iter, i_col, s_val, -1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add image based on si_item data to icon view.
 *
 * @param[out] gw_iconview  IconView to add image
 * @param[in]  si_item      SearchItem with image data
 * @return     none  
 */
static void
add_searchitem_to_img_view (GtkWidget  *gw_iconview,
                            SearchItem *si_item)
{
    GtkListStore *list_store;
    GdkPixbuf    *gp_pbuf;
    GtkTreeIter   iter;

    list_store = GTK_LIST_STORE (gtk_icon_view_get_model (
                GTK_ICON_VIEW (gw_iconview)));

    if (si_item->thumb_url == NULL)
        return;
    if ((gp_pbuf = pixbuf_from_url (si_item->thumb_url)) != NULL) {
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           WEB_COL_NAME,      si_item->name,
                           WEB_COL_MARKUP,    si_item->markup,
                           WEB_COL_PIXBUF,    gp_pbuf,
                           WEB_COL_ID,        si_item->id,
                           WEB_COL_URL,       si_item->url,
                           WEB_COL_URL_THUMB, si_item->thumb_url,
                           WEB_COL_WIDTH,     si_item->width,
                           WEB_COL_HEIGHT,    si_item->height,
                           -1);
        g_object_unref(gp_pbuf);
        gp_pbuf = NULL;
    }
    while (gtk_events_pending ())
        gtk_main_iteration ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Pexels service settings.
 *
 * @param[in] s_cfg_file  Config file path
 * @return    Pexels API key or null
 */
static char *
pexels_settings_dialog (const char *s_cfg_file)
{
    GtkWidget *gw_dialog;       /* Pexels settings dialog */
    GtkWidget *gw_content_box;  /* Dialog's box */
    GtkWidget *gw_api_entry;    /* Entry for API key */
    Setting   *st_setts = NULL; /* For settings */
    Setting   *st_st    = NULL; /* For particular setting */
    char      *s_result = NULL; /* Result string with API key */
    int        i_err    = 0;    /* Error output */
    int        i_res    = 0;    /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons ("Pexels configuration",
                                             NULL,
                                             flags,
                                             "_OK",
                                             GTK_RESPONSE_ACCEPT,
                                             "_Cancel",
                                             GTK_RESPONSE_REJECT,
                                             NULL);

    gw_content_box = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    gw_api_entry = gtk_entry_new ();

    st_setts = setts_read (s_cfg_file, &i_err);
    setts_check_defaults (st_setts);

    /* Get Pexels api string */
    st_st = setting_find_child (st_setts,
                                get_setting_name (SETTING_PEXELS_API));
    if (st_st != NULL) {
        gtk_entry_set_text (GTK_ENTRY (gw_api_entry),
                            setting_get_string (st_st));
    }
    settings_free_all (st_setts);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("Pexels API key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_api_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (
    "To get your API key, you need to be registered on the Pexels website and "
    " request a key from: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_link_button_new ("https://www.pexels.com/api/new/"),
                        FALSE, FALSE, 4);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        const char *s_api = gtk_entry_get_text (GTK_ENTRY (gw_api_entry));
        
        if ((i_err = setts_update_pexels_api (s_cfg_file, s_api)) == ERR_OK) {
            s_result = strdup (s_api);
        }
        else {
            message_dialog_error (NULL, err_get_message (i_err));
        }
    }
    gtk_widget_destroy (gw_dialog);
    return s_result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Prepare image name from Pexels image url address.
 *
 * @param[in] s_url  Image url
 * @return    Image name
 */
static char *
pexels_get_name (const char *s_url)
{
    char *s_res  = NULL; /* Result string */
    char *s_repl = NULL; /* For replacing text */
    char *s_dup  = NULL; /* Url duplicate */

    s_dup  = strdup (s_url);
    s_repl = strrchr (s_dup, '/');
    if (s_repl == NULL)
        return NULL;
    *s_repl = '\0';
    s_repl = strrchr (s_dup, '/');
    if (s_repl == NULL)
        return NULL;
    s_res = strdup (s_repl+1);
    free (s_dup);
    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Pexels json image info to SearchItem item.
 *
 * @param[in] j_obj  Json object to convert
 * @return    SearchItem item
 */
static SearchItem *
pexels_json_obj_to_searchitem (json_object *j_obj)
{
    json_object *j_val;
    json_object *j_val2;
    char        *s_name = NULL;

    SearchItem *si_item = searchitem_new ();

    if (json_object_object_get_ex (j_obj, "id", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_id (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("photo id : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "width", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_width (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("width : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "height", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_height (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("height : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "url", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

#ifdef DEBUG
        printf ("url : %s\n", json_object_get_string (j_val));
#endif
        s_name = pexels_get_name (json_object_get_string (j_val));
        /* const char *s_name = json_object_get_string (j_val); */
        searchitem_set_name (si_item, s_name);
        searchitem_set_markup (si_item, s_name);
#ifdef DEBUG
        printf ("name : %s\n", s_name);
#endif
        free (s_name);
    }
    if (json_object_object_get_ex (j_obj, "src", &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        if (json_object_object_get_ex (j_val, "original", &j_val2) &&
            json_object_get_type (j_val2) == json_type_string) {

            searchitem_set_url (si_item, json_object_get_string (j_val2));
#ifdef DEBUG
            printf ("original : %s\n", json_object_get_string (j_val2));
#endif
        }
        if (json_object_object_get_ex (j_val, "tiny", &j_val2) &&
            json_object_get_type (j_val2) == json_type_string) {

            searchitem_set_thumb_url (si_item, json_object_get_string (j_val2));
#ifdef DEBUG
            printf ("thumb : %s\n", json_object_get_string (j_val2));
#endif
        }
    }
    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Analyze Pexels json search response and add results to image list.
 *
 * @param[in]  s_buff     String with json data
 * @param[out] ww_widget  Webwidget to set data
 * @return     none
 */
static void
pexels_json_to_webwidget (const char *s_buff,
                          WebWidget  *ww_widget)
{
    json_object *j_obj;
    json_object *j_val;
    json_object *j_arr;
    enum json_tokener_error j_err; /* Json error output */
    SearchItem *si_item = NULL;
    size_t      i       = 0;
    size_t      ui_cnt  = 0;

    j_obj = json_tokener_parse_verbose (s_buff, &j_err);
    if (j_obj == NULL ||
        json_object_get_type (j_obj) != json_type_object ||
        j_err != json_tokener_success) {
#ifdef DEBUG
        printf ("Json error: %d\n", j_err);
        printf ("Json type:  %d\n", json_object_get_type (j_obj));
        printf ("Error converting json to stlist, wrong json file\n");
#endif
        if (j_obj != NULL)
            json_object_put (j_obj);
    }
    else {
        if (json_object_object_get_ex (j_obj, "total_results", &j_val) &&
            json_object_get_type (j_val) == json_type_int) {

            ww_widget->found_cnt = json_object_get_int (j_val);
#ifdef DEBUG
            printf ("found images : %d\n", ww_widget->found_cnt);
#endif
        }
        if (json_object_object_get_ex (j_obj, "photos", &j_arr) &&
            json_object_get_type (j_arr) == json_type_array) {

            ui_cnt = json_object_array_length (j_arr);

            if (ui_cnt > (size_t) ww_widget->per_page)
                ui_cnt = (size_t) ww_widget->per_page;

            for (i = 0; i < ui_cnt; ++i) {
                if ((j_val = json_object_array_get_idx (j_arr, i)) != NULL) {
                    si_item = pexels_json_obj_to_searchitem (j_val);
                    add_searchitem_to_img_view (ww_widget->img_view, si_item);
                    searchitem_free (si_item);
                }
            }
        }
        json_object_put (j_obj);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Pexels database.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        none
 */
static void
pexels_search (WebWidget *ww_widget)
{
    UrlData *ud_data;
    char    *s_api = NULL;

    s_api = combo_get_active_str (ww_widget->combo, WW_COMBO_STR_1);

    if (s_api == NULL || s_api[0] == '\0') {
        message_dialog_warning (NULL, "Pexels API key is not set");
        return;
    }
    ud_data = urldata_search_pexels (ww_widget->query,
                                     s_api,
                                     ww_widget->page,
                                     ww_widget->per_page);
    if (ud_data->errbuf != NULL) {
        message_dialog_error (NULL, ud_data->errbuf);
    }
    else if (urldata_full (ud_data)) {
        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (ww_widget->img_view))));
        pexels_json_to_webwidget (ud_data->buffer, ww_widget);
    }
    urldata_free (ud_data);
    free (s_api);
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
                                        ww_widget->query, ww_widget->found_cnt);
    gtk_label_set_markup (GTK_LABEL (ww_widget->count_label), s_markup);
    g_free (s_markup);

    snprintf (s_entry, 128, "%d", ww_widget->page);
    gtk_entry_set_text (GTK_ENTRY (ww_widget->nav_entry), s_entry);
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
    GList      *gl_res = NULL;
    gboolean    b_res  = FALSE;
    GtkTreeIter gti_iter;
    SearchItem *si_item;

    GtkTreeModel *gtm_model;

    gtm_model = gtk_combo_box_get_model (
            GTK_COMBO_BOX (gw_selected_combo));

    b_res = gtk_tree_model_get_iter_first (gtm_model, &gti_iter);

    while (b_res) {

        si_item = searchitem_new ();
        gtk_tree_model_get (gtm_model, &gti_iter,
                            WW_SELCOMBO_NAME, &si_item->name,
                            WW_SELCOMBO_URL, &si_item->url,
                            WW_SELCOMBO_WIDTH, &si_item->width,
                            WW_SELCOMBO_HEIGHT, &si_item->height,
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
    int i_id = combo_get_active_int (ww_widget->combo, WW_COMBO_ID);
    switch (i_id) {
        case WEB_WIDGET_PEXELS:
            pexels_search (ww_widget);
            break;
        default:
            break;
    }
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
    const char *s_query = NULL;

    s_query = gtk_entry_get_text (GTK_ENTRY (ww_widget->entry));
    if (s_query[0] == '\0') {
        message_dialog_warning (NULL, "Empty search query");
        return;
    }
    if (ww_widget->query != NULL)
        free (ww_widget->query);
    ww_widget->query = strdup (s_query);
    ww_widget->page = 1;
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
    if (ww_widget->query != NULL && ww_widget->page < INT_MAX) {
        ++ww_widget->page;
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
    if (ww_widget->query != NULL && ww_widget->page > 1) {
        --ww_widget->page;
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
    if (ww_widget->query != NULL) {
        s_val = gtk_entry_get_text (GTK_ENTRY (gw_entry));
        i_val = strtoll (s_val, NULL, 10);
        if      (i_val < 1)       i_val = 1;
        else if (i_val > INT_MAX) i_val = INT_MAX;
        ww_widget->page = (int) i_val;
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
    char *s_api = NULL; /* For pexels API key */
    int   i_err = 0;    /* Error output */
    int   i_id  = 0;    /* Service id */

    i_id  = combo_get_active_int (ww_widget->combo, WW_COMBO_ID);

    switch (i_id) {
        case WEB_WIDGET_PEXELS:
            s_api = pexels_settings_dialog (ww_widget->cfg_file);
            if (s_api != NULL) {
                combo_set_active_str (ww_widget->combo, WW_COMBO_STR_1, s_api);
                i_err = setts_update_pexels_api (ww_widget->cfg_file, s_api);
                if (i_err != ERR_OK) {
                    message_dialog_error (NULL, err_get_message (i_err));
                }
                free (s_api);
            }
            break;
        default:
            break;
    }
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
    char         *s_name      = NULL;
    char         *s_url       = NULL;
    int           i_id        = 0;
    int           i_w         = 0;
    int           i_h         = 0;
    float         f_w         = 0;
    float         f_h         = 0;

    gtm_combo_model = gtk_combo_box_get_model (
            GTK_COMBO_BOX (ww_widget->selected_combo));

    gls_slstore = GTK_LIST_STORE (gtk_combo_box_get_model (
                GTK_COMBO_BOX (ww_widget->selected_combo)));

    gtm_model = gtk_icon_view_get_model (GTK_ICON_VIEW (ww_widget->img_view));

    gl_selected = gtk_icon_view_get_selected_items (
            GTK_ICON_VIEW (ww_widget->img_view));

    gl_item = gl_selected;

    while (gl_item != NULL) {

        gtp_path = gl_item->data;

        if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gtp_path)) {

            f_w = SEL_TH_WIDTH;
            f_h = SEL_TH_HEIGHT;

            gtk_tree_model_get (gtm_model, &gti_iter,
                                WEB_COL_NAME,   &s_name, 
                                WEB_COL_PIXBUF, &gp_pbuf,
                                WEB_COL_ID,     &i_id,
                                WEB_COL_URL,    &s_url,
                                WEB_COL_WIDTH,  &i_w,
                                WEB_COL_HEIGHT, &i_h,
                                -1);

            if (sel_combo_check_exist (gtm_combo_model, i_id) == 0) {

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
                                    WW_SELCOMBO_ID,     i_id,
                                    WW_SELCOMBO_PIXBUF, gp_pbuf2,
                                    WW_SELCOMBO_NAME,   s_name,
                                    WW_SELCOMBO_URL,    s_url,
                                    WW_SELCOMBO_WIDTH,  i_w,
                                    WW_SELCOMBO_HEIGHT, i_h,
                                    -1);
                g_object_unref(gp_pbuf2);
            }
            free (s_name);
            free (s_url);
            g_object_unref(gp_pbuf);
        }
        gl_item = gl_item->next;
    }
    g_list_free_full (gl_selected, (GDestroyNotify) gtk_tree_path_free);

    if (gtk_combo_box_get_active (GTK_COMBO_BOX
                (ww_widget->selected_combo)) == -1) {

        gtk_combo_box_set_active (GTK_COMBO_BOX (ww_widget->selected_combo), 0);
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
    GtkWidget       *gw_combo;            /* Result ComboBox */
    GtkCellRenderer *cell;                /* CellRenderer */
    GtkListStore    *list_store;          /* ListStore for combobox data */
    GtkTreeIter      iter;                /* TreeIter */
    const char      *s_pexels_api = NULL; /* Pexels API key */
    Setting         *st_st;               /* For individual setting */

    /* Get Pexels api string */
    st_st = setting_find_child (st_settings,
                                get_setting_name (SETTING_PEXELS_API));

    s_pexels_api = st_st == NULL ? "" : setting_get_string (st_st);

    /* Liststore for service data */
    list_store = gtk_list_store_new (WW_COMBO_CNT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        WW_COMBO_ID, WEB_WIDGET_PEXELS,
                        WW_COMBO_NAME, "Pexels",
                        WW_COMBO_STR_1, s_pexels_api,
                        -1);

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
                                     G_TYPE_INT,
                                     GDK_TYPE_PIXBUF,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_INT,
                                     G_TYPE_INT);

    gw_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_pixbuf_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, FALSE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "pixbuf", WW_SELCOMBO_PIXBUF);

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "text", WW_SELCOMBO_NAME);

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
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     GDK_TYPE_PIXBUF,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_INT,
                                     G_TYPE_INT);

    gw_icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (list_store));

    gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (gw_icon_view),
                                      GTK_SELECTION_MULTIPLE);

    gtk_icon_view_set_text_column (GTK_ICON_VIEW (gw_icon_view), WEB_COL_NAME);
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
 * @brief  Free WebWidget item.
 */
void
webwidget_free (WebWidget *ww_widget)
{
    free (ww_widget->cfg_file);
    free (ww_widget->query);
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

    ww_widget->cfg_file = strdup (s_cfg_file);

    gw_web_combo = webwidget_combobox_create (st_settings);
    gw_img_view  = webwidget_imgview_create ();

    gw_count_label = gtk_label_new ("Search query:   Found 0 results");

    /* Search box */
    gw_search_box    = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_sett_button   = gtk_button_new_with_label ("Settings");
    gw_search_entry  = gtk_search_entry_new ();
    gw_search_button = gtk_button_new_with_label ("Search");
    g_signal_connect_swapped (gw_sett_button, "clicked",
            G_CALLBACK (event_settings_pressed), ww_widget);
    g_signal_connect_swapped (gw_search_entry, "activate",
            G_CALLBACK (event_search_pressed), ww_widget);
    g_signal_connect_swapped (gw_search_button, "clicked",
            G_CALLBACK (event_search_pressed), ww_widget);

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

    ww_widget->search_box     = gw_search_box;
    ww_widget->combo          = gw_web_combo;
    ww_widget->entry          = gw_search_entry;
    ww_widget->nav_entry      = gw_nav_entry;
    ww_widget->img_view       = gw_img_view;
    ww_widget->nav_box        = gw_nav_box;
    ww_widget->selected_combo = gw_selected_combo;
    ww_widget->selected_box   = gw_add_sltd_box;
    ww_widget->count_label    = gw_count_label;
    ww_widget->query          = NULL;
    ww_widget->page           = 0;
    ww_widget->per_page       = IMGS_ON_PAGE;
    ww_widget->found_cnt      = 0;
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

    gl_items = sel_combo_get_list (ww_widget->selected_combo);
    gl_res = download_progress_window (GTK_WINDOW (gw_dialog), gl_items);
    g_list_free_full (gl_items, (GDestroyNotify) searchitem_free);

    return gl_res;
}
/*----------------------------------------------------------------------------*/

