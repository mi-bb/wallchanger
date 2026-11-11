/**
 * @file  webwidget.c
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
 * @brief  Web widget
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <err.h>
#include <ctype.h>
#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include "util/file_io.h"
#include "url_data.h"
#include "settings/json_functions.h"
#include "search_item.h"
#include "gui/message_dialogs.h"
#include "util/string_functions.h"
#include "settings/settings.h"
#include "util/errors.h"
#include "settings/cfg_file.h"
#include "util/file_dir_functions.h"
#include "defs.h"
#include "gui/icons.h"
#include "webwidget_common.h"
#include "cache_query.h"
#include "webwidget.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   NAME_LEN
 * @brief Length of name in download progress window.
 *
 * @var   SEL_NAME_LEN
 * @brief Length of name in selected images combobox.
 */
constexpr int NAME_LEN     = 40;
constexpr int SEL_NAME_LEN = 60;
/*----------------------------------------------------------------------------*/
/**
 * @var   INFO_TEXT_LEN
 * @brief Width in characters of the title and author in image info widget.
 */
constexpr int INFO_TEXT_LEN = 28;
/*----------------------------------------------------------------------------*/
/**
 * @def   IMAGE_INFO_JSON_FLAGS
 * @brief Format of the saved image info: one member per line and, where
 *        json-c can do it, addresses without escaped slashes.
 */
#ifdef JSON_C_TO_STRING_NOSLASHESCAPE
#define IMAGE_INFO_JSON_FLAGS \
    (JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE)
#else
#define IMAGE_INFO_JSON_FLAGS JSON_C_TO_STRING_PRETTY
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Services combobox columns
 */
enum e_services_columns {
    WW_COMBO_ID,      /**< Service id */
    WW_COMBO_NAME,    /**< Service name */
    WW_COMBO_API_KEY, /**< Service API key */
    WW_COMBO_LOGO,    /**< Service logo pixbuf */
    WW_COMBO_CNT      /**< Column count */
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
    WW_SELCOMBO_SERVICE,   /**< Name of the image's search service */
    WW_SELCOMBO_TITLE,     /**< Image title */
    WW_SELCOMBO_AUTHOR,    /**< Author of image */
    WW_SELCOMBO_LICENSE,   /**< Image license name */
    WW_SELCOMBO_LICENSE_URL, /**< Image license url */
    WW_SELCOMBO_PAGE_URL,  /**< Picture page url */
    WW_SELCOMBO_CNT        /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * Stop downloading images button pressed
 *
 * @param[out] stop  Stop val to set
 * @return     none
 */
static void
event_stop_button_pressed (int *stop)
{
    *stop = 1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add a string member to a json object if the string is not empty.
 *
 * @param[out] obj  Json object to add member to
 * @param[in]  key  Member name
 * @param[in]  val  Member value, can be null
 * @return     none
 */
static void
json_add_string (json_object *obj,
                 const char  *key,
                 const char  *val)
{
    if (!str_is_empty (val))
        json_object_object_add (obj, key, json_object_new_string (val));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save the source, author and license of a downloaded image in the
 *         image info directory, in a "<image file name>.json" file.
 *
 * @param[in] item  SearchItem with data of the downloaded image
 * @return    Saving status, ERR_OK or error code
 */
static int
save_image_info (const SearchItem *item)
{
    json_object *obj  = nullptr; /* Image info to save */
    char        *path = nullptr; /* Image info file path */
    int          res  = ERR_OK;

    path = cfg_file_get_image_info_path ();

    if ((res = dir_create_with_subdirs (path)) == ERR_OK) {

        obj = json_object_new_object ();
        json_add_string (obj, "service",     item->service_name);
        json_add_string (obj, "title",       item->title);
        json_add_string (obj, "author",      item->author_name);
        json_add_string (obj, "license",     item->license_name);
        json_add_string (obj, "license_url", item->license_url);
        json_add_string (obj, "page_url",    item->page_url);
        json_add_string (obj, "image_url",   item->image_url);

        str_append (&path, "/");
        str_append (&path, item->file_name);
        str_append (&path, ".json");

        res = save_file_data (path,
                              json_object_to_json_string_ext (
                                  obj, IMAGE_INFO_JSON_FLAGS));
        json_object_put (obj);
    }
    free (path);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Download images progress window.
 *
 * @param[in] parent     Parent window
 * @param[in] item_list  List with SearchItem image data items
 * @return    List with paths of downloaded images
 */
static GList *
download_progress_window (GtkWindow *parent,
                          GList     *item_list)
{
    GtkWidget  *window;      /* Progress window widget */
    GtkWidget  *label;       /* Label for file name */
    GtkWidget  *progress;    /* Progress bar */
    GtkWidget  *box_main;    /* Box for window widgets */
    GtkWidget  *stop_button; /* Stop downloading button */
    SearchItem *item;        /* For images from list */
    GList      *result = nullptr; /* Result list with downloaded files */
    const char *format = "<span style=\"italic\">%s</span>";
    char       *markup = nullptr; /* For label's markup */
    char       *wpdir  = nullptr; /* Path to wallpaper directory */
    char       *fn     = nullptr; /* File path for wallpaper save */
    char       *error  = nullptr; /* Error output */
    float       step   = 0;       /* Progress bar step */
    float       frac   = 0;       /* Fraction for progress bar to set */
    int         stop   = 0;       /* To set when stop button is pressed */
    int         info_err = ERR_OK; /* First error of saving image info */
    int         res    = ERR_OK;  /* Saving image info result */
    size_t      nlen   = 0;       /* Length of file name */
    char nbuff[NAME_LEN * 4 + 4];

    memset (nbuff, '\0', NAME_LEN * 4 + 4);
    memset (nbuff, ' ',  NAME_LEN + 10);

    /* Getting wallpaper save path and checking it's permissions, creating if
     * it doesn't exist */
    wpdir = cfg_file_get_app_wallpapers_path ();

    /* Setting progress step and making a pointer to list */
    step  = (float) 1.0 / (float) g_list_length (item_list);

    /* Creating progress window with widgets */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_transient_for (GTK_WINDOW (window), parent);
    gtk_window_set_modal (GTK_WINDOW (window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (window), FALSE);

    label = gtk_label_new (nbuff);
    stop_button = gtk_button_new_with_label ("Cancel");
    g_signal_connect_swapped (stop_button, "clicked",
                              G_CALLBACK (event_stop_button_pressed),
                              &stop);

    progress = gtk_progress_bar_new ();
    gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (progress), TRUE);
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progress), nullptr);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), 0);

    box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box_main), 10);

    gtk_box_pack_start (GTK_BOX (box_main),
                        gtk_label_new ("Downloading file:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_main),
                        label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_main),
                        progress,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_main),
                        stop_button,
                        FALSE, FALSE, 4);

    gtk_container_add (GTK_CONTAINER (window), box_main);
    gtk_widget_show_all (window);

    /* Processing list of wallpapers to download */
    while (item_list != nullptr) {
        if (stop == 1)
            break;

        frac  += step;
        item  = item_list->data;

        nlen = (size_t) g_utf8_strlen (item->file_name, -1);

        if (nlen > NAME_LEN) {
            g_utf8_strncpy (nbuff, item->file_name, NAME_LEN);
            strcat (nbuff, "...");
        }
        else {
            strcpy (nbuff, item->file_name);
        }
        fn = str_comb (wpdir, "/");
        str_append (&fn, item->file_name);
        markup = g_markup_printf_escaped (format, nbuff);

        gtk_label_set_markup (GTK_LABEL (label), markup);
        g_free (markup);

        while (gtk_events_pending ())
            gtk_main_iteration ();
#ifdef DEBUG
        printf ("downloading:%s\nto:%s\n", item->image_url, fn);
#endif
        urldata_get_to_file (item->image_url, fn, &error);
        if (error != nullptr) {
            message_dialog_error (nullptr, error);
            free (error);
            free (fn);
            break;
        }
        /* The author and license have to stay known for the saved image */
        if ((res = save_image_info (item)) != ERR_OK && info_err == ERR_OK)
            info_err = res;

        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), frac);

        while (gtk_events_pending ())
            gtk_main_iteration ();

        result  = g_list_prepend (result, fn);
        item_list = item_list->next;
    }
    free (wpdir);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), 1.0);

    gtk_widget_destroy (window);

    if (info_err != ERR_OK) {
        message_dialog_error (parent,
                "Images were downloaded, but the information about their "
                "authors and licenses could not be saved.");
        warnx ("saving image info: %s", err_get_message (info_err));
    }
    return g_list_reverse (result);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get API key of the active service from Combobox.
 *
 * @param[in] combo  Combobox
 * @return    New string with the API key, empty if not set; free with free()
 */
[[nodiscard]] static char *
combo_get_active_api_key (GtkWidget *combo)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    char         *value   = nullptr; /* Key as stored in the model */
    char         *api_key = nullptr; /* Copy to return */

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
        gtk_tree_model_get (model, &iter, WW_COMBO_API_KEY, &value, -1);
    }
    api_key = strdup (value != nullptr ? value : "");
    g_free (value);

    return api_key;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set API key of the active service in Combobox.
 *
 * @param[in,out] combo    Combobox
 * @param[in]     api_key  API key value
 * @return        none
 */
static void
combo_set_active_api_key (GtkWidget  *combo,
                          const char *api_key)
{
    GtkListStore *list_store;
    GtkTreeIter   iter;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter)) {

        list_store = GTK_LIST_STORE (gtk_combo_box_get_model (
                    GTK_COMBO_BOX (combo)));

        gtk_list_store_set (list_store, &iter,
                            WW_COMBO_API_KEY, api_key,
                            -1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update labels and entry with srach results info.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
update_labels (WebWidget *widget)
{
    const char *format_count = "Found %d images";
    char       *markup       = nullptr;
    char        entry_text[128];

    markup = g_markup_printf_escaped (format_count,
                                        widget->found_cnt);
    gtk_label_set_markup (GTK_LABEL (widget->count_label), markup);
    g_free (markup);

    snprintf (entry_text, 128, "%d", widget->page);
    gtk_entry_set_text (GTK_ENTRY (widget->nav_entry), entry_text);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if image with specified file name exists on selected to
 *         download list.
 *
 * Images are compared by file name, because it holds the service name next
 * to the image id and ids of different services can be the same.
 *
 * @param[in] model      TreeModel to check
 * @param[in] file_name  Image file name to check
 * @return    1 if exists, 0 if not
 */
static int
sel_combo_check_exist (GtkTreeModel *model,
                       const char   *file_name)
{
    char       *read_name = nullptr;
    gboolean    res       = FALSE;
    GtkTreeIter iter;

    res = gtk_tree_model_get_iter_first (model, &iter);

    while (res) {

        gtk_tree_model_get (model,                 &iter,
                            WW_SELCOMBO_FILE_NAME, &read_name,
                            -1);
        if (g_strcmp0 (file_name, read_name) == 0) {
            free (read_name);
            return 1;
        }
        free (read_name);
        res = gtk_tree_model_iter_next (model, &iter);
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of SearchItem items from selected to download combo box.
 *
 * @param[in] selected_combo  Combobox to get list
 * @return    List of images to download in SearchItem items
 */
static GList *
sel_combo_get_list (GtkWidget *selected_combo)
{
    GList        *result = nullptr;
    SearchItem   *item   = nullptr;
    GtkTreeModel *model  = nullptr;
    gboolean      res    = FALSE;
    GtkTreeIter   iter;

    model = gtk_combo_box_get_model (
            GTK_COMBO_BOX (selected_combo));

    res = gtk_tree_model_get_iter_first (model, &iter);

    while (res) {

        item = search_item_new ();
        gtk_tree_model_get (model,               &iter,
                            WW_SELCOMBO_FILE_NAME,   &item->file_name,
                            WW_SELCOMBO_IMAGE_URL,   &item->image_url,
                            WW_SELCOMBO_SERVICE,     &item->service_name,
                            WW_SELCOMBO_TITLE,       &item->title,
                            WW_SELCOMBO_AUTHOR,      &item->author_name,
                            WW_SELCOMBO_LICENSE,     &item->license_name,
                            WW_SELCOMBO_LICENSE_URL, &item->license_url,
                            WW_SELCOMBO_PAGE_URL,    &item->page_url,
                            -1);
        result = g_list_prepend (result, item);
        res = gtk_tree_model_iter_next (model, &iter);
    }
    return g_list_reverse (result);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search web for images.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        true if the page of results was found and shown, false on
 *                error or when another search is still in progress
 */
static bool
search_web (WebWidget *widget)
{
    char *api_key = nullptr; /* API key of the active service */
    bool  res     = false;

    /* Showing results runs the main loop, so the buttons work during
     * a search */
    if (widget->searching)
        return false;

    widget->searching = true;

    /* Check if there is a cached info about this search query */
    if (check_for_cached_query (widget,
                                ww_name (widget->active_service))) {
        res = true;
    }
    else {
        api_key = combo_get_active_api_key (widget->combo);
        res     = ww_search (widget, api_key);
        free (api_key);
    }
    widget->searching = false;

    if (res)
        update_labels (widget);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_search_pressed (WebWidget *widget)
{
    const char *query = gtk_entry_get_text (GTK_ENTRY (widget->entry));
    char       *old_query = nullptr; /* Query of the shown results */
    int         old_page  = 0;       /* Page of the shown results */

    if (widget->searching)
        return;

    if (!check_unicode_warn (query,
                             "Search query is not a valid unicode text"))
        return;
    else if (str_is_empty_warn (query, "Empty search query"))
        return;
    else if (!str_is_alnum_or_space_warn (
                query, "Sorry, only alphanumerich characters allowed"))
        return;

    old_query = widget->query;
    old_page  = widget->page;

    widget->query = strdup (query);
    widget->page  = 1;

    if (search_web (widget)) {
        free (old_query);
    }
    else {
        /* The list still shows the results of the previous query */
        free (widget->query);
        widget->query = old_query;
        widget->page  = old_page;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search options button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_search_opts_pressed (WebWidget *widget)
{
    char *search_opts = nullptr; /* Search options string */

    if (widget->searching)
        return;

    search_opts = ww_search_opts_dialog (widget);

    if (search_opts != nullptr &&
        strcmp (search_opts, widget->search_opts) != 0) {

        free (widget->search_opts);
        widget->search_opts = strdup (search_opts);
    }
#ifdef DEBUG
            printf ("now opts : %s\n", widget->search_opts);
#endif
    free (search_opts);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Show another page of results of the current query.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @param[in]     page    Number of page to show
 * @return        none
 */
static void
show_page (WebWidget *widget,
           const int  page)
{
    const int old_page = widget->page;

    if (widget->query == nullptr || widget->searching)
        return;

    widget->page = page;

    /* On error the list still shows the previous page */
    if (!search_web (widget)) {
        widget->page = old_page;
        update_labels (widget);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Next page button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_next_pressed (WebWidget *widget)
{
    if (widget->page < INT_MAX)
        show_page (widget, widget->page + 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Previous page button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_prev_pressed (WebWidget *widget)
{
    if (widget->page > 1)
        show_page (widget, widget->page - 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enter pressed in page number entry.
 *
 * @param[in,out] entry   Page number entry
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_nav_entry_act (GtkWidget *entry,
                     WebWidget *widget)
{
    const char    *str = nullptr; /* String from entry */
    long long int  val = 0;       /* Int value of entry string */

    str = gtk_entry_get_text (GTK_ENTRY (entry));
    val = strtoll (str, nullptr, 10);
    if      (val < 1)       val = 1;
    else if (val > INT_MAX) val = INT_MAX;

    show_page (widget, (int) val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Settings button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_settings_pressed (WebWidget *widget)
{
    char *api_key = nullptr; /* API key of the active service */
    int   err = ERR_OK;      /* Error output */
    int   res = 0;           /* Dialog result */

    if (widget->searching)
        return;

    api_key = combo_get_active_api_key (widget->combo);
    res     = ww_api_key_dialog (widget->active_service, &api_key);

    if (res == GTK_RESPONSE_ACCEPT) {
        err = ww_update_api_key (widget->config_file,
                                 api_key,
                                 widget->active_service);
        combo_set_active_api_key (widget->combo, api_key);
    }
    if (err != ERR_OK) {
        message_dialog_error (nullptr, err_get_message (err));
    }
    free (api_key);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add selected images to list button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_add_selected_pressed (WebWidget *widget)
{
    GtkListStore *list_store;    /* Combobox ListStore */
    GtkTreeModel *model;         /* Iconview tree model */
    GtkTreeModel *combo_model;   /* Combobox tree model */
    GdkPixbuf    *pixbuf;        /* Thumbnail pixbuf */
    GdkPixbuf    *scaled_pixbuf; /* Smaller thumbnail pixbuf */
    GtkTreeIter   iter;          /* TreeIter */
    GtkTreeIter   new_iter;      /* TreeIter */
    GtkTreePath  *path;          /* TreePath */
    GList        *selected  = nullptr; /* List of selected items */
    GList        *node      = nullptr; /* List item for iteration */
    char         *disp_name = nullptr; /* Image display name */
    char         *file_name = nullptr; /* Image file name */
    char         *image_url = nullptr; /* Image url */
    char         *id        = nullptr; /* Image id */
    char         *service   = nullptr; /* Short name of image's service */
    char         *title     = nullptr; /* Image title */
    char         *author    = nullptr; /* Image author */
    char         *license   = nullptr; /* Image license name */
    char         *license_url = nullptr; /* Image license url */
    char         *page_url  = nullptr; /* Image page url */
    int           width     = 0;  /* Thumbnail width */
    int           height    = 0;  /* Thumbnail height */
    float         scaled_w  = 0;  /* Smaller thumbnail width */
    float         scaled_h  = 0;  /* Smaller thumbnail height */
    char          dname[SEL_NAME_LEN * 4 + 4]; /* For shorter name in combo */

    combo_model = gtk_combo_box_get_model (
            GTK_COMBO_BOX (widget->selected_combo));

    list_store = GTK_LIST_STORE (gtk_combo_box_get_model (
                GTK_COMBO_BOX (widget->selected_combo)));

    model = gtk_icon_view_get_model (GTK_ICON_VIEW (
                widget->img_view));

    selected = gtk_icon_view_get_selected_items (
            GTK_ICON_VIEW (widget->img_view));

    node = selected;

    while (node != nullptr) {

        path = node->data;

        if (gtk_tree_model_get_iter (model, &iter, path)) {

            scaled_w = SEL_TH_WIDTH;
            scaled_h = SEL_TH_HEIGHT;

            gtk_tree_model_get (model, &iter,
                                WEB_COL_PIXBUF,    &pixbuf,
                                WEB_COL_WIDTH,     &width,
                                WEB_COL_HEIGHT,    &height,
                                WEB_COL_ID,        &id,
                                WEB_COL_DISP_NAME, &disp_name,
                                WEB_COL_FILE_NAME, &file_name,
                                WEB_COL_IMAGE_URL, &image_url,
                                WEB_COL_SERVICE,   &service,
                                WEB_COL_TITLE,     &title,
                                WEB_COL_AUTHOR,    &author,
                                WEB_COL_LICENSE,   &license,
                                WEB_COL_LICENSE_URL, &license_url,
                                WEB_COL_PAGE_URL,  &page_url,
                                -1);

            if (file_name != nullptr && disp_name != nullptr &&
                sel_combo_check_exist (combo_model, file_name) == 0) {

                width = gdk_pixbuf_get_width  (pixbuf);
                height = gdk_pixbuf_get_height (pixbuf);

                if (g_utf8_strlen (disp_name, -1) > SEL_NAME_LEN) {
                    g_utf8_strncpy (dname, disp_name, SEL_NAME_LEN);
                    strcat (dname, "...");
                }
                else {
                    strcpy (dname, disp_name);
                }

                if (width > height) {
                    scaled_h = (float) height / (float) width * scaled_w;
                }
                else if (width < height) {
                    scaled_w = (float) width / (float) height * scaled_h;
                }
                scaled_pixbuf = gdk_pixbuf_scale_simple (pixbuf,
                                                    (int) scaled_w,
                                                    (int) scaled_h,
                                                    GDK_INTERP_HYPER);

                gtk_list_store_append (list_store, &new_iter);
                gtk_list_store_set (list_store, &new_iter,
                                    WW_SELCOMBO_PIXBUF,    scaled_pixbuf,
                                    WW_SELCOMBO_WIDTH,     width,
                                    WW_SELCOMBO_HEIGHT,    height,
                                    WW_SELCOMBO_ID,        id,
                                    WW_SELCOMBO_DISP_NAME, dname,
                                    WW_SELCOMBO_FILE_NAME, file_name,
                                    WW_SELCOMBO_IMAGE_URL, image_url,
                                    WW_SELCOMBO_SERVICE,
                                    ww_label (ww_site_from_name (service)),
                                    WW_SELCOMBO_TITLE,       title,
                                    WW_SELCOMBO_AUTHOR,      author,
                                    WW_SELCOMBO_LICENSE,     license,
                                    WW_SELCOMBO_LICENSE_URL, license_url,
                                    WW_SELCOMBO_PAGE_URL,    page_url,
                                    -1);
                g_object_unref(scaled_pixbuf);
            }
            free (id);
            free (disp_name);
            free (file_name);
            free (image_url);
            free (service);
            free (title);
            free (author);
            free (license);
            free (license_url);
            free (page_url);
            g_object_unref(pixbuf);
        }
        node = node->next;
    }
    g_list_free_full (selected, (GDestroyNotify) gtk_tree_path_free);

    if (gtk_combo_box_get_active (GTK_COMBO_BOX
                (widget->selected_combo)) == -1) {

        gtk_combo_box_set_active (GTK_COMBO_BOX (
                    widget->selected_combo), 0);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Refresh string with search options for active search service.
 *
 * @param[in,out] widget  WebWidget item
 * @return        none
 */
static void
refresh_service_opts (WebWidget *widget)
{
    Setting *settings = nullptr;
    Setting *setting  = nullptr;
    int      err      = 0;

    settings = setts_read (widget->config_file, &err);
    setting = setting_get_child (
            settings_find (settings, ww_opts (widget->active_service)));

    free (widget->search_opts);
    widget->search_opts = setting == nullptr ? strdup ("") :
                               search_opts_to_str (setting);
    settings_free_all (settings);
#ifdef DEBUG
    printf ("now opts : %s\n", widget->search_opts);
#endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make the settings and search options buttons active only when the
 *         active search service has an API key or search options to set.
 *
 * @param[in,out] widget  WebWidget item
 * @return        none
 */
static void
update_service_buttons (WebWidget *widget)
{
    const bool has_key  = ww_has_api_key (widget->active_service);
    const bool has_opts = ww_has_search_opts (widget->active_service);

    gtk_widget_set_sensitive (widget->sett_button, has_key);
    gtk_widget_set_tooltip_text (widget->sett_button,
            has_key ? "Service settings" :
                      "This service needs no settings");

    gtk_widget_set_sensitive (widget->opts_button, has_opts);
    gtk_widget_set_tooltip_text (widget->opts_button,
            has_opts ? "Search options" :
                       "This service has no search options");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed cobo box with image search service, sets active service
 *         active_service value in WebWidget.
 *
 * @param[in]  combo   Combobox which sent signal
 * @param[out] widget  WebWidget item
 * @return     none
 */
static void
event_service_combo_changed (GtkComboBox *combo,
                             WebWidget   *widget)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    if (gtk_combo_box_get_active_iter (combo, &iter)) {

        model = gtk_combo_box_get_model (combo);
        gtk_tree_model_get (model, &iter,
                WW_COMBO_ID, &widget->active_service, -1);

        refresh_service_opts (widget);
        update_service_buttons (widget);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create combo box for image search services.
 *
 * @return  Combobox widget
 */
static GtkWidget *
webwidget_combobox_create (Setting *settings)
{
    GtkWidget       *combo;       /* Result ComboBox */
    GtkCellRenderer *cell;        /* CellRenderer */
    GtkListStore    *list_store;  /* ListStore for combobox data */
    GtkTreeIter      iter;        /* TreeIter */
    GdkPixbuf       *logo     = nullptr; /* Service logo */
    char            *api_key  = nullptr; /* Service API key */
    int i = 0;

    /* Liststore for service data */
    list_store = gtk_list_store_new (WW_COMBO_CNT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     GDK_TYPE_PIXBUF);

    for (i = 0; i < WEB_SERV_CNT; ++i) {
        logo = get_icon (ww_logo_id (i));
        api_key = ww_get_api_key (settings, i);
        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter,
                            WW_COMBO_ID,      i,
                            WW_COMBO_NAME,    ww_label (i),
                            WW_COMBO_API_KEY, api_key,
                            WW_COMBO_LOGO,    logo,
                            -1);
        /* Not every service has a logo */
        if (logo != nullptr)
            g_object_unref (logo);
        free (api_key);
    }
    combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (combo),
                                   cell, "text", WW_COMBO_NAME);

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter))
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);

    return combo;
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
    GtkWidget       *combo;      /* Result ComboBox */
    GtkCellRenderer *cell;       /* CellRenderer */
    GtkListStore    *list_store; /* ListStore for combobox data */
    GtkTreeIter      iter;       /* TreeIter */

    list_store = gtk_list_store_new (WW_SELCOMBO_CNT,
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
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_pixbuf_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, FALSE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (combo),
                                   cell, "pixbuf", WW_SELCOMBO_PIXBUF);

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (combo),
                                   cell, "text", WW_SELCOMBO_DISP_NAME);

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter))
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);

    return combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the text of an image info label that shortens long texts and
 *         shows the whole text in its tooltip.
 *
 * @param[out] label  Label to set
 * @param[in]  text   Text to show, can be null
 * @return     none
 */
static void
info_label_set_text (GtkWidget  *label,
                     const char *text)
{
    gtk_label_set_text (GTK_LABEL (label), text != nullptr ? text : "");
    gtk_widget_set_tooltip_text (label, str_is_empty (text) ? nullptr : text);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Image on preview list activated, get image info and set image
 *         info widgets.
 *
 * @param[in]  iconview   IconView which sent signal
 * @param[in]  path       TreePath of active icon
 * @param[out] widget  WebWidget item
 */
static void
event_imgview_activated (GtkIconView *iconview,
                         GtkTreePath *path,
                         WebWidget   *widget)
{
    GtkTreeModel *model;
    GdkPixbuf    *logo         = nullptr;
    char         *title        = nullptr;
    char         *license      = nullptr;
    char         *license_url  = nullptr;
    char         *service      = nullptr;
    char         *page_url     = nullptr;
    char         *image_author = nullptr;
    int           width        = 0;
    int           height       = 0;
    int           site         = 0;
    GtkTreeIter   iter;
    char dim[128];
    dim[0] = '\0';

    model = gtk_icon_view_get_model (iconview);

    if (gtk_tree_model_get_iter (model, &iter, path)) {
        gtk_tree_model_get (model, &iter,
                            WEB_COL_WIDTH,       &width,
                            WEB_COL_HEIGHT,      &height,
                            WEB_COL_AUTHOR,      &image_author,
                            WEB_COL_PAGE_URL,    &page_url,
                            WEB_COL_TITLE,       &title,
                            WEB_COL_LICENSE,     &license,
                            WEB_COL_LICENSE_URL, &license_url,
                            WEB_COL_SERVICE,     &service,
                            -1);
        info_label_set_text (widget->title_label, title);
        info_label_set_text (widget->author_label, image_author);

        gtk_link_button_set_uri (GTK_LINK_BUTTON (widget->page_link),
                                 page_url != nullptr ? page_url : "");
        gtk_widget_set_sensitive (widget->page_link, page_url != nullptr);

        /* License name with a link to the license text if there is one */
        gtk_button_set_label (GTK_BUTTON (widget->license_link),
                              license != nullptr ? license : "Unknown");
        gtk_link_button_set_uri (GTK_LINK_BUTTON (widget->license_link),
                                 license_url != nullptr ? license_url : "");
        gtk_widget_set_sensitive (widget->license_link,
                                  license_url != nullptr);

        if (width && height) {
            sprintf (dim, "[%dx%d]", width, height);
        }
        gtk_label_set_text (GTK_LABEL (widget->dim_label), dim);

        /* The list can show images of other service than the active one */
        site = ww_site_from_name (service);
        logo = get_icon (ww_logo_id (site));

        gtk_image_clear (GTK_IMAGE (widget->service_image));

        if (logo != nullptr) {
            gtk_image_set_from_pixbuf (GTK_IMAGE (widget->service_image),
                                       logo);
            gtk_label_set_text (GTK_LABEL (widget->service_label), "");
            g_object_unref (logo);
        }
        else {
            gtk_label_set_text (GTK_LABEL (widget->service_label),
                                ww_label (site) != nullptr ?
                                ww_label (site) : "");
        }
        free (title);
        free (license);
        free (license_url);
        free (service);
        free (image_author);
        free (page_url);
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
    GtkWidget    *icon_view;
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
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (list_store));

    gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view),
                                      GTK_SELECTION_MULTIPLE);
    /* gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view),
                                   WEB_COL_DISP_NAME); */
    gtk_icon_view_set_markup_column (GTK_ICON_VIEW (icon_view),
                                     WEB_COL_MARKUP);
    gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view),
                                     WEB_COL_PIXBUF);
    gtk_icon_view_set_item_width (GTK_ICON_VIEW (icon_view), 210);
    gtk_icon_view_set_columns (GTK_ICON_VIEW (icon_view), 3);

    return icon_view;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create widget with image info widgets.
 *
 * @return  Result widget
 */
static GtkWidget *
webwidget_image_info_create (WebWidget *widget)
{
    GtkWidget *result;
    GtkWidget *service_box;
    GtkWidget *service_image;
    GtkWidget *service_label;
    GtkWidget *title_label;
    GtkWidget *author_label;
    GtkWidget *dim_label;
    GtkWidget *image_link;
    GtkWidget *license_link;
    GtkWidget *grid;

    service_image = gtk_image_new ();
    service_label = gtk_label_new (nullptr);
    title_label   = gtk_label_new (nullptr);
    author_label  = gtk_label_new (nullptr);
    dim_label     = gtk_label_new (nullptr);
    image_link    = gtk_link_button_new_with_label ("", "Image page");
    license_link  = gtk_link_button_new_with_label ("", "-");
    grid          = gtk_grid_new ();

    /* Nothing to link to until an image is chosen */
    gtk_widget_set_sensitive (image_link, FALSE);
    gtk_widget_set_sensitive (license_link, FALSE);

    /* Titles and author names can be long */
    gtk_label_set_ellipsize (GTK_LABEL (title_label), PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars (GTK_LABEL (title_label), INFO_TEXT_LEN);
    gtk_label_set_ellipsize (GTK_LABEL (author_label), PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars (GTK_LABEL (author_label), INFO_TEXT_LEN);

    /* Service logo, or its name when it has no logo */
    service_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (service_box), service_image, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (service_box), service_label, FALSE, FALSE, 0);

    gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing (GTK_GRID (grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 16);

    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Image provided by:"),
                     0,0,2,1);
    gtk_grid_attach (GTK_GRID (grid),
                     service_box,
                     0,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     image_link,
                     1,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Title:"),
                     2,0,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     title_label,
                     2,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Author:"),
                     3,0,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     author_label,
                     3,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("License:"),
                     4,0,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     license_link,
                     4,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Dimensions:"),
                     5,0,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     dim_label,
                     5,1,1,1);

    widget->service_image = service_image;
    widget->service_label = service_label;
    widget->title_label   = title_label;
    widget->author_label  = author_label;
    widget->page_link     = image_link;
    widget->license_link  = license_link;
    widget->dim_label     = dim_label;

    result = grid;

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free WebWidget item.
 */
void
webwidget_free (WebWidget *widget)
{
    free (widget->query);
    free (widget->search_opts);
    free (widget->config_file);
    free (widget->wallpaper_dir);
    free (widget);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init WebWidget item.
 *
 * @param[out] widget  WebWidget item
 * @return     none
 */
static void
webwidget_init (WebWidget *widget)
{
    widget->query          = nullptr;
    widget->search_opts    = nullptr;
    widget->config_file    = nullptr;
    widget->wallpaper_dir  = nullptr;
    widget->page           = 0;
    widget->found_cnt      = 0;
    widget->active_service = 0;
    widget->thumb_quality  = 0;
    widget->searching      = false;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create WebWidget item with widgets and data for wallpaper search.
 */
WebWidget *
webwidget_create (Setting    *settings,
                  const char *cfg_file)
{
    WebWidget *widget;
    GtkWidget *search_box;
    GtkWidget *web_combo;
    GtkWidget *img_view;
    GtkWidget *sett_button;
    GtkWidget *search_entry;
    GtkWidget *search_button;
    GtkWidget *search_opts_btn;
    GtkWidget *nav_box;
    GtkWidget *nav_prev;
    GtkWidget *nav_next;
    GtkWidget *nav_entry;
    GtkWidget *count_label;
    GtkWidget *add_sltd_box;
    GtkWidget *add_sltd_button;
    GtkWidget *selected_combo;
    GtkWidget *img;
    GdkPixbuf *pixbuf = nullptr;
    Setting   *setting = nullptr;

    if ((widget = malloc (sizeof (WebWidget))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    webwidget_init (widget);

    for (int site = 0; site < WEB_SERV_CNT; ++site)
        cache_query_delete_older_than (ww_name (site), 1);

    if ((setting = settings_find (settings,
                   get_setting_name (SETT_THUMB_QUALITY))) != nullptr) {
        widget->thumb_quality = (int) setting_get_int (setting);
    }
    widget->config_file   = strdup (cfg_file);
    widget->wallpaper_dir  = cfg_file_get_app_wallpapers_path ();
    widget->image_info_widget = webwidget_image_info_create (widget);

    web_combo = webwidget_combobox_create (settings);
    img_view  = webwidget_imgview_create ();

    g_signal_connect (web_combo, "changed",
            G_CALLBACK (event_service_combo_changed), widget);
    g_signal_connect (img_view, "item-activated",
            G_CALLBACK (event_imgview_activated), widget);

    count_label = gtk_label_new ("Found 0 images");

    /* Search box */
    search_box      = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    search_entry    = gtk_search_entry_new ();
    search_button   = gtk_button_new_with_label ("Search");
    search_opts_btn = gtk_button_new_with_label ("Search opts");
    sett_button     = gtk_button_new ();
    pixbuf = get_icon (W_ICON_SETTING);
    img = gtk_image_new_from_pixbuf (pixbuf);
    gtk_button_set_image (GTK_BUTTON (sett_button), img);
    g_object_unref (pixbuf);

    g_signal_connect_swapped (sett_button, "clicked",
            G_CALLBACK (event_settings_pressed), widget);
    g_signal_connect_swapped (search_entry, "activate",
            G_CALLBACK (event_search_pressed), widget);
    g_signal_connect_swapped (search_button, "clicked",
            G_CALLBACK (event_search_pressed), widget);
    g_signal_connect_swapped (search_opts_btn, "clicked",
            G_CALLBACK (event_search_opts_pressed), widget);

    gtk_box_pack_start (GTK_BOX (search_box),
                        web_combo,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (search_box),
                        sett_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (search_box),
                        search_entry,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (search_box),
                        search_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (search_box),
                        search_opts_btn,
                        FALSE, FALSE, 4);

    /* Navigation box */
    nav_box = gtk_grid_new ();
    /*nav_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);*/
    gtk_widget_set_halign (nav_box, GTK_ALIGN_CENTER);
    nav_prev  = gtk_button_new_with_label ("Prev page");
    nav_next  = gtk_button_new_with_label ("Next page");
    nav_entry = gtk_entry_new ();
    gtk_entry_set_width_chars (GTK_ENTRY (nav_entry), 4);
    gtk_entry_set_text (GTK_ENTRY (nav_entry), "1");
    gtk_grid_set_row_spacing (GTK_GRID (nav_box), 8);
    gtk_grid_set_column_spacing (GTK_GRID (nav_box), 8);
    gtk_grid_attach (GTK_GRID (nav_box), nav_prev,
                     0,0,1,1);
    gtk_grid_attach (GTK_GRID (nav_box), nav_entry,
                     1,0,1,1);
    gtk_grid_attach (GTK_GRID (nav_box), nav_next,
                     2,0,1,1);

    g_signal_connect_swapped (nav_prev, "clicked",
            G_CALLBACK (event_prev_pressed), widget);
    g_signal_connect_swapped (nav_next, "clicked",
            G_CALLBACK (event_next_pressed), widget);
    g_signal_connect (nav_entry, "activate",
            G_CALLBACK (event_nav_entry_act), widget);

    /* Add selected images box */
    add_sltd_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    add_sltd_button = gtk_button_new_with_label (
            "Add selected images to list");
    selected_combo = webwidget_selected_combobox_create ();

    gtk_box_pack_start (GTK_BOX (add_sltd_box),
                        add_sltd_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (add_sltd_box),
                        selected_combo,
                        TRUE, TRUE, 4);
    g_signal_connect_swapped (add_sltd_button, "clicked",
            G_CALLBACK (event_add_selected_pressed), widget);

    widget->search_box     = search_box;
    widget->combo          = web_combo;
    widget->entry          = search_entry;
    widget->sett_button    = sett_button;
    widget->opts_button    = search_opts_btn;
    widget->nav_entry      = nav_entry;
    widget->img_view       = img_view;
    widget->nav_box        = nav_box;
    widget->selected_combo = selected_combo;
    widget->selected_box   = add_sltd_box;
    widget->count_label    = count_label;
    widget->active_service = WEB_SERV_WALLHAVEN;
    widget->search_opts    = strdup ("");

    refresh_service_opts (widget);
    update_service_buttons (widget);

    return widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save selected wallpapers in user's app share folder.
 */
GList *
save_selected_wallpapers (GtkWidget *dialog,
                          WebWidget *widget)
{
    GList *items  = nullptr; /* List of images selected to download */
    GList *result = nullptr; /* Result list od downloaded wallpapers */

    items = sel_combo_get_list (widget->selected_combo);
    result = download_progress_window (GTK_WINDOW (dialog), items);
    g_list_free_full (items, (GDestroyNotify) search_item_free);

    return result;
}
/*----------------------------------------------------------------------------*/
