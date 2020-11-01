/**
 * @file  webwallhaven.c
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
 * @brief  Settings for searching the Wallhaven website.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <ctype.h>
#include <err.h>
#include "../config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include "strfun.h"
#include "chquery.h"
#include "dlgsmsg.h"
#include "errs.h"
#include "searchitem.h"
#include "setts.h"
#include "urldata.h"
#include "webwidget_c.h"
#include "webpexels.h"
#include "webwallhaven.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   SERV_NAME
 * @brief Service name
 */
#define SERV_NAME "Wallhaven" 
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enum with available search options.
 */
enum e_options {
    GW_CATEGORIES,  /**< Image category */
    GW_PURITY,      /**< Image purity */
    GW_SORTING,     /**< Sorting method */
    GW_ORDER,       /**< Order of images */
    GW_TOPRANGE,    /**< Top range val */
    GW_ATLEAST,     /**< Min resolution */
    GW_RESOLUTIONS, /**< List of resolutions */
    GW_RATIOS,      /**< Image ratios */
    GW_COLORS,      /**< Colour of image */
    GW_CNT          /**< Number of options */
};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_opts
 * @brief Array with option names
 */
static const char *s_opts[] = {
    "categories", "purity", "sorting", "order", "topRange", "atleast",
    "resolutions", "ratios", "colors", NULL
};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_sort
 * @brief Sorting options
 */
static const char *s_sort[] = {
    "date_added", "relevance", "random", "views", "favorites", "toplist", NULL
};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_order
 * @brief Sorting order
 */
static const char *s_order[] = {"desc", "asc", NULL};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Process SearchIem item with image data and create name for icon view
 *         and for image file to save.
 *
 * @param[in,out] si_item  SearchItem item to process.
 * @return        none
 */
static void
wallhaven_process_item_set_names (SearchItem *si_item)
{
    char *s_base_name = NULL; /* Base name for processing */
    char *s_file_name = NULL; /* Name for file to save */
    char *s_disp_name = NULL; /* Name to display on list */
    char *s_ext       = NULL; /* Pointer to extension */
    const char *s_format = "%s\n<span font_size=\"small\">[%dx%d]</span>";

    if (si_item->s_id == NULL)
        return;
    else if (si_item->s_image_url == NULL)
        return;

    /* Base name made of picture url */
    s_base_name = si_item->s_id;

    /* Create file name, add image extension to base name */
    s_ext = strrchr (si_item->s_image_url, '.');
    if (s_ext != NULL) {
        s_file_name = str_comb (s_base_name, s_ext);
    }
    /* Display name to show on image list */
    s_disp_name = si_item->s_id;

    /* Save names in SearchItem */
    searchitem_set_file_name    (si_item, s_file_name);
    searchitem_set_display_name (si_item, s_disp_name);
    /* searchitem_set_display_markup (si_item, s_disp_name); */
    si_item->s_display_markup = g_markup_printf_escaped (
            s_format, s_disp_name, si_item->i_width, si_item->i_height);

    free (s_file_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Pixbay json image info to SearchItem item.
 *
 * @param[in] j_obj  Json object to convert
 * @return    SearchItem item
 */
static SearchItem *
wallhaven_json_obj_to_searchitem (json_object *j_obj)
{
    json_object *j_val;
    json_object *j_val2;

    SearchItem *si_item = searchitem_new ();

    searchitem_set_service_name (si_item, SERV_NAME);

    if (json_object_object_get_ex (j_obj, "id", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_id_string (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("photo id : %s\n", si_item->s_id);
#endif
    }
    if (json_object_object_get_ex (j_obj, "dimension_x", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_width (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("width : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "dimension_y", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_height (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("height : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "url", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_page_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("url : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "path", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_image_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("image url : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "thumbs", &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        if (json_object_object_get_ex (j_val, "small", &j_val2) &&
            json_object_get_type (j_val2) == json_type_string) {

            searchitem_set_thumb_url (si_item, json_object_get_string (j_val2));
#ifdef DEBUG
            printf ("thumb url : %s\n", json_object_get_string (j_val2));
#endif
        }
    }
    wallhaven_process_item_set_names (si_item);

    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Analyze Pixbay json search response and add results to image list.
 *
 * @param[in]  s_buff     String with json data
 * @param[out] ww_widget  Webwidget to set data
 * @param[out] cq_query   CacheQuery item to insert SearchItem items
 * @return     none
 */
static void
wallhaven_json_to_webwidget (const char *s_buff,
                             WebWidget  *ww_widget,
                             CacheQuery *cq_query)
{
    json_object *j_obj;            /* Json with search data */
    json_object *j_val;            /* Some value */
    json_object *j_val2;            /* Some value */
    json_object *j_arr;            /* For array data */
    enum json_tokener_error j_err; /* Json error output */
    SearchItem *si_item = NULL;    /* For image info */
    size_t      i       = 0;       /* i */
    size_t      ui_cnt  = 0;       /* Elements in array */

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
        if (json_object_object_get_ex (j_obj, "meta", &j_val) &&
            json_object_get_type (j_val) == json_type_object) {

            if (json_object_object_get_ex (j_val, "total", &j_val2) &&
                json_object_get_type (j_val2) == json_type_int) {

                ww_widget->i_found_cnt = json_object_get_int (j_val2);
#ifdef DEBUG
                printf ("found images : %d\n", ww_widget->i_found_cnt);
#endif
            }
        }

        if (json_object_object_get_ex (j_obj, "data", &j_arr) &&
            json_object_get_type (j_arr) == json_type_array) {

            ui_cnt = json_object_array_length (j_arr);

            for (i = 0; i < ui_cnt; ++i) {
                if ((j_val = json_object_array_get_idx (j_arr, i)) != NULL) {
                    si_item = wallhaven_json_obj_to_searchitem (j_val);
                    add_searchitem_to_img_view (ww_widget->gw_img_view,
                                                si_item,
                                                ww_widget->s_wallp_dir,
                                                SERV_NAME,
                                                ww_widget->i_thumb_quality);
                    cachequery_append_item (cq_query, si_item);
                }
            }
        }
        json_object_put (j_obj);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Wallhaven database.
 */
void
wallhaven_search (WebWidget         *ww_widget,
                  const FourStrings *fs_data)
{
    UrlData    *ud_data  = NULL; /* For search results */
    CacheQuery *cq_query = NULL; /* For cache saving */
    char       *s_query  = NULL; /* For search query */
    int         i_err    = 0;    /* Error output */

    if (str_is_empty_msg (fs_data->s_str1, SERV_NAME " API key is not set"))
        return;

    /* Check if there is a cached info about this search query */
    if (check_for_cached_query (ww_widget, SERV_NAME))
        return;

    s_query = str_replace_in (ww_widget->s_query, " ", "+");

    ud_data = urldata_search_wallhaven (s_query,
                                        ww_widget->s_search_opts,
                                        fs_data->s_str1,
                                        ww_widget->i_page);
    if (ud_data->errbuf != NULL) {
        message_dialog_error (NULL, ud_data->errbuf);
    }
    else if (urldata_full (ud_data)) {
        cq_query = cachequery_new (SERV_NAME,
                                   ww_widget->s_query,
                                   ww_widget->s_search_opts,
                                   ww_widget->i_page);

        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                    GTK_ICON_VIEW (ww_widget->gw_img_view))));

        wallhaven_json_to_webwidget (ud_data->buffer, ww_widget, cq_query);
        cq_query->i_found_cnt = ww_widget->i_found_cnt;

        i_err = cachequery_save (cq_query);

        if (i_err != ERR_OK) {
            message_dialog_error (NULL, err_get_message (i_err));
        }
        cachequery_free (cq_query);
    }
    urldata_free (ud_data);
    free (s_query);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Wallhaven service settings.
 */
int
wallhaven_settings_dialog (FourStrings *fs_data)
{
    GtkWidget *gw_dialog;      /* Wallhaven settings dialog */
    GtkWidget *gw_content_box; /* Dialog's box */
    GtkWidget *gw_api_entry;   /* Entry for API key */
    int        i_res = 0;      /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons (SERV_NAME " configuration",
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

    gtk_entry_set_text (GTK_ENTRY (gw_api_entry), fs_data->s_str1);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (SERV_NAME " API key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_api_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (
    "To get your API key, you need to be registered on the Wallhaven website "
    " and paste API key from: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_link_button_new (
                            "https://wallhaven.cc/settings/account"),
                        FALSE, FALSE, 4);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        free (fs_data->s_str1);
        fs_data->s_str1 = strdup (
                gtk_entry_get_text (GTK_ENTRY (gw_api_entry)));
    }
    gtk_widget_destroy (gw_dialog);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get search options from Setting item to widgets.
 *
 * @param[out] gw_array  Array with settings widgets
 * @param[in]  st_setts  Setting item with list of options
 * @return     none
 */
static void
set_search_opts (GtkWidget **gw_array,
                 Setting    *st_setts)
{
    Setting *st_set  = NULL;
    Setting *st_item = NULL;

    st_set = setting_get_child (settings_find (st_setts, SERV_NAME "_opts"));

    if (st_set == NULL) {
        return;
    }
#ifdef DEBUG
    settings_print (st_set);
#endif

    st_item = settings_find (st_set, s_opts[GW_SORTING]);
    if (st_item != NULL) {
#ifdef DEBUG
        printf ("set : %s %s\n", s_opts[GW_SORTING],
                setting_get_string (st_item));
#endif
        gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_SORTING]),
                                     setting_get_string (st_item));
    }
    st_item = settings_find (st_set, s_opts[GW_ORDER]);
    if (st_item != NULL) {
#ifdef DEBUG
        printf ("set : %s %s\n", s_opts[GW_ORDER],
                setting_get_string (st_item));
#endif
        gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_ORDER]),
                                     setting_get_string (st_item));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get search options from widgets to Setting item.
 *
 * @param[in,out] gw_array  Array with settings widgets
 * @return        Setting items with search options
 */
static Setting *
get_search_opts (GtkWidget **gw_array)
{
    Setting    *st_sett = NULL;
    char       *s_val   = NULL;

    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_SORTING]));
    st_sett = setting_new_string (s_opts[GW_SORTING], s_val);
    free (s_val);

    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_ORDER]));
    if (strcmp (s_val, s_order[0]) != 0) {
        settings_append (st_sett,
                         setting_new_string (s_opts[GW_ORDER], s_val));
    }
    free (s_val);

    return st_sett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Converts image search options in Setting format to string for url.
 */
char *
wallhaven_search_opts_to_str (const Setting *st_setts)
{
    char          *s_res   = NULL;
    const Setting *st_item = NULL;
    char           s_buff[32];

    s_res   = strdup ("");
    st_item = st_setts;

    while (st_item != NULL) {
        str_append (&s_res, "&");
        str_append (&s_res, setting_get_name (st_item));
        str_append (&s_res, "=");
        if (setting_get_type (st_item) == SET_VAL_INT) {
            sprintf (s_buff, "%" PRId64, setting_get_int (st_item));
            str_append (&s_res, s_buff);
        }
        else if (setting_get_type (st_item) == SET_VAL_STRING) {
            str_append (&s_res, setting_get_string (st_item));
        }
        st_item = st_item->next;
    }
    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 */
char *
wallhaven_search_opts_dialog (WebWidget *ww_widget)
{
    GtkWidget     *gw_array[GW_CNT];   /* Array with widgets */
    GtkWidget     *gw_dialog;          /* Pixbay settings dialog */
    GtkWidget     *gw_content_box;     /* Dialog's box */
    GtkWidget     *gw_box;             /* Box for widgets */
    GtkWidget     *gw_hbox;            /* Horizontal box for widgets */
    Setting       *st_settings = NULL; /* Settings */
    char          *s_res       = NULL; /* Result string */
    int            i_err       = 0;    /* Error output */
    int            i_res       = 0;    /* Dialog result */
    int            i           = 0;    /* i */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons (SERV_NAME " search options",
                                             NULL,
                                             flags,
                                             "_OK",
                                             GTK_RESPONSE_ACCEPT,
                                             "_Cancel",
                                             GTK_RESPONSE_REJECT,
                                             NULL);

    gw_content_box = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);
    gw_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);

    /* Sorting combobox */
    gw_array[GW_SORTING] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_SORTING],
        "Method of sorting results");
    for (i = 0; s_sort[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_SORTING]),
                                   s_sort[i], s_sort[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_SORTING]),
                                                s_sort[0]);
    /* Box for sorting */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Sorting:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_SORTING],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    /* Order combobox */
    gw_array[GW_ORDER] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_ORDER],
        "Sorting order");
    for (i = 0; s_order[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_ORDER]),
                                   s_order[i], s_order[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_ORDER]),
                                                s_order[0]);
    /* Box for sorting */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Sort order:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_ORDER],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);


    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_hbox, FALSE, FALSE, 4);

    st_settings = setts_read (ww_widget->s_cfg_file, &i_err);
    set_search_opts (gw_array, setting_get_child (st_settings));
    settings_free_all (st_settings);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        st_settings = setting_new_setting (SERV_NAME "_opts");

        setting_add_child (st_settings, get_search_opts (gw_array));
#ifdef DEBUG
        settings_print (st_settings);
#endif
        setts_check_update_file (ww_widget->s_cfg_file, st_settings);
        s_res = wallhaven_search_opts_to_str (setting_get_child (st_settings));
        settings_free_all (st_settings);
    }
    else {
        s_res = NULL;
    }
    gtk_widget_destroy (gw_dialog);

    return s_res;
}
/*----------------------------------------------------------------------------*/


