/**
 * @file  web_wallhaven.c
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
 * @brief  Settings for searching the Wallhaven website.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/string_functions.h"
#include "search_item.h"
#include "settings/settings.h"
#include "settings/setting_item.h"
#include "url_data.h"
#include "webwidget_common.h"
#include "web_util.h"
#include "web_wallhaven.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enum with available search options.
 */
enum e_options {
    GW_CATEGORIES,  /**< Image category */
    GW_SORTING,     /**< Sorting method */
    GW_ORDER,       /**< Order of images */
    GW_TOPRANGE,    /**< Top range val */
    GW_ATLEAST,     /**< Min resolution */
    GW_RESOLUTIONS, /**< List of resolutions */
    GW_RATIOS,      /**< Image ratios */
    GW_COLORS,      /**< Colour of image */
    GW_CATEG1,      /**< Image category 1 */
    GW_CATEG2,      /**< Image category 2 */
    GW_CATEG3,      /**< Image category 3 */
    GW_PURITY1,     /**< Purity 1 */
    GW_PURITY2,     /**< Purity 2 */
    GW_PURITY3,     /**< Purity 3 */
    GW_CNT          /**< Number of options */
};
/*----------------------------------------------------------------------------*/
/**
 * @var   opts
 * @brief Array with option names
 */
static const char *opts[] = {
    "categories",  "sorting", "order", "topRange", "atleast", "resolutions",
    "ratios", "colors", "categories", nullptr, nullptr, "purity", nullptr
};
/*----------------------------------------------------------------------------*/
/**
 * @var   sort
 * @brief Sorting options
 */
static const char *sort[] = {
    "date_added", "relevance", "random", "views", "favorites", "toplist", nullptr
};
/*----------------------------------------------------------------------------*/
/**
 * @var   order
 * @brief Sorting order
 */
static const char *order[] = {"desc", "asc", nullptr};
/*----------------------------------------------------------------------------*/
/**
 * @var   toprange
 * @brief Top range options
 */
static const char *toprange[] = {
    "1d", "3d", "1w", "1M" , "3M", "6M", "1y", nullptr
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Wallhaven json image info to SearchItem item.
 *
 * @param[in] obj  Json object to convert
 * @return    SearchItem item
 */
static SearchItem *
wallhaven_json_obj_to_search_item (json_object *obj)
{
    SearchItem *item = search_item_new ();

    search_item_set_id_string (item, web_json_str (obj, "id"));
    search_item_set_width     (item, web_json_int (obj, "dimension_x"));
    search_item_set_height    (item, web_json_int (obj, "dimension_y"));
    search_item_set_page_url  (item, web_json_str (obj, "url"));
    search_item_set_image_url (item, web_json_str (obj, "path"));
    search_item_set_thumb_url (item,
            web_json_str (web_json_get (obj, "thumbs", json_type_object),
                          "small"));
    /* Images are uploaded by users and have no license information */
    search_item_set_license_name (item, "Not specified");

    search_item_set_names (item, wallhaven_provider.name,
                           web_ext_from_url (item->image_url));
    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Wallhaven search response.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found images
 * @param[in]  add_item   Function to pass found images to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not a Wallhaven search response
 */
static bool
wallhaven_parse (const char  *json,
                 int         *found_cnt,
                 WebItemFunc  add_item,
                 void        *user_data)
{
    json_object *obj  = web_json_parse (json);
    json_object *arr  = web_json_get (obj, "data", json_type_array);
    SearchItem  *item = nullptr; /* For image info */
    size_t       cnt  = 0;       /* Elements in array */

    if (arr == nullptr) {
        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (web_json_get (obj, "meta", json_type_object),
                               "total");
    cnt = json_object_array_length (arr);

    for (size_t i = 0; i < cnt; ++i) {
        item = wallhaven_json_obj_to_search_item (web_json_idx (arr, i));

        if (item->id != nullptr && item->image_url != nullptr)
            add_item (item, user_data);
        else
            search_item_free (item);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Wallhaven search request url.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      Wallhaven API key, can be empty
 * @param[in] page         Results page number
 * @return    New string with url. After use it should be freed using free.
 */
static char *
wallhaven_search_url (const char *query,
                      const char *search_opts,
                      const char *api_key,
                      const int   page)
{
    char *url     = nullptr; /* Result url */
    char *escaped = nullptr; /* Percent-encoded query or API key */
    char  page_str[16];      /* Page number as string */

    snprintf (page_str, sizeof (page_str), "%d", page);

    escaped = urldata_escape (query);
    url = str_comb ("https://wallhaven.cc/api/v1/search?q=", escaped);
    free (escaped);

    /* The key is optional, it only gives access to the NSFW images */
    if (!str_is_empty (api_key)) {
        escaped = urldata_escape (api_key);
        str_append (&url, "&apikey=");
        str_append (&url, escaped);
        free (escaped);
    }
    str_append (&url, "&page=");
    str_append (&url, page_str);
    str_append (&url, str_is_empty (search_opts) ?
                        "&sorting=relevance" : search_opts);
    return url;
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
    Setting    *set  = nullptr;
    Setting    *item = nullptr;
    const char *val  = nullptr;
    int i = 0;

    set = setting_get_child (settings_find (settings,
                                               ww_opts (WEB_SERV_WALLHAVEN)));

    if (set == nullptr) {
        return;
    }
#ifdef DEBUG
    settings_print (set);
#endif

    if ((item = settings_find (set, opts[GW_SORTING])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n", opts[GW_SORTING],
                setting_get_string (item));
#endif
        gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_SORTING]),
                                     setting_get_string (item));
    }
    if ((item = settings_find (set, opts[GW_TOPRANGE])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n", opts[GW_TOPRANGE],
                setting_get_string (item));
#endif
        gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_TOPRANGE]),
                                     setting_get_string (item));
    }
    if ((item = settings_find (set, opts[GW_ORDER])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n", opts[GW_ORDER],
                setting_get_string (item));
#endif
        gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_ORDER]),
                                     setting_get_string (item));
    }
    if ((item = settings_find (set, opts[GW_ATLEAST])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                opts[GW_ATLEAST], setting_get_string (item));
#endif
        gtk_entry_set_text (GTK_ENTRY (widgets[GW_ATLEAST]),
                            setting_get_string (item));
    }
    if ((item = settings_find (set, opts[GW_RESOLUTIONS])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                opts[GW_RESOLUTIONS], setting_get_string (item));
#endif
        gtk_entry_set_text (GTK_ENTRY (widgets[GW_RESOLUTIONS]),
                            setting_get_string (item));
    }
    if ((item = settings_find (set, opts[GW_RATIOS])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                opts[GW_RATIOS], setting_get_string (item));
#endif
        gtk_entry_set_text (GTK_ENTRY (widgets[GW_RATIOS]),
                            setting_get_string (item));
    }
    if ((item = settings_find (set, opts[GW_CATEG1])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                opts[GW_CATEG1], setting_get_string (item));
#endif
        val = setting_get_string (item);
        if (strlen (val) != 3)
            return;
        for (i = GW_CATEG1; i <= GW_CATEG3; ++i) {
            gtk_check_menu_item_set_active (
                    GTK_CHECK_MENU_ITEM (widgets[i]),
                    val[i - GW_CATEG1] == '1' ? TRUE : FALSE);
        }
    }
    if ((item = settings_find (set, opts[GW_PURITY1])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                opts[GW_PURITY1], setting_get_string (item));
#endif
        val = setting_get_string (item);
        if (strlen (val) != 3)
            return;
        for (i = GW_PURITY1; i <= GW_PURITY3; ++i) {
            gtk_check_menu_item_set_active (
                    GTK_CHECK_MENU_ITEM (widgets[i]),
                    val[i - GW_PURITY1] == '1' ? TRUE : FALSE);
        }
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
    Setting    *sett    = nullptr;
    char       *val     = nullptr;
    char       *val2    = nullptr;
    const char *cval    = nullptr;
    gboolean    checked = FALSE;
    int         i       = 0;

    /* Getting sorting info */
    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_SORTING]));
    sett = settings_append (sett,
            setting_new_string (opts[GW_SORTING], val));
    /* If toplist is set */
    if (strcmp (val, sort[5]) == 0) {
        val2 = gtk_combo_box_text_get_active_text (
                 GTK_COMBO_BOX_TEXT (widgets[GW_TOPRANGE]));
        sett = settings_append (sett,
                         setting_new_string (opts[GW_TOPRANGE], val2));
        free (val2);
    }
    free (val);

    /* Getting order value */
    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_ORDER]));
    if (strcmp (val, order[0]) != 0) {
        sett = settings_append (sett,
                         setting_new_string (opts[GW_ORDER], val));
    }
    free (val);

    /* Getting at least value */
    cval = gtk_entry_get_text (GTK_ENTRY (widgets[GW_ATLEAST]));
    if (cval[0] != '\0') {
        sett = settings_append (sett,
                         setting_new_string (opts[GW_ATLEAST], cval));
    }
    /* Getting resolutions */
    cval = gtk_entry_get_text (GTK_ENTRY (widgets[GW_RESOLUTIONS]));
    if (cval[0] != '\0') {
        sett = settings_append (sett,
                         setting_new_string (opts[GW_RESOLUTIONS], cval));
    }
    /* Getting ratios */
    cval = gtk_entry_get_text (GTK_ENTRY (widgets[GW_RATIOS]));
    if (cval[0] != '\0') {
        sett = settings_append (sett,
                         setting_new_string (opts[GW_RATIOS], cval));
    }
    /* Getting category value */
    val = strdup ("");
    for (i = GW_CATEG1; i <= GW_CATEG3; ++i) {
        checked = gtk_check_menu_item_get_active (
                GTK_CHECK_MENU_ITEM (widgets[i]));
        str_append (&val, checked ? "1" : "0");
    }
    sett = settings_append (sett,
            setting_new_string (opts[GW_CATEG1], val));
    free (val);
    /* Getting purity value */
    val = strdup ("");
    for (i = GW_PURITY1; i <= GW_PURITY3; ++i) {
        checked = gtk_check_menu_item_get_active (
                GTK_CHECK_MENU_ITEM (widgets[i]));
        str_append (&val, checked ? "1" : "0");
    }
    sett = settings_append (sett,
            setting_new_string (opts[GW_PURITY1], val));
    free (val);

    return sett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 *
 * @param[in] widget  WebWidget with config file path
 * @return    String with search options or null if dialog was cancelled
 */
static char *
wallhaven_search_opts_dialog (WebWidget *widget)
{
    GtkWidget *widgets[GW_CNT]; /* Array with widgets */
    GtkWidget *dialog;          /* Search options dialog */
    GtkWidget *content_box;     /* Dialog's box */
    GtkWidget *box;             /* Box for widgets */
    GtkWidget *hbox;            /* Horizontal box for widgets */
    GtkWidget *menu;
    GtkWidget *menu_button;
    Setting   *settings = nullptr; /* Settings */
    char      *result   = nullptr; /* Result string */
    int        err      = 0;    /* Error output */
    int        res      = 0;    /* Dialog result */
    int        i        = 0;

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Wallhaven search options",
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

    /* Box for categories and purity */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    /* Categories */
    menu    = gtk_menu_new ();
    menu_button = gtk_menu_button_new ();
    gtk_button_set_label (GTK_BUTTON (menu_button), "Category");
    widgets[GW_CATEG1] = gtk_check_menu_item_new_with_label ("General");
    widgets[GW_CATEG2] = gtk_check_menu_item_new_with_label ("Anime");
    widgets[GW_CATEG3] = gtk_check_menu_item_new_with_label ("People");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), widgets[GW_CATEG1]);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), widgets[GW_CATEG2]);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), widgets[GW_CATEG3]);
    gtk_widget_show (widgets[GW_CATEG1]);
    gtk_widget_show (widgets[GW_CATEG2]);
    gtk_widget_show (widgets[GW_CATEG3]);
    gtk_check_menu_item_set_active (
            GTK_CHECK_MENU_ITEM (widgets[GW_CATEG1]), TRUE);
    gtk_check_menu_item_set_active (
            GTK_CHECK_MENU_ITEM (widgets[GW_CATEG2]), TRUE);
    gtk_check_menu_item_set_active (
            GTK_CHECK_MENU_ITEM (widgets[GW_CATEG3]), TRUE);
    gtk_menu_button_set_popup (GTK_MENU_BUTTON (menu_button), menu);
    gtk_box_pack_start (GTK_BOX (box), menu_button, FALSE, FALSE, 4);

    /* Purity */
    menu    = gtk_menu_new ();
    menu_button = gtk_menu_button_new ();
    gtk_button_set_label (GTK_BUTTON (menu_button), "Purity");
    /* Categories */
    widgets[GW_PURITY1] = gtk_check_menu_item_new_with_label ("sfw");
    widgets[GW_PURITY2] = gtk_check_menu_item_new_with_label ("sketchy");
    widgets[GW_PURITY3] = gtk_check_menu_item_new_with_label ("nsfw");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), widgets[GW_PURITY1]);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), widgets[GW_PURITY2]);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), widgets[GW_PURITY3]);
    gtk_widget_show (widgets[GW_PURITY1]);
    gtk_widget_show (widgets[GW_PURITY2]);
    gtk_widget_show (widgets[GW_PURITY3]);
    gtk_check_menu_item_set_active (
            GTK_CHECK_MENU_ITEM (widgets[GW_PURITY1]), TRUE);
    gtk_check_menu_item_set_active (
            GTK_CHECK_MENU_ITEM (widgets[GW_PURITY2]), FALSE);
    gtk_check_menu_item_set_active (
            GTK_CHECK_MENU_ITEM (widgets[GW_PURITY3]), FALSE);
    gtk_menu_button_set_popup (GTK_MENU_BUTTON (menu_button), menu);
    gtk_box_pack_start (GTK_BOX (box), menu_button, FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Sorting combobox */
    widgets[GW_SORTING] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_SORTING],
        "Method of sorting results.");
    for (i = 0; sort[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_SORTING]),
                                   sort[i], sort[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_SORTING]),
                                                sort[0]);
    /* Box for sorting */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Sorting:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_SORTING],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Order combobox */
    widgets[GW_ORDER] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_ORDER],
        "Sorting order.");
    for (i = 0; order[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_ORDER]),
                                   order[i], order[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_ORDER]),
                                                order[0]);
    /* Box for sorting */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);

    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Sort order:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_ORDER],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Top range combobox */
    widgets[GW_TOPRANGE] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_TOPRANGE],
          "Sorting must be set to 'toplist' for this setting to be activated");
    for (i = 0; toprange[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_TOPRANGE]),
                                   toprange[i], toprange[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_TOPRANGE]),
                                                toprange[3]);
    /* Box for Top range */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);

    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Top range:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_TOPRANGE],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Atleast entry */
    widgets[GW_ATLEAST] = gtk_entry_new ();
    gtk_widget_set_tooltip_text (widgets[GW_ATLEAST],
        "Minimum resolution allowed\ne.g. 1920x1080\n"
        "When this field is empty, all widths are allowed.");
    /* Box for atleast */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("At least:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_ATLEAST],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Resolution entry */
    widgets[GW_RESOLUTIONS] = gtk_entry_new ();
    gtk_widget_set_tooltip_text (widgets[GW_RESOLUTIONS],
        "List of exact wallpaper resolutions. Single resolution allowed.\n"
        "e.g. 1920x1080,1920x1200\n"
        "When this field is empty, all widths are allowed.");
    /* Box for resolution */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Resolutions:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_RESOLUTIONS],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Ratios entry */
    widgets[GW_RATIOS] = gtk_entry_new ();
    gtk_widget_set_tooltip_text (widgets[GW_RATIOS],
        "List of aspect ratios. Single ratio allowed.\n"
        "e.g. 16x9,16x10\n"
        "When this field is empty, all ratios are allowed.");
    /* Box for ratios */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Ratios:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_RATIOS],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box), hbox, FALSE, FALSE, 4);

    settings = setts_read (widget->config_file, &err);
    set_search_opts (widgets, settings);
    settings_free_all (settings);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        settings = setting_new_setting (ww_opts (WEB_SERV_WALLHAVEN));

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
/**
 * @var   wallhaven_provider
 * @brief Wallhaven search service.
 */
const WebProvider wallhaven_provider = {
    .name         = "Wallhaven",
    .label        = "Wallhaven",
    .opts_name    = "Wallhaven_opts",
    .api_key_name = "Wallhaven api",
    .api_key_info = "The API key is optional. It is only needed to search "
                    "for NSFW images. Registered users can copy their key "
                    "from the account settings page:",
    .api_key_url  = "https://wallhaven.cc/settings/account",
    .logo         = W_LOGO_WALLHAVEN,
    .search_url   = wallhaven_search_url,
    .parse        = wallhaven_parse,
    .opts_dialog  = wallhaven_search_opts_dialog
};
/*----------------------------------------------------------------------------*/
