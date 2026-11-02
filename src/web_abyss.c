/**
 * @file  web_abyss.c
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
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
 * @brief  Settings for searching the Wallpaper Abyss website.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <ctype.h>
#include <err.h>
#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include "string_functions.h"
#include "cache_query.h"
#include "message_dialogs.h"
#include "errors.h"
#include "search_item.h"
#include "settings.h"
#include "setting_item.h"
#include "url_data.h"
#include "webwidget_common.h"
#include "web_abyss.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enum with available search options.
 */
enum e_options {
    GW_WIDTH,    /**< Spinbutton with width */
    GW_HEIGHT,   /**< Spinbutton with height */
    GW_CNT       /**< Number of options */
};
/*----------------------------------------------------------------------------*/
/**
 * @var   opts
 * @brief Array with option names
 */
static const char *opts[] = {"width", "height", nullptr};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Process SearchIem item with image data and create name for icon view
 *         and for image file to save.
 *
 * @param[in,out] item  SearchItem item to process.
 * @return        none
 */
static void
wallpaperabyss_process_item_set_names (SearchItem *item)
{
    char *file_name = nullptr; /* Name for file to save */
    char *disp_name = nullptr; /* Name to display on list */
    char *ext       = nullptr; /* Pointer to extension */
    const char *format = "%s\n<span font_size=\"small\">[%dx%d]</span>";

    if (item->id == nullptr)
        return;
    else if (item->image_url == nullptr)
        return;

    /* Create file name, add image extension to base name */
    ext = strrchr (item->image_url, '.');
    if (ext != nullptr) {
        file_name = str_comb (ww_name (WEB_SERV_WALLABYSS), "_");
        str_append (&file_name, item->id);
        str_append (&file_name, ext);
    }
    /* Display name to show on image list */
    disp_name = item->id;

    /* Save names in SearchItem */
    search_item_set_file_name    (item, file_name);
    search_item_set_display_name (item, disp_name);
    /* search_item_set_display_markup (item, disp_name); */
    item->display_markup = g_markup_printf_escaped (
            format, disp_name, item->width, item->height);
#ifdef DEBUG
    printf ("fn : %s\n", item->file_name);
#endif
    free (file_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Wallpaper Abyss json image info to SearchItem item.
 *
 * @param[in] obj  Json object to convert
 * @return    SearchItem item
 */
static SearchItem *
wallpaperabyss_json_obj_to_search_item (json_object *obj)
{
    json_object *val;

    SearchItem *item = search_item_new ();

    search_item_set_service_name (item, ww_name (WEB_SERV_WALLABYSS));

    if (json_object_object_get_ex (obj, "id", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_id_string (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("photo id : %s\n", item->id);
#endif
    }
    if (json_object_object_get_ex (obj, "width", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_width (item, json_object_get_int (val));
#ifdef DEBUG
        printf ("width : %d\n", item->width);
#endif
    }
    if (json_object_object_get_ex (obj, "height", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_height (item, json_object_get_int (val));
#ifdef DEBUG
        printf ("height : %d\n", item->height);
#endif
    }
    if (json_object_object_get_ex (obj, "user_name", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_author_name (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("author : %s\n", item->author_name);
#endif
    }
    if (json_object_object_get_ex (obj, "url_page", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_page_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("url : %s\n", json_object_get_string (val));
#endif
    }
    if (json_object_object_get_ex (obj, "url_image", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_image_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("image url : %s\n", item->image_url);
#endif
    }
    if (json_object_object_get_ex (obj, "url_thumb", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_thumb_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("thumb url : %s\n", item->thumb_url);
#endif
    }
    wallpaperabyss_process_item_set_names (item);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Analyze Wallpaper Abyss json search response and add results to
 *         image list.
 *
 * @param[in]  buffer  String with json data
 * @param[out] widget  Webwidget to set data
 * @param[out] cache   CacheQuery item to insert SearchItem items
 * @return     none
 */
static void
wallpaperabyss_json_to_webwidget (const char *buffer,
                                  WebWidget  *widget,
                                  CacheQuery *cache)
{
    json_object *obj;  /* Json with search data */
    json_object *val;  /* Some value */
    json_object *arr;  /* For array data */
    enum json_tokener_error json_err; /* Json error output */
    SearchItem *item = nullptr; /* For image info */
    size_t      cnt  = 0;  /* Elements in array */
    size_t      i    = 0;

    obj = json_tokener_parse_verbose (buffer, &json_err);
    if (obj == nullptr ||
        json_object_get_type (obj) != json_type_object ||
        json_err != json_tokener_success) {
#ifdef DEBUG
        printf ("Json error: %d\n", json_err);
        printf ("Json type:  %d\n", json_object_get_type (obj));
        printf ("Error converting json to stlist, wrong json file\n");
#endif
        if (obj != nullptr)
            json_object_put (obj);
    }
    else {
        if (json_object_object_get_ex (obj, "success", &val) &&
            json_object_get_type (val) == json_type_boolean) {
            if (!json_object_get_boolean (val)) {
                if (json_object_object_get_ex (obj, "error", &val) &&
                    json_object_get_type (val) == json_type_string) {
                    warnx ("%s\n", json_object_get_string (val));
                }
                json_object_put (obj);
                return;
            }
        }

        if (json_object_object_get_ex (obj, "total_match", &val) &&
            json_object_get_type (val) == json_type_string) {

            widget->found_cnt = json_object_get_int (val);
#ifdef DEBUG
            printf ("found images : %d\n", widget->found_cnt);
#endif
        }

        if (json_object_object_get_ex (obj, "wallpapers", &arr) &&
            json_object_get_type (arr) == json_type_array) {

            cnt = json_object_array_length (arr);

            for (i = 0; i < cnt; ++i) {
                if ((val = json_object_array_get_idx (arr, i)) != nullptr) {
                    item = wallpaperabyss_json_obj_to_search_item (val);
                    add_search_item_to_img_view (widget->img_view,
                                                 item,
                                                 widget->wallpaper_dir,
                                                 ww_name (WEB_SERV_WALLABYSS),
                                                 widget->thumb_quality);
                    cache_query_append_item (cache, item);
                }
            }
        }
        json_object_put (obj);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Wallpaper Abyss database.
 */
void
wallpaperabyss_search (WebWidget      *widget,
                       const NStrings *strings)
{
    UrlData    *data  = nullptr; /* For search results */
    CacheQuery *cache = nullptr; /* For cache saving */
    char       *search_query = nullptr; /* For search query */
    int         err    = 0;  /* Error output */

    if (str_is_empty_warn (strings->str[0],
                           "Wallpaper Abyss API key is not set"))
        return;

    search_query = str_replace_in (widget->query, " ", "+");

    data = urldata_search_wallabyss (search_query,
                                        widget->search_opts,
                                        strings->str[0],
                                        widget->page);
    if (data->errbuf != nullptr) {
        message_dialog_error (nullptr, data->errbuf);
    }
    else if (urldata_full (data)) {
        cache = cache_query_new (ww_name (WEB_SERV_WALLABYSS),
                                    widget->query,
                                    widget->search_opts,
                                    widget->page);

        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                    GTK_ICON_VIEW (widget->img_view))));

        wallpaperabyss_json_to_webwidget (data->buffer, widget, cache);
        cache->found_cnt = widget->found_cnt;

        err = cache_query_save (cache);

        if (err != ERR_OK) {
            message_dialog_error (nullptr, err_get_message (err));
        }
        cache_query_free (cache);
    }
    urldata_free (data);
    free (search_query);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Wallpaper Abyss service settings.
 */
int
wallpaperabyss_settings_dialog (NStrings *strings)
{
    GtkWidget *dialog;      /* Wallpaper Abyss settings dialog */
    GtkWidget *content_box; /* Dialog's box */
    GtkWidget *api_entry;   /* Entry for API key */
    int        res = 0;     /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Wallpaper Abyss configuration",
                                             nullptr,
                                             flags,
                                             "_OK",
                                             GTK_RESPONSE_ACCEPT,
                                             "_Cancel",
                                             GTK_RESPONSE_REJECT,
                                             nullptr);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    api_entry = gtk_entry_new ();

    gtk_entry_set_text (GTK_ENTRY (api_entry), strings->str[0]);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new ("Wallpaper Abyss API key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        api_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new (
    "To get your API key, you need to be registered on the Wallpaper Abyss "
    "website and request it on: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_link_button_new (
                            "https://wall.alphacoders.com/api_signup.php"),
                        FALSE, FALSE, 4);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        free (strings->str[0]);
        strings->str[0] = strdup (
                gtk_entry_get_text (GTK_ENTRY (api_entry)));
    }
    gtk_widget_destroy (dialog);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get search options from Setting item to widgets.
 *
 * @param[out] widgets   Array with settings widgets
 * @param[in]  settings  Setting item with list of options
 * @return     none
 */
static void
set_search_opts (GtkWidget **widgets,
                 Setting    *settings)
{
    Setting *set  = nullptr;
    Setting *item = nullptr;

    set = setting_get_child (settings_find (settings,
                                               ww_opts (WEB_SERV_WALLABYSS)));

    if (set == nullptr) {
        return;
    }
#ifdef DEBUG
    settings_print (set);
#endif
    if ((item = settings_find (set, opts[GW_WIDTH])) != nullptr) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (widgets[GW_WIDTH]),
                                   (double) setting_get_int (item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                opts[GW_WIDTH], setting_get_int (item));
#endif
    }
    if ((item = settings_find (set, opts[GW_HEIGHT])) != nullptr) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (widgets[GW_HEIGHT]),
                                   (double) setting_get_int (item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                opts[GW_HEIGHT], setting_get_int (item));
#endif
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get search options from widgets to Setting item.
 *
 * @param[in,out] widgets  Array with settings widgets
 * @return        Setting items with search options
 */
static Setting *
get_search_opts (GtkWidget **widgets)
{
    Setting *sett = nullptr;
    int      valw = 0;
    int      valh = 0;

    valw = gtk_spin_button_get_value_as_int (
             GTK_SPIN_BUTTON (widgets[GW_WIDTH]));
    valh = gtk_spin_button_get_value_as_int (
             GTK_SPIN_BUTTON (widgets[GW_HEIGHT]));

    if (valw > 0 && valh > 0) {
        sett = settings_append (sett,
                setting_new_int (opts[GW_WIDTH], valw));
        sett = settings_append (sett,
                setting_new_int (opts[GW_HEIGHT], valh));
    }
    return sett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 */
char *
wallpaperabyss_search_opts_dialog (WebWidget *widget)
{
    GtkAdjustment *adjust1;         /* Adjustment for width spinbutton */
    GtkAdjustment *adjust2;         /* Adjustment for height spinbutton */
    GtkWidget     *widgets[GW_CNT]; /* Array with widgets */
    GtkWidget     *dialog;          /* Pixabay settings dialog */
    GtkWidget     *content_box;     /* Dialog's box */
    GtkWidget     *box;             /* Box for widgets */
    GtkWidget     *hbox;            /* Horizontal box for widgets */
    Setting       *settings = nullptr; /* Settings */
    char          *result   = nullptr; /* Result string */
    int            err      = 0;    /* Error output */
    int            res      = 0;    /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    adjust1 = gtk_adjustment_new (0.0, 0.0, 10000.0, 1.0, 100.0, 0.0);
    adjust2 = gtk_adjustment_new (0.0, 0.0, 10000.0, 1.0, 100.0, 0.0);

    dialog = gtk_dialog_new_with_buttons ("Wallpaper Abyss search options",
                                             nullptr,
                                             flags,
                                             "_OK",
                                             GTK_RESPONSE_ACCEPT,
                                             "_Cancel",
                                             GTK_RESPONSE_REJECT,
                                             nullptr);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);

    /* Min image width and height */
    widgets[GW_WIDTH]  = gtk_spin_button_new (adjust1, 1.0, 0);
    widgets[GW_HEIGHT] = gtk_spin_button_new (adjust2, 1.0, 0);
    gtk_widget_set_tooltip_text (widgets[GW_WIDTH],
          "Image width. If 0 value is set, all widths are allowed.");
    gtk_widget_set_tooltip_text (widgets[GW_HEIGHT],
          "Image height. If 0 value is set, all heights are allowed.");
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Image width:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_WIDTH],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Image height:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_HEIGHT],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box), hbox, FALSE, FALSE, 4);

    settings = setts_read (widget->config_file, &err);
    set_search_opts (widgets, settings);
    settings_free_all (settings);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        settings = setting_new_setting (ww_opts (WEB_SERV_WALLABYSS));

        setting_add_child (settings, get_search_opts (widgets));
#ifdef DEBUG
        settings_print (settings);
#endif
        setts_check_update_file (widget->config_file, settings);
        result = search_opts_to_str (setting_get_child (settings));
        settings_free_all (settings);
    }
    else {
        result = nullptr;
    }
    gtk_widget_destroy (dialog);

    return result;
}
/*----------------------------------------------------------------------------*/

