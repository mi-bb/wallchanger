/**
 * @file  web_pixabay.c
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
 * @brief  Settings for searching the Pixabay website.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <ctype.h>
#include <inttypes.h>
#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include "url_data.h"
#include "errors.h"
#include "cache_query.h"
#include "webwidget_common.h"
#include "message_dialogs.h"
#include "string_functions.h"
#include "setting_item.h"
#include "settings.h"
#include "web_pixabay.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enum with available search options.
 */
enum e_options {
    GW_CATEGORY,     /**< Image category */
    GW_ORIENTATION,  /**< Image orientation */
    GW_ED_CHOICE,    /**< Get editor's choice images */
    GW_SAFE_SEARCH,  /**< Safe search images */
    GW_ORDER,        /**< Order of images */
    GW_MIN_WIDTH,    /**< Spinbutton with min width */
    GW_MIN_HEIGHT,   /**< Spinbutton with min height */
    GW_PER_PAGE,     /**< Spinbutton with images per page */
    GW_COLOUR,       /**< Colour of image */
    GW_COLOUR_ENTRY, /**< Entry for colour values */
    GW_CNT           /**< Number of options */
};
/*----------------------------------------------------------------------------*/
/**
 * @var   opts
 * @brief Array with option names
 */
static const char *opts[] = {
    "category", "orientation", "editors_choice", "safesearch", "order",
    "min_width", "min_height", "per_page", "colors", nullptr
};
/*----------------------------------------------------------------------------*/
/**
 * @var   category
 * @brief Array with image categories
 */
static const char *category[] = {
    "none", "backgrounds", "fashion", "nature", "science", "education",
    "feelings", "health", "people", "religion", "places", "animals",
    "industry", "computer", "food", "sports", "transportation", "travel",
    "buildings", "business", "music", nullptr
};
/*----------------------------------------------------------------------------*/
/**
 * @var   colour
 * @brief Array with image colours
 */
static const char *colour[] = {
    "any", "grayscale", "transparent", "red", "orange", "yellow", "green",
    "turquoise", "blue", "lilac", "pink", "white", "gray", "black",
    "brown", nullptr
};
/*----------------------------------------------------------------------------*/
/**
 * @var   true_false
 * @brief Array with true and false value
 */
static const char *true_false[] = {"false", "true", nullptr};
/*----------------------------------------------------------------------------*/
/**
 * @var   order
 * @brief Array with image sort orders
 */
static const char *order[] = {"popular", "latest", nullptr};
/*----------------------------------------------------------------------------*/
/**
 * @var   orientation
 * @brief Array with image orientations
 */
static const char *orientation[] = {"all", "horizontal", "vertical", nullptr};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Extract base image name from image url.
 *
 * @param[in] url  Url to process
 * @return    New string with base name. After use it should be freed using
 *            free.
 */
static char *
pixabay_extract_base_name (const char *url)
{
    char *result  = nullptr; /* Result string */
    char *repl = nullptr; /* For replacing text */
    char *dup  = nullptr; /* Url duplicate */

    dup  = strdup (url);
    repl = strrchr (dup, '/');
    if (repl == nullptr) {
        free (dup);
        return nullptr;
    }
    *repl = '\0';
    repl = strrchr (dup, '/');
    if (repl == nullptr) {
        free (dup);
        return nullptr;
    }
    result = strdup (repl+1);
    free (dup);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create Image name for icon view from base name.
 *
 * @param[in] bname  Base name to process
 * @return    New string with name. After use it should be freed using free.
 */
static char *
pixabay_create_display_name (const char *bname)
{
    char  *result  = nullptr; /* Result string */
    char  *repl = nullptr; /* For replacing text */

    if (str_is_empty (bname))
        return strdup ("");

    result  = str_replace_in (bname, "-", " ");
    gunichar gc = g_utf8_get_char (result);
    gc = g_unichar_toupper (gc);
    g_unichar_to_utf8 (gc, result);
    repl = result + strlen (result);
    while (isdigit (*--repl));
    *repl = '\0';

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Process SearchIem item with image data and create name for icon view
 *         and for image file to save.
 *
 * @param[in,out] item  SearchItem item to process.
 * @return        none
 */
static void
pixabay_process_item_set_names (SearchItem *item)
{
    char *base_name = nullptr; /* Base name for processing */
    char *file_name = nullptr; /* Name for file to save */
    char *disp_name = nullptr; /* Name to display on list */
    char *ext       = nullptr; /* Pointer to extension */
    const char *format = "%s\n<span font_size=\"small\">[%dx%d]</span>";

    if (item->page_url == nullptr)
        return;
    else if (item->image_url == nullptr)
        return;

    /* Base name made of picture url */
    base_name = pixabay_extract_base_name (item->page_url);

    /* Create file name, add image extension to base name */
    ext = strrchr (item->image_url, '.');
    if (ext != nullptr) {
        file_name = str_comb (base_name, ext);
    }
    /* Display name to show on image list */
    disp_name = pixabay_create_display_name (base_name);

    /* Save names in SearchItem */
    search_item_set_file_name      (item, file_name);
    search_item_set_display_name   (item, disp_name);
    /* search_item_set_display_markup (item, disp_name); */
    item->display_markup = g_markup_printf_escaped (
            format, disp_name, item->width, item->height);

    free (base_name);
    free (file_name);
    free (disp_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Pixabay json image info to SearchItem item.
 *
 * @param[in] obj  Json object to convert
 * @return    SearchItem item
 */
static SearchItem *
pixabay_json_obj_to_search_item (json_object *obj)
{
    json_object *val;

    SearchItem *item = search_item_new ();

    search_item_set_service_name (item, ww_name (WEB_SERV_PIXABAY));

    if (json_object_object_get_ex (obj, "id", &val) &&
        json_object_get_type (val) == json_type_int) {

        search_item_set_id_uint (item,
                                 (uint64_t) json_object_get_int64 (val));
#ifdef DEBUG
        printf ("photo id : %s\n", item->id);
#endif
    }
    if (json_object_object_get_ex (obj, "imageWidth", &val) &&
        json_object_get_type (val) == json_type_int) {

        search_item_set_width (item, json_object_get_int (val));
#ifdef DEBUG
        printf ("width : %d\n", json_object_get_int (val));
#endif
    }
    if (json_object_object_get_ex (obj, "imageHeight", &val) &&
        json_object_get_type (val) == json_type_int) {

        search_item_set_height (item, json_object_get_int (val));
#ifdef DEBUG
        printf ("height : %d\n", json_object_get_int (val));
#endif
    }
    if (json_object_object_get_ex (obj, "user", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_author_name (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("author : %s\n", json_object_get_string (val));
#endif
    }
    if (json_object_object_get_ex (obj, "pageURL", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_page_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("url : %s\n", json_object_get_string (val));
#endif
    }
    if (json_object_object_get_ex (obj, "imageURL", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_image_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("image url : %s\n", json_object_get_string (val));
#endif
    }
    else if (json_object_object_get_ex (obj, "largeImageURL", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_image_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("image url : %s\n", json_object_get_string (val));
#endif
    }
    if (json_object_object_get_ex (obj, "previewURL", &val) &&
        json_object_get_type (val) == json_type_string) {

        search_item_set_thumb_url (item, json_object_get_string (val));
#ifdef DEBUG
        printf ("thumb url : %s\n", json_object_get_string (val));
#endif
    }
    pixabay_process_item_set_names (item);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Analyze Pixabay json search response and add results to image list.
 *
 * @param[in]  buffer  String with json data
 * @param[out] widget  Webwidget to set data
 * @param[out] cache   CacheQuery item to insert SearchItem items
 * @return     none
 */
static void
pixabay_json_to_webwidget (const char  *buffer,
                           WebWidget   *widget,
                           CacheQuery  *cache)
{
    json_object *obj;  /* Json with search data */
    json_object *val;  /* Some value */
    json_object *arr;  /* For array data */
    enum json_tokener_error json_err; /* Json error output */
    SearchItem *item = nullptr; /* For image info */
    size_t      cnt  = 0;       /* Elements in array */
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
        if (json_object_object_get_ex (obj, "totalHits", &val) &&
            json_object_get_type (val) == json_type_int) {

            widget->found_cnt = json_object_get_int (val);
#ifdef DEBUG
            printf ("found images : %d\n", widget->found_cnt);
#endif
        }
        if (json_object_object_get_ex (obj, "hits", &arr) &&
            json_object_get_type (arr) == json_type_array) {

            cnt = json_object_array_length (arr);

            for (i = 0; i < cnt; ++i) {
                if ((val = json_object_array_get_idx (arr, i)) != nullptr) {
                    item = pixabay_json_obj_to_search_item (val);
                    add_search_item_to_img_view (widget->img_view,
                                                 item,
                                                 widget->wallpaper_dir,
                                                 ww_name (WEB_SERV_PIXABAY),
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
 * @brief  Search in Pixabay database.
 */
void
pixabay_search (WebWidget      *widget,
                const NStrings *strings)
{
    UrlData    *data         = nullptr; /* For search results */
    CacheQuery *cache        = nullptr; /* For cache saving */
    char       *search_query = nullptr; /* For search query */
    int         err          = 0;       /* Error output */

    if (str_is_empty_warn (strings->str[0], "Pixabay API key is not set"))
        return;

    search_query = str_replace_in (widget->query, " ", "+");

#ifdef DEBUG
    printf ("query : %s\n", widget->query);
    printf ("search opts : %s\n", widget->search_opts);
#endif

    data = urldata_search_pixabay (search_query,
                                      widget->search_opts,
                                      strings->str[0],
                                      widget->page);
    if (data->errbuf != nullptr) {
        message_dialog_error (nullptr, data->errbuf);
    }
    else if (urldata_full (data)) {
        cache = cache_query_new (ww_name (WEB_SERV_PIXABAY),
                                    widget->query,
                                    widget->search_opts,
                                    widget->page);

        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (widget->img_view))));

        pixabay_json_to_webwidget (data->buffer, widget,
                                   cache);
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
 * @brief  Dialog with Pixabay service settings.
 */
int
pixabay_settings_dialog (NStrings *strings)
{
    GtkWidget *dialog;      /* Pixabay settings dialog */
    GtkWidget *content_box; /* Dialog's box */
    GtkWidget *api_entry;   /* Entry for API key */
    int        res = 0;     /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Pixabay configuration",
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
                        gtk_label_new ("Pixabay API key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        api_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new (
    "To get your API key, you need to be registered on the Pixabay website and "
    " paste API key from: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_link_button_new ("https://pixabay.com/api/docs/"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new (
    "You should consider applying for a full access API key on this page, so "
    "you will get access to full size photos.\n"
    "Without full access key images will have max width/height of 1280px."),
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
                                               ww_opts (WEB_SERV_PIXABAY)));

    if (set == nullptr) {
        return;
    }
#ifdef DEBUG
    settings_print (set);
#endif
    for (int i = 0; i < GW_CNT; ++i) {
        if (i == GW_COLOUR || i == GW_COLOUR_ENTRY ||
            i == GW_MIN_WIDTH || i == GW_MIN_HEIGHT || i == GW_PER_PAGE)
            continue;
        if ((item = settings_find (set, opts[i])) != nullptr) {
#ifdef DEBUG
            printf ("set : %s %s\n", opts[i], setting_get_string (item));
#endif
            gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[i]),
                                         setting_get_string (item));
        }
    }
    if ((item = settings_find (set, opts[GW_COLOUR])) != nullptr) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                opts[GW_COLOUR], setting_get_string (item));
#endif
        gtk_entry_set_text (GTK_ENTRY (widgets[GW_COLOUR_ENTRY]),
                            setting_get_string (item));
    }

    if ((item = settings_find (set, opts[GW_MIN_WIDTH])) != nullptr) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (widgets[GW_MIN_WIDTH]),
                                   (double) setting_get_int (item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                opts[GW_MIN_WIDTH], setting_get_int (item));
#endif
    }
    if ((item = settings_find (set, opts[GW_MIN_HEIGHT])) != nullptr) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (widgets[GW_MIN_HEIGHT]),
                                   (double) setting_get_int (item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                opts[GW_MIN_HEIGHT], setting_get_int (item));
#endif
    }
    if ((item = settings_find (set, opts[GW_PER_PAGE])) != nullptr) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (widgets[GW_PER_PAGE]),
                                   (double) setting_get_int (item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                opts[GW_PER_PAGE], setting_get_int (item));
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
    Setting    *sett    = nullptr;
    char       *val     = nullptr;
    const char *val2    = nullptr;
    int         int_val = 0;

    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_ORDER]));
    sett = setting_new_string (opts[GW_ORDER], val);
    free (val);

    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_ORIENTATION]));
    if (strcmp (val, orientation[0]) != 0) {
        sett = settings_append (sett,
                setting_new_string (opts[GW_ORIENTATION], val));
    }
    free (val);

    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_CATEGORY]));
    if (strcmp (val, category[0]) != 0) {
        sett = settings_append (sett,
                         setting_new_string (opts[GW_CATEGORY], val));
    }
    free (val);

    val2 = gtk_entry_get_text (GTK_ENTRY (widgets[GW_COLOUR_ENTRY]));
    if (val2[0] != '\0') {
        sett = settings_append (sett,
                         setting_new_string (opts[GW_COLOUR], val2));
    }
    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_ED_CHOICE]));
    sett = settings_append (sett,
            setting_new_string (opts[GW_ED_CHOICE], val));
    free (val);

    val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_SAFE_SEARCH]));
    sett = settings_append (sett,
                     setting_new_string (opts[GW_SAFE_SEARCH], val));
    free (val);

    int_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (widgets[GW_MIN_WIDTH]));
    if (int_val > 0) {
        sett = settings_append (sett,
                         setting_new_int (opts[GW_MIN_WIDTH], int_val));
    }

    int_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (widgets[GW_MIN_HEIGHT]));
    if (int_val > 0) {
        sett = settings_append (sett,
                         setting_new_int (opts[GW_MIN_HEIGHT], int_val));
    }
    int_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (widgets[GW_PER_PAGE]));
    sett = settings_append (sett,
            setting_new_int (opts[GW_PER_PAGE], int_val));

    return sett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Colour combobox changed, add colour to colour entry.
 *
 * @param[in,out] widgets  Array with settings widgets
 * @return        none
 */
static void
event_combo_changed (GtkWidget **widgets)
{
    char *entxt = nullptr;
    char *txt   = nullptr;

    txt = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (widgets[GW_COLOUR]));

    if (txt != nullptr && strcmp (txt, colour[0]) != 0) {
        entxt = strdup (gtk_entry_get_text (GTK_ENTRY (
                          widgets[GW_COLOUR_ENTRY])));
        if (entxt[0] != '\0')
            str_append (&entxt, ",");
        str_append (&entxt, txt);

        gtk_entry_set_text (GTK_ENTRY (widgets[GW_COLOUR_ENTRY]), entxt);
        free (entxt);
    }
    free (txt);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 */
char *
pixabay_search_opts_dialog (WebWidget *widget)
{
    GtkAdjustment *adjust1;         /* Adjustment for max w spinbutton */
    GtkAdjustment *adjust2;         /* Adjustment for max h spinbutton */
    GtkAdjustment *adjust3;         /* Adjustment for per page spinbutton */
    GtkWidget     *widgets[GW_CNT]; /* Array with widgets */
    GtkWidget     *dialog;          /* Pixabay settings dialog */
    GtkWidget     *content_box;     /* Dialog's box */
    GtkWidget     *box;             /* Box for widgets */
    GtkWidget     *hbox;            /* Horizontal box for widgets */
    Setting       *settings = nullptr; /* Settings */
    char          *result   = nullptr; /* Result string */
    int            err      = 0;    /* Error output */
    int            res      = 0;    /* Dialog result */
    int            i        = 0;    /* i */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    adjust1 = gtk_adjustment_new (0.0, 0.0, 10000.0, 1.0, 100.0, 0.0);
    adjust2 = gtk_adjustment_new (0.0, 0.0, 10000.0, 1.0, 100.0, 0.0);
    adjust3 = gtk_adjustment_new (12.0, 3.0, 200.0, 1.0, 2.0, 0.0);

    dialog = gtk_dialog_new_with_buttons ("Pixabay search options",
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

    /* Orientation combobox */
    widgets[GW_ORIENTATION] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_ORIENTATION],
        "Whether an image is wider than it is tall, or taller than it is wide");
    for (i = 0; orientation[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_ORIENTATION]),
                                   orientation[i], orientation[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_ORIENTATION]),
                                                orientation[0]);
    /* Box for orientation */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Orientation:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_ORIENTATION],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Category combobox */
    widgets[GW_CATEGORY] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_CATEGORY],
                                 "Filter results by category");
    for (i = 0; category[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_CATEGORY]),
                                   category[i], category[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_CATEGORY]),
                                 category[0]);
    /* Box for category */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Category:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_CATEGORY],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Editor's choice combobox */
    widgets[GW_ED_CHOICE] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_ED_CHOICE],
            "Select images that have received an Editor's Choice award");
    for (i = 0; true_false[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_ED_CHOICE]),
                                   true_false[i], true_false[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_ED_CHOICE]),
                                 true_false[0]);
    /* Box for editor's choice */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Editor's choice:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_ED_CHOICE],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Safe search combobox */
    widgets[GW_SAFE_SEARCH] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_SAFE_SEARCH],
        "A flag indicating that only images suitable for all ages should be "
        "returned");
    for (i = 0; true_false[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_SAFE_SEARCH]),
                                   true_false[i], true_false[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_SAFE_SEARCH]),
                                 true_false[0]);
    /* Box for safe search */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Safe search:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_SAFE_SEARCH],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Order combobox */
    widgets[GW_ORDER] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_ORDER],
                                 "How the results should be ordered");
    for (i = 0; order[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_ORDER]),
                                   order[i], order[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_ORDER]),
                                 order[0]);
    /* Box for order */
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Order:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_ORDER],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    /* Min image width and height */
    widgets[GW_MIN_WIDTH]  = gtk_spin_button_new (adjust1, 1.0, 0);
    widgets[GW_MIN_HEIGHT] = gtk_spin_button_new (adjust2, 1.0, 0);
    gtk_widget_set_tooltip_text (widgets[GW_MIN_WIDTH],
          "Minimum image width. If 0 value is set, all widths are allowed.");
    gtk_widget_set_tooltip_text (widgets[GW_MIN_HEIGHT],
          "Minimum image height. If 0 value is set, all heights are allowed.");
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Min width:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_MIN_WIDTH],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (box),
                        gtk_label_new ("Min height:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        widgets[GW_MIN_HEIGHT],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box), hbox, FALSE, FALSE, 4);

    /* Box for colour */
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);

    /* Colour combobox */
    widgets[GW_COLOUR] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (widgets[GW_COLOUR],
            "Filter images by color properties");
    for (i = 0; colour[i] != nullptr; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (widgets[GW_COLOUR]),
                                   colour[i], colour[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (widgets[GW_COLOUR]),
                                 colour[0]);
    g_signal_connect_swapped (widgets[GW_COLOUR], "changed",
                              G_CALLBACK (event_combo_changed), widgets);
    gtk_box_pack_start (GTK_BOX (hbox),
                        gtk_label_new ("Colour:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox),
                        widgets[GW_COLOUR],
                        FALSE, FALSE, 4);
    /* Colour entry */
    widgets[GW_COLOUR_ENTRY] = gtk_entry_new ();
    gtk_widget_set_tooltip_text (widgets[GW_COLOUR_ENTRY],
            "Filter images by color properties. A comma separated list of "
            "values may be used to select multiple properties.");
    gtk_box_pack_start (GTK_BOX (hbox),
                        widgets[GW_COLOUR_ENTRY],
                        TRUE, TRUE, 4);

    /* Per page */
    widgets[GW_PER_PAGE] = gtk_spin_button_new (adjust3, 1.0, 0);
    gtk_widget_set_tooltip_text (widgets[GW_PER_PAGE],
            "Determine the number of results per page (3-200).");
    gtk_box_pack_start (GTK_BOX (hbox),
                        gtk_label_new ("Images per page:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox),
                        widgets[GW_PER_PAGE],
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box), hbox, FALSE, FALSE, 4);

    settings = setts_read (widget->config_file, &err);
    set_search_opts (widgets, settings);
    settings_free_all (settings);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        settings = setting_new_setting (ww_opts (WEB_SERV_PIXABAY));

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

