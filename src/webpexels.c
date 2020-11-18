/**
 * @file  webpexels.c
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
 * @brief  Settings for searching the Pexels website.
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
#include "urldata.h"
#include "setts.h"
#include "setting.h"
#include "webwidget_c.h"
#include "webpexels.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Extract base image name from image url.
 *
 * @param[in] s_url  Url to process
 * @return    New string with base name. After use it should be freed using
 *            free.
 */
static char *
pexels_extract_base_name (const char *s_url)
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
 * @brief  Create Image name for icon view from base name.
 *
 * @param[in] s_bname  Base name to process
 * @return    New string with name. After use it should be freed using free.
 */
static char *
pexels_create_display_name (const char *s_bname)
{
    char  *s_res  = NULL; /* Result string */
    char  *s_repl = NULL; /* For replacing text */

    if (str_is_empty (s_bname))
        return strdup ("");

    s_res  = str_replace_in (s_bname, "-", " ");
    gunichar gc = g_utf8_get_char (s_res);
    gc = g_unichar_toupper (gc);
    g_unichar_to_utf8 (gc, s_res);
    s_repl = s_res + strlen (s_res);
    while (isdigit (*--s_repl));
    *s_repl = '\0';

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Process SearchIem item with image data and create name for icon view
 *         and for image file to save.
 *
 * @param[in,out] si_item  SearchItem item to process.
 * @return        none
 */
static void
pexels_process_item_set_names (SearchItem *si_item)
{
    char *s_base_name = NULL; /* Base name for processing */
    char *s_file_name = NULL; /* Name for file to save */
    char *s_disp_name = NULL; /* Name to display on list */
    char *s_ext       = NULL; /* Pointer to extension */
    const char *s_format = "%s\n<span font_size=\"small\">[%dx%d]</span>";

    if (si_item->s_page_url == NULL)
        return;
    else if (si_item->s_image_url == NULL)
        return;

    /* Base name made of picture url */
    s_base_name = pexels_extract_base_name (si_item->s_page_url);

    /* Create file name, add image extension to base name */
    s_ext = strrchr (si_item->s_image_url, '.');
    if (s_ext != NULL) {
        s_file_name = str_comb (s_base_name, s_ext);
    }
    /* Display name to show on image list */
    s_disp_name = pexels_create_display_name (s_base_name);

    /* Save names in SearchItem */
    searchitem_set_file_name      (si_item, s_file_name);
    searchitem_set_display_name   (si_item, s_disp_name);
    /* searchitem_set_display_markup (si_item, s_disp_name); */
    si_item->s_display_markup = g_markup_printf_escaped (
            s_format, s_disp_name, si_item->i_width, si_item->i_height);

    free (s_base_name);
    free (s_file_name);
    free (s_disp_name);
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

    SearchItem *si_item = searchitem_new ();

    searchitem_set_service_name (si_item, ww_name (WEB_WIDGET_PEXELS));

    if (json_object_object_get_ex (j_obj, "id", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_id_uint (si_item,
                                (uint64_t) json_object_get_int64 (j_val));
#ifdef DEBUG
        printf ("photo id : %s\n", si_item->s_id);
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
    if (json_object_object_get_ex (j_obj, "photographer", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_author_name (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("author : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "url", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_page_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("url : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "src", &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        if (json_object_object_get_ex (j_val, "original", &j_val2) &&
            json_object_get_type (j_val2) == json_type_string) {

            searchitem_set_image_url (si_item, json_object_get_string (j_val2));
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
        pexels_process_item_set_names (si_item);
    }
#ifdef DEBUG
    searchitem_print (si_item);
#endif
    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Analyze Pexels json search response and add results to image list.
 *
 * @param[in]  s_buff     String with json data
 * @param[out] ww_widget  Webwidget to set data
 * @param[out] cq_query   CacheQuery item to insert SearchItem items
 * @return     none
 */
static void
pexels_json_to_webwidget (const char *s_buff,
                          WebWidget  *ww_widget,
                          CacheQuery *cq_query)
{
    json_object *j_obj;            /* Json with search data */
    json_object *j_val;            /* Some value */
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
        if (json_object_object_get_ex (j_obj, "total_results", &j_val) &&
            json_object_get_type (j_val) == json_type_int) {

            ww_widget->i_found_cnt = json_object_get_int (j_val);
#ifdef DEBUG
            printf ("found images : %d\n", ww_widget->i_found_cnt);
#endif
        }
        if (json_object_object_get_ex (j_obj, "photos", &j_arr) &&
            json_object_get_type (j_arr) == json_type_array) {

            ui_cnt = json_object_array_length (j_arr);

            for (i = 0; i < ui_cnt; ++i) {
                if ((j_val = json_object_array_get_idx (j_arr, i)) != NULL) {
                    si_item = pexels_json_obj_to_searchitem (j_val);
                    add_searchitem_to_img_view (ww_widget->gw_img_view,
                                                si_item,
                                                ww_widget->s_wallp_dir,
                                                ww_name (WEB_WIDGET_PEXELS),
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
 * @brief  Search in Pexels database.
 */
void
pexels_search (WebWidget      *ww_widget,
               const NStrings *ns_data)
{
    UrlData    *ud_data  = NULL; /* For search results */
    CacheQuery *cq_query = NULL; /* For cache saving */
    char       *s_query  = NULL; /* For search query */
    int         i_err    = 0;    /* Error output */

    if (str_is_empty_msg (ns_data->s_str[0], "Pexels API key is not set"))
        return;

    s_query = str_replace_in (ww_widget->s_query, " ", "+");

    ud_data = urldata_search_pexels (s_query,
                                     ww_widget->s_search_opts,
                                     ns_data->s_str[0],
                                     ww_widget->i_page);
    if (ud_data->errbuf != NULL) {
        message_dialog_error (NULL, ud_data->errbuf);
    }
    else if (urldata_full (ud_data)) {
        cq_query = cachequery_new (ww_name (WEB_WIDGET_PEXELS),
                                   ww_widget->s_query,
                                   ww_widget->s_search_opts,
                                   ww_widget->i_page);

        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                    GTK_ICON_VIEW (ww_widget->gw_img_view))));

        pexels_json_to_webwidget (ud_data->buffer, ww_widget, cq_query);
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
 * @brief  Dialog with Pexels service settings.
 */
int
pexels_settings_dialog (NStrings *ns_data)
{
    GtkWidget *gw_dialog;      /* Pexels settings dialog */
    GtkWidget *gw_content_box; /* Dialog's box */
    GtkWidget *gw_api_entry;   /* Entry for API key */
    int        i_res = 0;      /* Dialog result */

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

    gtk_entry_set_text (GTK_ENTRY (gw_api_entry), ns_data->s_str[0]);

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
    " request it on: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_link_button_new ("https://www.pexels.com/api/new/"),
                        FALSE, FALSE, 4);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        free (ns_data->s_str[0]);
        ns_data->s_str[0] = strdup (
                gtk_entry_get_text (GTK_ENTRY (gw_api_entry)));
    }
    gtk_widget_destroy (gw_dialog);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get search options from Setting item to widgets.
 *
 * @param[out] gw_per_page  Widget with images per page
 * @param[in]  st_setts  Setting item with list of options
 * @return     none
 */
static void
set_search_opts (GtkWidget *gw_per_page,
                 Setting   *st_setts)
{
    Setting *st_set  = NULL;
    Setting *st_item = NULL;

    st_set = setting_get_child (settings_find (st_setts,
                                               ww_opts (WEB_WIDGET_PEXELS)));
    if (st_set == NULL) {
        return;
    }
#ifdef DEBUG
    settings_print (st_set);
#endif
    if ((st_item = settings_find (st_set, "per_page")) != NULL) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (gw_per_page),
                                   (double) setting_get_int (st_item));
#ifdef DEBUG
        printf ("set : %s %ld\n", "per_page", setting_get_int (st_item));
#endif
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get search options from widgets to Setting item.
 *
 * @param[in] gw_per_page  Widget with images per page
 * @return    Setting items with search options
 */
static Setting *
get_search_opts (GtkWidget *gw_per_page)
{
    Setting    *st_sett = NULL;
    int         i_val   = 0;

    i_val = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gw_per_page));
    st_sett = setting_new_int ("per_page", i_val);

    return st_sett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 */
char *
pexels_search_opts_dialog (WebWidget *ww_widget)
{
    GtkAdjustment *ga_adjust1;         /* Adjustment for per page spinbutton */
    GtkWidget     *gw_per_page;        /* Spin button for per page */
    GtkWidget     *gw_dialog;          /* Pexels settings dialog */
    GtkWidget     *gw_content_box;     /* Dialog's box */
    GtkWidget     *gw_hbox;            /* Horizontal box for widgets */
    Setting       *st_settings = NULL; /* Settings */
    char          *s_res       = NULL; /* Result string */
    int            i_err       = 0;    /* Error output */
    int            i_res       = 0;    /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    ga_adjust1 = gtk_adjustment_new (12.0, 1.0, 80.0, 1.0, 2.0, 0.0);

    gw_dialog = gtk_dialog_new_with_buttons ("Pexels search options",
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

    /* Images per page */
    gw_per_page = gtk_spin_button_new (ga_adjust1, 1.0, 0);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gtk_label_new ("Images per page:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gw_per_page,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_hbox, FALSE, FALSE, 4);

    st_settings = setts_read (ww_widget->s_cfg_file, &i_err);
    set_search_opts (gw_per_page, st_settings);
    settings_free_all (st_settings);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        st_settings = setting_new_setting (ww_opts (WEB_WIDGET_PEXELS));

        setting_add_child (st_settings, get_search_opts (gw_per_page));
#ifdef DEBUG
        settings_print (st_settings);
#endif
        setts_check_update_file (ww_widget->s_cfg_file, st_settings);
        s_res = search_opts_to_str (setting_get_child (st_settings));
        settings_free_all (st_settings);
    }
    else {
        s_res = NULL;
    }
    gtk_widget_destroy (gw_dialog);

    return s_res;

}
/*----------------------------------------------------------------------------*/

