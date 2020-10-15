/**
 * @file  webpixbay.c
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
 * @brief  Settings for searching the Pixbay website.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <ctype.h>
#include "../config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include "urldata.h"
#include "webwidget_common.h"
#include "dlgsmsg.h"
#include "strfun.h"
#include "webpixbay.h"
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
 * @brief  Convert Pixbay json image info to SearchItem item.
 *
 * @param[in] j_obj  Json object to convert
 * @return    SearchItem item
 */
static SearchItem *
pixbay_json_obj_to_searchitem (json_object *j_obj)
{
    json_object *j_val;

    SearchItem *si_item = searchitem_new ();

    if (json_object_object_get_ex (j_obj, "id", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_id_uint (si_item,
                                (uint64_t) json_object_get_int64 (j_val));
#ifdef DEBUG
        printf ("photo id : %d\n", json_object_get_int (j_val));
        printf ("photo id : %s\n", si_item->s_id);
#endif
    }
    if (json_object_object_get_ex (j_obj, "imageWidth", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_width (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("width : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "imageHeight", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_height (si_item, json_object_get_int (j_val));
#ifdef DEBUG
        printf ("height : %d\n", json_object_get_int (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "user", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_author_name (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("author : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "pageURL", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_page_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("url : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "imageURL", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_image_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("image url : %s\n", json_object_get_string (j_val));
#endif
    }
    else if (json_object_object_get_ex (j_obj, "largeImageURL", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_image_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("image url : %s\n", json_object_get_string (j_val));
#endif
    }
    if (json_object_object_get_ex (j_obj, "previewURL", &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_thumb_url (si_item, json_object_get_string (j_val));
#ifdef DEBUG
        printf ("thumb url : %s\n", json_object_get_string (j_val));
#endif
    }
    pexels_process_item_set_names (si_item);

    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Analyze Pixbay json search response and add results to image list.
 *
 * @param[in]  s_buff     String with json data
 * @param[out] ww_widget  Webwidget to set data
 * @return     none
 */
static void
pixbay_json_to_webwidget (const char *s_buff,
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
        if (json_object_object_get_ex (j_obj, "totalHits", &j_val) &&
            json_object_get_type (j_val) == json_type_int) {

            ww_widget->i_found_cnt = json_object_get_int (j_val);
#ifdef DEBUG
            printf ("found images : %d\n", ww_widget->i_found_cnt);
#endif
        }
        if (json_object_object_get_ex (j_obj, "hits", &j_arr) &&
            json_object_get_type (j_arr) == json_type_array) {

            ui_cnt = json_object_array_length (j_arr);

            if (ui_cnt > (size_t) ww_widget->i_per_page)
                ui_cnt = (size_t) ww_widget->i_per_page;

            for (i = 0; i < ui_cnt; ++i) {
                if ((j_val = json_object_array_get_idx (j_arr, i)) != NULL) {
                    si_item = pixbay_json_obj_to_searchitem (j_val);
                    add_searchitem_to_img_view (ww_widget->gw_img_view,
                                                si_item,
                                                ww_widget->s_thumb_dir,
                                                "pixbay_");
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
 */
void
pixbay_search (WebWidget         *ww_widget,
               const FourStrings *fs_data)
{
    UrlData *ud_data = NULL; /* For search results */
    char    *s_query = NULL; /* For search query */

    if (str_is_empty_msg (fs_data->s_str1, "Pixbay API key is not set"))
        return;

    s_query = str_replace_in (ww_widget->s_query, " ", "+");
    ud_data = urldata_search_pixbay (s_query,
                                     fs_data->s_str1,
                                     ww_widget->i_page,
                                     ww_widget->i_per_page);
    if (ud_data->errbuf != NULL) {
        message_dialog_error (NULL, ud_data->errbuf);
    }
    else if (urldata_full (ud_data)) {
        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (ww_widget->gw_img_view))));
        pixbay_json_to_webwidget (ud_data->buffer, ww_widget);
    }
    urldata_free (ud_data);
    free (s_query);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Pixbay service settings.
 */
int
pixbay_settings_dialog (FourStrings *fs_data)
{
    GtkWidget *gw_dialog;      /* Pixbay settings dialog */
    GtkWidget *gw_content_box; /* Dialog's box */
    GtkWidget *gw_api_entry;   /* Entry for API key */
    int        i_res = 0;      /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons ("Pixbay configuration",
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
                        gtk_label_new ("Pixbay API key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_api_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (
    "To get your API key, you need to be registered on the Pixbay website and "
    " paste API key from: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_link_button_new ("https://pixabay.com/api/docs/"),
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
