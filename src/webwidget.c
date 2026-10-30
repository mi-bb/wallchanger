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
#include "url_data.h"
#include "json_functions.h"
#include "search_item.h"
#include "message_dialogs.h"
#include "string_functions.h"
#include "settings.h"
#include "errors.h"
#include "cfg_file.h"
#include "file_dir_functions.h"
#include "defs.h"
#include "icons.h"
#include "nstrings.h"
#include "webwidget_common.h"
#include "web_pixabay.h"
#include "web_wallhaven.h"
#include "web_abyss.h"
#include "web_flickr.h"
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
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), frac);

        while (gtk_events_pending ())
            gtk_main_iteration ();

        result  = g_list_prepend (result, fn);
        item_list = item_list->next;
    }
    free (wpdir);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), 1.0);

    gtk_widget_destroy (window);

    return g_list_reverse (result);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get strings with API key values from Combobox.
 *
 * @param[in]  combo  Combobox
 * @param[out] nstrings   n strings to write API keys
 * @return     none
 */
static void
combo_get_active_strings (GtkWidget *combo,
                          NStrings  *nstrings)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
        gtk_tree_model_get (model,          &iter,
                            WW_COMBO_STR_1, &nstrings->str[0],
                            WW_COMBO_STR_2, &nstrings->str[1],
                            WW_COMBO_STR_3, &nstrings->str[2],
                            WW_COMBO_STR_4, &nstrings->str[3],
                            -1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set strings with API key values to Combobox.
 *
 * @param[in]  combo  Combobox
 * @param[out] nstrings   n strings with API keys
 * @return     none
 */
static void
combo_set_active_strings (GtkWidget      *combo,
                          const NStrings *nstrings)
{
    GtkListStore *list_store;
    GtkTreeIter   iter;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter)) {

        list_store = GTK_LIST_STORE (gtk_combo_box_get_model (
                    GTK_COMBO_BOX (combo)));

        gtk_list_store_set (list_store,    &iter,
                            WW_COMBO_STR_1, nstrings->str[0],
                            WW_COMBO_STR_2, nstrings->str[1],
                            WW_COMBO_STR_3, nstrings->str[2],
                            WW_COMBO_STR_4, nstrings->str[3],
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
 * @brief  Check if image with specified id exists on selected to download list.
 *
 * @param[in] model  TreeModel to check
 * @param[in] id       Image id to check
 * @return    1 if exists, 0 if not
 */
static int
sel_combo_check_exist (GtkTreeModel *model,
                       const char   *id)
{
    char       *read_id = nullptr;
    gboolean    res     = FALSE;
    GtkTreeIter iter;

    res = gtk_tree_model_get_iter_first (model, &iter);

    while (res) {

        gtk_tree_model_get (model,      &iter,
                            WW_SELCOMBO_ID, &read_id,
                            -1);
        if (strcmp (id, read_id) == 0) {
            free (read_id);
            return 1;
        }
        free (read_id);
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
        gtk_tree_model_get (model,             &iter,
                            WW_SELCOMBO_FILE_NAME, &item->file_name,
                            WW_SELCOMBO_IMAGE_URL, &item->image_url,
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
 * @return        none
 */
static void
search_web (WebWidget *widget)
{
    NStrings *nstrings = nullptr; /* For 4 strings for API key data */

    /* Check if there is a cached info about this search query */
    if (check_for_cached_query (widget,
                                ww_name (widget->active_service))) {
        update_labels (widget);
        return;
    }

    nstrings = nstrings_new (4, NS_VAL_NULL);

    combo_get_active_strings (widget->combo, nstrings);

    switch (widget->active_service) {
        case WEB_SERV_PIXABAY:
            pixabay_search (widget, nstrings);
            break;
        case WEB_SERV_WALLHAVEN:
            wallhaven_search (widget, nstrings);
            break;
        case WEB_SERV_WALLABYSS:
            wallpaperabyss_search (widget, nstrings);
            break;
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            flickr_search (widget, nstrings);
            break;
#endif
        default:
            break;
    }
    nstrings_free (nstrings);
    update_labels (widget);
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

    if (!check_unicode_warn (query,
                             "Search query is not a valid unicode text"))
        return;
    else if (str_is_empty_warn (query, "Empty search query"))
        return;
    else if (!str_is_alnum_or_space_warn (
                query, "Sorry, only alphanumerich characters allowed"))
        return;

    if (widget->query != nullptr)
        free (widget->query);

    widget->query = strdup (query);
    widget->page = 1;
    search_web (widget);
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
    int   id          = 0;       /* Service id */

    id = widget->active_service;

    switch (id) {
        case WEB_SERV_PIXABAY:
            search_opts = pixabay_search_opts_dialog (widget);
            break;
        case WEB_SERV_WALLHAVEN:
            search_opts = wallhaven_search_opts_dialog (widget);
            break;
        case WEB_SERV_WALLABYSS:
            search_opts = wallpaperabyss_search_opts_dialog (widget);
            break;
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            /* flickr_search_opts_dialog (widget); */
            break;
#endif
        default:
            break;
    }
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
 * @brief  Next page button pressed.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        none
 */
static void
event_next_pressed (WebWidget *widget)
{
    if (widget->query != nullptr && widget->page < INT_MAX) {
        ++widget->page;
        search_web (widget);
    }
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
    if (widget->query != nullptr && widget->page > 1) {
        --widget->page;
        search_web (widget);
    }
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

    if (widget->query != nullptr) {
        str = gtk_entry_get_text (GTK_ENTRY (entry));
        val = strtoll (str, nullptr, 10);
        if      (val < 1)       val = 1;
        else if (val > INT_MAX) val = INT_MAX;
        widget->page = (int) val;
        search_web (widget);
    }
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
    NStrings *nstrings = nullptr; /* Four strings for API key data */
    int       err = ERR_OK; /* Error output */
    int       id  = 0;      /* Service id */
    int       res = 0;      /* Dialog result */

    id    = widget->active_service;
    nstrings = nstrings_new (4, NS_VAL_NULL);

    combo_get_active_strings (widget->combo, nstrings);

    switch (id) {
        case WEB_SERV_PIXABAY:
            res = pixabay_settings_dialog (nstrings);
            break;
        case WEB_SERV_WALLHAVEN:
            res = wallhaven_settings_dialog (nstrings);
            break;
        case WEB_SERV_WALLABYSS:
            res = wallpaperabyss_settings_dialog (nstrings);
            break;
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            res = flickr_settings_dialog (nstrings);
            break;
#endif
        default:
            break;
    }
    if (res == GTK_RESPONSE_ACCEPT) {
        err = ww_update_api_key_data (widget->config_file,
                                        nstrings,
                                        widget->active_service);
        combo_set_active_strings (widget->combo, nstrings);
    }
    if (err != ERR_OK) {
        message_dialog_error (nullptr, err_get_message (err));
    }
    nstrings_free (nstrings);
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
                                -1);

            if (sel_combo_check_exist (combo_model, id) == 0) {

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
                                    -1);
                g_object_unref(scaled_pixbuf);
            }
            free (id);
            free (disp_name);
            free (file_name);
            free (image_url);
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
    NStrings        *nstrings = nullptr; /* For API keys */
    int i = 0;

    /* Liststore for service data */
    list_store = gtk_list_store_new (WW_COMBO_CNT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     GDK_TYPE_PIXBUF);

    for (i = 0; i < WEB_SERV_CNT; ++i) {
        logo = get_icon (ww_logo_id (i));
        nstrings = ww_get_api_key_data (settings, i);
        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter,
                            WW_COMBO_ID,    i,
                            WW_COMBO_NAME,  ww_name (i),
                            WW_COMBO_STR_1, nstrings->str[0],
                            WW_COMBO_STR_2, nstrings->str[1],
                            WW_COMBO_STR_3, nstrings->str[2],
                            WW_COMBO_STR_4, nstrings->str[3],
                            WW_COMBO_LOGO,  logo,
                            -1);
        g_object_unref (logo);
        nstrings_free (nstrings);
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
    char         *disp_name    = nullptr;
    char         *file_name    = nullptr;
    char         *image_url    = nullptr;
    char         *page_url     = nullptr;
    char         *image_author = nullptr;
    int           width        = 0;
    int           height       = 0;
    GtkTreeIter   iter;
    char dim[128];
    dim[0] = '\0';

    model = gtk_icon_view_get_model (iconview);

    if (gtk_tree_model_get_iter (model, &iter, path)) {
        gtk_tree_model_get (model, &iter,
                            WEB_COL_WIDTH,     &width,
                            WEB_COL_HEIGHT,    &height,
                            WEB_COL_DISP_NAME, &disp_name,
                            WEB_COL_AUTHOR,    &image_author,
                            WEB_COL_FILE_NAME, &file_name,
                            WEB_COL_PAGE_URL,  &page_url,
                            WEB_COL_IMAGE_URL, &image_url,
                            -1);
        gtk_label_set_text (GTK_LABEL (widget->author_label),
                            image_author != nullptr ? image_author : "");
        gtk_link_button_set_uri (GTK_LINK_BUTTON (widget->page_link),
                                 page_url != nullptr ? page_url : "");
        if (width && height) {
            sprintf (dim, "[%dx%d]", width, height);
        }
        gtk_label_set_text (GTK_LABEL (widget->dim_label), dim);

        gtk_image_clear (GTK_IMAGE (widget->service_image));

        logo = get_icon (ww_logo_id (widget->active_service));

        if (logo != nullptr) {
            gtk_image_set_from_pixbuf (GTK_IMAGE (widget->service_image),
                                       logo);
        }
        g_object_unref (logo);
        free (disp_name);
        free (image_author);
        free (file_name);
        free (image_url);
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
    GtkWidget *service_image;
    GtkWidget *author_label;
    GtkWidget *dim_label;
    GtkWidget *image_link;
    GtkWidget *grid;

    service_image = gtk_image_new ();
    author_label  = gtk_label_new (nullptr);
    dim_label     = gtk_label_new (nullptr);
    image_link    = gtk_link_button_new ("Image page");
    grid          = gtk_grid_new ();

    gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing (GTK_GRID (grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 16);

    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Image provided by:"),
                     0,0,2,1);
    gtk_grid_attach (GTK_GRID (grid),
                     service_image,
                     0,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     image_link,
                     1,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Author:"),
                     2,0,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     author_label,
                     2,1,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Dimensions:"),
                     3,0,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     dim_label,
                     3,1,1,1);

    widget->service_image = service_image;
    widget->author_label  = author_label;
    widget->page_link     = image_link;
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

    cache_query_delete_older_than (ww_name (WEB_SERV_PIXABAY),   1);
    cache_query_delete_older_than (ww_name (WEB_SERV_WALLHAVEN), 1);
    cache_query_delete_older_than (ww_name (WEB_SERV_WALLABYSS), 1);
#ifdef HAVE_FLICKCURL
    cache_query_delete_older_than (ww_name (WEB_SERV_FLICKR),    1);
#endif

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
    gtk_widget_set_tooltip_text (sett_button, "Service settings");
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
    widget->nav_entry      = nav_entry;
    widget->img_view       = img_view;
    widget->nav_box        = nav_box;
    widget->selected_combo = selected_combo;
    widget->selected_box   = add_sltd_box;
    widget->count_label    = count_label;
    widget->active_service = WEB_SERV_WALLHAVEN;
    widget->search_opts    = strdup ("");

    refresh_service_opts (widget);

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
