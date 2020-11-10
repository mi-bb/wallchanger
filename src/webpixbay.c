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
#include <inttypes.h>
#include "../config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include "urldata.h"
#include "errs.h"
#include "chquery.h"
#include "webwidget_c.h"
#include "dlgsmsg.h"
#include "strfun.h"
#include "setting.h"
#include "setts.h"
#include "webpixbay.h"
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
 * @var   s_opts
 * @brief Array with option names
 */
static const char *s_opts[] = {
    "category", "orientation", "editors_choice", "safesearch", "order",
    "min_width", "min_height", "per_page", "colors"
};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_category
 * @brief Array with image categories
 */
static const char *s_category[] = {
    "none", "backgrounds", "fashion", "nature", "science", "education",
    "feelings", "health", "people", "religion", "places", "animals",
    "industry", "computer", "food", "sports", "transportation", "travel",
    "buildings", "business", "music", NULL};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_colour
 * @brief Array with image colours
 */
static const char *s_colour[] = {
    "any", "grayscale", "transparent", "red", "orange", "yellow", "green",
    "turquoise", "blue", "lilac", "pink", "white", "gray", "black",
    "brown", NULL};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_true_false
 * @brief Array with true and false value
 */
static const char *s_true_false[] = {"false", "true", NULL};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_order
 * @brief Array with image sort orders
 */
static const char *s_order[] = {"popular", "latest", NULL};
/*----------------------------------------------------------------------------*/
/**
 * @var   s_orientation
 * @brief Array with image orientations
 */
static const char *s_orientation[] = {"all", "horizontal", "vertical", NULL};
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

    searchitem_set_service_name (si_item, ww_name (WEB_WIDGET_PIXBAY));

    if (json_object_object_get_ex (j_obj, "id", &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_id_uint (si_item,
                                (uint64_t) json_object_get_int64 (j_val));
#ifdef DEBUG
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
 * @param[out] cq_query   CacheQuery item to insert SearchItem items
 * @return     none
 */
static void
pixbay_json_to_webwidget (const char  *s_buff,
                          WebWidget   *ww_widget,
                          CacheQuery  *cq_query)
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

            for (i = 0; i < ui_cnt; ++i) {
                if ((j_val = json_object_array_get_idx (j_arr, i)) != NULL) {
                    si_item = pixbay_json_obj_to_searchitem (j_val);
                    add_searchitem_to_img_view (ww_widget->gw_img_view,
                                                si_item,
                                                ww_widget->s_wallp_dir,
                                                ww_name (WEB_WIDGET_PIXBAY),
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
pixbay_search (WebWidget         *ww_widget,
               const FourStrings *fs_data)
{
    UrlData    *ud_data      = NULL; /* For search results */
    CacheQuery *cq_query     = NULL; /* For cache saving */
    char       *s_query      = NULL; /* For search query */
    int         i_err        = 0;    /* Error output */

    if (str_is_empty_msg (fs_data->s_str1, "Pixbay API key is not set"))
        return;

    s_query = str_replace_in (ww_widget->s_query, " ", "+");

#ifdef DEBUG
    printf ("query : %s\n", ww_widget->s_query);
    printf ("search opts : %s\n", ww_widget->s_search_opts);
#endif

    ud_data = urldata_search_pixbay (s_query,
                                     ww_widget->s_search_opts,
                                     fs_data->s_str1,
                                     ww_widget->i_page);
    if (ud_data->errbuf != NULL) {
        message_dialog_error (NULL, ud_data->errbuf);
    }
    else if (urldata_full (ud_data)) {
        cq_query = cachequery_new (ww_name (WEB_WIDGET_PIXBAY),
                                   ww_widget->s_query,
                                   ww_widget->s_search_opts,
                                   ww_widget->i_page);

        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (ww_widget->gw_img_view))));

        pixbay_json_to_webwidget (ud_data->buffer, ww_widget,
                                  cq_query);
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
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (
    "You should consider applying for a full access API key on this page, so "
    "you will get access to full size photos.\n"
    "Without full access key images will have max width/height of 1280px."),
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

    st_set = setting_get_child (settings_find (st_setts,
                                               ww_opts (WEB_WIDGET_PIXBAY)));

    if (st_set == NULL) {
        return;
    }
#ifdef DEBUG
    settings_print (st_set);
#endif
    for (int i = 0; i < GW_CNT; ++i) {
        if (i == GW_COLOUR || i == GW_COLOUR_ENTRY ||
            i == GW_MIN_WIDTH || i == GW_MIN_HEIGHT || i == GW_PER_PAGE)
            continue;
        if ((st_item = settings_find (st_set, s_opts[i])) != NULL) {
#ifdef DEBUG
            printf ("set : %s %s\n", s_opts[i], setting_get_string (st_item));
#endif
            gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[i]),
                                         setting_get_string (st_item));
        }
    }
    if ((st_item = settings_find (st_set, s_opts[GW_COLOUR])) != NULL) {
#ifdef DEBUG
        printf ("set : %s %s\n",
                s_opts[GW_COLOUR], setting_get_string (st_item));
#endif
        gtk_entry_set_text (GTK_ENTRY (gw_array[GW_COLOUR_ENTRY]),
                            setting_get_string (st_item));
    }

    if ((st_item = settings_find (st_set, s_opts[GW_MIN_WIDTH])) != NULL) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (gw_array[GW_MIN_WIDTH]),
                                   (double) setting_get_int (st_item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                s_opts[GW_MIN_WIDTH], setting_get_int (st_item));
#endif
    }
    if ((st_item = settings_find (st_set, s_opts[GW_MIN_HEIGHT])) != NULL) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (gw_array[GW_MIN_HEIGHT]),
                                   (double) setting_get_int (st_item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                s_opts[GW_MIN_HEIGHT], setting_get_int (st_item));
#endif
    }
    if ((st_item = settings_find (st_set, s_opts[GW_PER_PAGE])) != NULL) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (gw_array[GW_PER_PAGE]),
                                   (double) setting_get_int (st_item));
#ifdef DEBUG
        printf ("set : %s %ld\n",
                s_opts[GW_PER_PAGE], setting_get_int (st_item));
#endif
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
    const char *s_val2  = NULL;
    int         i_val   = 0;

    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_ORDER]));
    st_sett = setting_new_string (s_opts[GW_ORDER], s_val);
    free (s_val);

    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_ORIENTATION]));
    if (strcmp (s_val, s_orientation[0]) != 0) {
        settings_append (st_sett,
                         setting_new_string (s_opts[GW_ORIENTATION], s_val));
    }
    free (s_val);

    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_CATEGORY]));
    if (strcmp (s_val, s_category[0]) != 0) {
        settings_append (st_sett,
                         setting_new_string (s_opts[GW_CATEGORY], s_val));
    }
    free (s_val);

    s_val2 = gtk_entry_get_text (GTK_ENTRY (gw_array[GW_COLOUR_ENTRY]));
    if (s_val2[0] != '\0') {
        settings_append (st_sett,
                         setting_new_string (s_opts[GW_COLOUR], s_val2));
    }
    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_ED_CHOICE]));
    settings_append (st_sett, setting_new_string (s_opts[GW_ED_CHOICE], s_val));
    free (s_val);

    s_val = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_SAFE_SEARCH]));
    settings_append (st_sett,
                     setting_new_string (s_opts[GW_SAFE_SEARCH], s_val));
    free (s_val);

    i_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (gw_array[GW_MIN_WIDTH]));
    if (i_val > 0) {
        settings_append (st_sett,
                         setting_new_int (s_opts[GW_MIN_WIDTH], i_val));
    }

    i_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (gw_array[GW_MIN_HEIGHT]));
    if (i_val > 0) {
        settings_append (st_sett,
                         setting_new_int (s_opts[GW_MIN_HEIGHT], i_val));
    }
    i_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (gw_array[GW_PER_PAGE]));
    settings_append (st_sett, setting_new_int (s_opts[GW_PER_PAGE], i_val));

    return st_sett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Colour combobox changed, add colour to colour entry.
 *
 * @param[in,out] gw_array  Array with settings widgets
 * @return        none
 */
static void
event_combo_changed (GtkWidget **gw_array)
{
    char *s_entxt = NULL;
    char *s_txt   = NULL;

    s_txt = gtk_combo_box_text_get_active_text (
            GTK_COMBO_BOX_TEXT (gw_array[GW_COLOUR]));

    if (s_txt != NULL && strcmp (s_txt, s_colour[0]) != 0) {
        s_entxt = strdup (gtk_entry_get_text (GTK_ENTRY (
                          gw_array[GW_COLOUR_ENTRY])));
        if (s_entxt[0] != '\0')
            str_append (&s_entxt, ",");
        str_append (&s_entxt, s_txt);

        gtk_entry_set_text (GTK_ENTRY (gw_array[GW_COLOUR_ENTRY]), s_entxt);
        free (s_entxt);
    }
    free (s_txt);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 */
char *
pixbay_search_opts_dialog (WebWidget *ww_widget)
{
    GtkAdjustment *ga_adjust1;         /* Adjustment for max w spinbutton */
    GtkAdjustment *ga_adjust2;         /* Adjustment for max h spinbutton */
    GtkAdjustment *ga_adjust3;         /* Adjustment for per page spinbutton */
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

    ga_adjust1 = gtk_adjustment_new (0.0, 0.0, 10000.0, 1.0, 100.0, 0.0);
    ga_adjust2 = gtk_adjustment_new (0.0, 0.0, 10000.0, 1.0, 100.0, 0.0);
    ga_adjust3 = gtk_adjustment_new (12.0, 3.0, 200.0, 1.0, 2.0, 0.0);

    gw_dialog = gtk_dialog_new_with_buttons ("Pixbay search options",
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

    /* Orientation combobox */
    gw_array[GW_ORIENTATION] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_ORIENTATION],
        "Whether an image is wider than it is tall, or taller than it is wide");
    for (i = 0; s_orientation[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_ORIENTATION]),
                                   s_orientation[i], s_orientation[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_ORIENTATION]),
                                                s_orientation[0]);
    /* Box for orientation */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Orientation:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_ORIENTATION],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    /* Category combobox */
    gw_array[GW_CATEGORY] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_CATEGORY],
                                 "Filter results by category");
    for (i = 0; s_category[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_CATEGORY]),
                                   s_category[i], s_category[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_CATEGORY]),
                                 s_category[0]);
    /* Box for category */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Category:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_CATEGORY],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    /* Editor's choice combobox */
    gw_array[GW_ED_CHOICE] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_ED_CHOICE],
            "Select images that have received an Editor's Choice award");
    for (i = 0; s_true_false[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_ED_CHOICE]),
                                   s_true_false[i], s_true_false[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_ED_CHOICE]),
                                 s_true_false[0]);
    /* Box for editor's choice */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Editor's choice:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_ED_CHOICE],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    /* Safe search combobox */
    gw_array[GW_SAFE_SEARCH] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_SAFE_SEARCH],
        "A flag indicating that only images suitable for all ages should be "
        "returned");
    for (i = 0; s_true_false[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_SAFE_SEARCH]),
                                   s_true_false[i], s_true_false[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_SAFE_SEARCH]),
                                 s_true_false[0]);
    /* Box for safe search */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Safe search:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_SAFE_SEARCH],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    /* Order combobox */
    gw_array[GW_ORDER] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_ORDER],
                                 "How the results should be ordered");
    for (i = 0; s_order[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_ORDER]),
                                   s_order[i], s_order[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_ORDER]),
                                 s_order[0]);
    /* Box for order */
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Order:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_ORDER],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    /* Min image width and height */
    gw_array[GW_MIN_WIDTH]  = gtk_spin_button_new (ga_adjust1, 1.0, 0);
    gw_array[GW_MIN_HEIGHT] = gtk_spin_button_new (ga_adjust2, 1.0, 0);
    gtk_widget_set_tooltip_text (gw_array[GW_MIN_WIDTH],
          "Minimum image width. If 0 value is set, all widths are allowed.");
    gtk_widget_set_tooltip_text (gw_array[GW_MIN_HEIGHT],
          "Minimum image height. If 0 value is set, all heights are allowed.");
    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Min width:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_MIN_WIDTH],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gtk_label_new ("Min height:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        gw_array[GW_MIN_HEIGHT],
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox), gw_box, FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_hbox, FALSE, FALSE, 4);

    /* Box for colour */
    gw_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);

    /* Colour combobox */
    gw_array[GW_COLOUR] = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_COLOUR],
            "Filter images by color properties");
    for (i = 0; s_colour[i] != NULL; ++i) {
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_array[GW_COLOUR]),
                                   s_colour[i], s_colour[i]);
    }
    gtk_combo_box_set_active_id (GTK_COMBO_BOX (gw_array[GW_COLOUR]),
                                 s_colour[0]);
    g_signal_connect_swapped (gw_array[GW_COLOUR], "changed",
                              G_CALLBACK (event_combo_changed), gw_array);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gtk_label_new ("Colour:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gw_array[GW_COLOUR],
                        FALSE, FALSE, 4);
    /* Colour entry */
    gw_array[GW_COLOUR_ENTRY] = gtk_entry_new ();
    gtk_widget_set_tooltip_text (gw_array[GW_COLOUR_ENTRY],
            "Filter images by color properties. A comma separated list of "
            "values may be used to select multiple properties.");
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gw_array[GW_COLOUR_ENTRY],
                        TRUE, TRUE, 4);

    /* Per page */
    gw_array[GW_PER_PAGE] = gtk_spin_button_new (ga_adjust3, 1.0, 0);
    gtk_widget_set_tooltip_text (gw_array[GW_PER_PAGE],
            "Determine the number of results per page (3-200).");
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gtk_label_new ("Images per page:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gw_array[GW_PER_PAGE],
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_hbox, FALSE, FALSE, 4);

    st_settings = setts_read (ww_widget->s_cfg_file, &i_err);
    set_search_opts (gw_array, setting_get_child (st_settings));
    settings_free_all (st_settings);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        st_settings = setting_new_setting (ww_opts (WEB_WIDGET_PIXBAY));

        setting_add_child (st_settings, get_search_opts (gw_array));
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

