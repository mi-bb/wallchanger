/**
 * @file  webwidget_common.c
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
 * @brief  Common functions for webwidgets.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <err.h>
#include "gui/message_dialogs.h"
#include "thumbnail.h"
#include "url_data.h"
#include "gui/icons.h"
#include "util/file_dir_functions.h"
#include "util/errors.h"
#include "cache_query.h"
#include "util/string_functions.h"
#include "settings/settings.h"
#include "web_provider.h"
#include "web_wallhaven.h"
#include "web_commons.h"
#include "web_openverse.h"
#include "web_artic.h"
#include "web_met.h"
#include "web_nasa.h"
#include "web_smithsonian.h"
#include "webwidget_common.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   providers
 * @brief Search services, indexed with the e_wall_sites values.
 */
static const WebProvider *const providers[WEB_SERV_CNT] = {
    [WEB_SERV_WALLHAVEN]   = &wallhaven_provider,
    [WEB_SERV_COMMONS]     = &commons_provider,
    [WEB_SERV_OPENVERSE]   = &openverse_provider,
    [WEB_SERV_ARTIC]       = &artic_provider,
    [WEB_SERV_MET]         = &met_provider,
    [WEB_SERV_NASA]        = &nasa_provider,
    [WEB_SERV_SMITHSONIAN] = &smithsonian_provider
};
/*----------------------------------------------------------------------------*/
/**
 * @struct SearchRun
 *
 * @brief  Data of one search passed to the function receiving found images.
 *
 * @var   SearchRun::widget
 * @brief WebWidget showing the results
 *
 * @var   SearchRun::provider
 * @brief Searched service
 *
 * @var   SearchRun::cache
 * @brief Query cache entry collecting the found images
 */
typedef struct SearchRun {
    WebWidget         *widget;
    const WebProvider *provider;
    CacheQuery        *cache;
} SearchRun;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the description of a search service.
 *
 * @param[in] site  Site id
 * @return    Service description, null for an unknown site
 */
static const WebProvider *
ww_provider (const int site)
{
    return (site >= 0 && site < WEB_SERV_CNT) ? providers[site] : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the short name of a search service.
 */
const char *
ww_name (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr ? provider->name : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the name of a search service to show to the user.
 */
const char *
ww_label (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr ? provider->label : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the id of the search service with the given short name.
 */
int
ww_site_from_name (const char *name)
{
    for (int site = 0; name != nullptr && site < WEB_SERV_CNT; ++site) {
        if (strcmp (providers[site]->name, name) == 0)
            return site;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the name of the setting with the search options of a service.
 */
const char *
ww_opts (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr ? provider->opts_name : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the logo image id of a search service.
 */
int
ww_logo_id (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr ? provider->logo : W_IMG_COUNT;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if a search service uses an API key.
 */
bool
ww_has_api_key (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr && provider->api_key_name != nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if a search service has search options.
 */
bool
ww_has_search_opts (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr &&
           (provider->opts_dialog != nullptr || provider->options != nullptr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the name of the setting that holds a service's API key.
 *
 * @param[in] site  Site id
 * @return    Setting name, or nullptr for an unknown site and for a service
 *            that does not use a key
 */
static const char *
ww_api_key_name (const int site)
{
    const WebProvider *provider = ww_provider (site);

    return provider != nullptr ? provider->api_key_name : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get API key of given service from settings.
 */
char *
ww_get_api_key (Setting  *settings,
                const int site)
{
    const char *name    = ww_api_key_name (site);
    Setting    *setting = nullptr;  /* Setting with the API key */
    const char *api_key = nullptr;  /* API key value */

    if (name != nullptr)
        setting = settings_find (settings, name);
    if (setting != nullptr)
        api_key = setting_get_string (setting);

    return strdup (api_key != nullptr ? api_key : "");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update API key in settings.
 */
int
ww_update_api_key (const char *cfg_file,
                   const char *api_key,
                   const int   site)
{
    const char *name = ww_api_key_name (site);
    Setting    *settings;     /* Setting with the API key */
    int         res = ERR_OK; /* Error output */

    if (name == nullptr)
        return ERR_OK;

    settings = setting_new_string (name, api_key);
    res = setts_check_update_file (cfg_file, settings);
    settings_free_all (settings);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string str is null or empty and show message msg
 *         if it is.
 */
int
str_is_empty_warn (const char *str,
                   const char *msg)
{
    if (str_is_empty (str)) {
        if (msg != nullptr)
            message_dialog_warning (nullptr, msg);
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string str is made of alphanumeric chars and spaces
 *         and show message msg if it is not.
 */
int
str_is_alnum_or_space_warn (const char *str,
                            const char *msg)
{
    gunichar c; /* Uni char to examine */

    while (*str != '\0') {
        c = g_utf8_get_char (str);
        if (!g_unichar_isalnum (c) && !g_unichar_isspace (c)) {
            if (msg != nullptr)
                message_dialog_warning (nullptr, msg);
            return 0;
        }
        str = g_utf8_find_next_char (str, nullptr);
    }
    return 1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string str is made of unicode chars
 *         and show message msg if it is not.
 */
int
check_unicode_warn (const char *str,
                    const char *msg)
{
    if (!g_utf8_validate (str, -1, nullptr)) {
        if (msg != nullptr)
            message_dialog_warning (nullptr, msg);
        return 0;
    }
    return 1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check from string str all non alphanumeric chars and non spaces.
 */
void
remove_non_alpha_space (char *str)
{
    gunichar  c;    /* Uni char to examine */
    char     *next; /* Position of next char */

    while (*str != '\0') {
        c = g_utf8_get_char (str);
        if (!g_unichar_isalnum (c) && !g_unichar_isspace (c)) {
            next = g_utf8_find_next_char (str, nullptr);
            memmove (str, next, strlen (next) + 1);
        }
        else {
            str = g_utf8_find_next_char (str, nullptr);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string value from Combobox's active col column.
 */
char *
combo_get_active_str (GtkWidget *combo,
                      const int  col)
{
    GtkTreeModel *model; /* TreeModel */
    GtkTreeIter   iter;  /* TreeIter */
    char *ret = nullptr; /* Return string */

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
        gtk_tree_model_get (model, &iter, col, &ret, -1);
    }
    return ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if given wallpaper is already in wallpaper dir and mark pixbuf
 *         with check mark if it is.
 *
 * @param[in]  wallpaper_dir  Wallpaper directory
 * @param[in]  file_name      Wallpaper file to check
 * @param[out] pixbuf         Thumbnail pixbuf
 * @return     none
 */
static void
wallpaper_check_mark (const char *wallpaper_dir,
                      const char *file_name,
                      GdkPixbuf  *pixbuf)
{
    GdkPixbuf *check_pixbuf = nullptr; /* Pixbuf for check image */
    char      *file_path    = nullptr; /* Wallpaper file path */

    file_path = str_comb (wallpaper_dir, "/");
    str_append (&file_path, file_name);

    if (file_check_permissions (file_path) == ERR_OK) {

        check_pixbuf = get_icon (W_IMG_CHECK);
        gdk_pixbuf_composite (check_pixbuf, pixbuf,
                              0, 0,
                              gdk_pixbuf_get_width (check_pixbuf),
                              gdk_pixbuf_get_height (check_pixbuf),
                              0, 0,
                              1.0, 1.0,
                              GDK_INTERP_HYPER,
                              255);
        g_object_unref (check_pixbuf);
    }
    free (file_path);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add image based on item data to icon view.
 */
void
add_search_item_to_img_view (GtkWidget        *iconview,
                             const SearchItem *item,
                             const char       *wallpaper_dir,
                             const char       *service_name,
                             const int         thumb_quality)
{
    GtkListStore *list_store;       /* ListStore */
    GdkPixbuf    *pixbuf = nullptr; /* Pixbuf with thumbnail */
    GtkTreeIter   iter;             /* TreeIter */

    list_store = GTK_LIST_STORE (gtk_icon_view_get_model (
                GTK_ICON_VIEW (iconview)));

    if (item->thumb_url == nullptr)
        return;

    pixbuf = thumbnail_get (service_name,
                             item->id,
                             thumb_quality,
                             item->thumb_url);

    if (pixbuf != nullptr) {

        wallpaper_check_mark (wallpaper_dir, item->file_name, pixbuf);

        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           WEB_COL_PIXBUF,    pixbuf,
                           WEB_COL_WIDTH,     item->width,
                           WEB_COL_HEIGHT,    item->height,
                           WEB_COL_ID,        item->id,
                           WEB_COL_DISP_NAME, item->display_name,
                           WEB_COL_AUTHOR,    item->author_name,
                           WEB_COL_MARKUP,    item->display_markup,
                           WEB_COL_FILE_NAME, item->file_name,
                           WEB_COL_PAGE_URL,  item->page_url,
                           WEB_COL_IMAGE_URL, item->image_url,
                           WEB_COL_THUMB_URL, item->thumb_url,
                           WEB_COL_TITLE,     item->title,
                           WEB_COL_LICENSE,   item->license_name,
                           WEB_COL_LICENSE_URL, item->license_url,
                           WEB_COL_SERVICE,   service_name,
                           -1);
        g_object_unref (pixbuf);
    }
    while (gtk_events_pending ())
        gtk_main_iteration ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check for search query cached info, get info and add items if info
 *         was found.
 */
int
check_for_cached_query (WebWidget  *widget,
                        const char *service_name)
{
    CacheQuery *cached_query = nullptr; /* For checking cache availability */
    int err = 0; /* Error output */
    int i   = 0;

    cached_query = cache_query_check_query (service_name,
                                            widget->query,
                                            widget->search_opts,
                                            widget->page,
                                            &err);
    if (err != ERR_OK) {
        message_dialog_error (nullptr, err_get_message (err));
    }
    if (cached_query != nullptr) {
        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (widget->img_view))));

        widget->found_cnt = cached_query->found_cnt;

        for (i = 0; i < cached_query->count; ++i) {
            add_search_item_to_img_view (widget->img_view,
                                         cached_query->items[i],
                                         widget->wallpaper_dir,
                                         service_name,
                                         widget->thumb_quality);
        }
        cache_query_free (cached_query);
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Converts image search options in Setting format to string for url.
 */
char *
search_opts_to_str (const Setting *settings)
{
    char          *res     = nullptr; /* String to return */
    const Setting *setting = nullptr; /* For settings iteration */
    char           buff[32];          /* Text buffer */

    res = strdup ("");
    setting = settings;

    while (setting != nullptr) {
        str_append (&res, "&");
        str_append (&res, setting_get_name (setting));
        str_append (&res, "=");
        if (setting_get_type (setting) == SET_VAL_INT) {
            sprintf (buff, "%" PRId64, setting_get_int (setting));
            str_append (&res, buff);
        }
        else if (setting_get_type (setting) == SET_VAL_STRING) {
            str_append (&res, setting_get_string (setting));
        }
        setting = setting->next;
    }
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog for setting the API key of a search service.
 */
int
ww_api_key_dialog (const int   site,
                   char      **api_key)
{
    const WebProvider *provider = ww_provider (site);
    GtkWidget *dialog;      /* Service settings dialog */
    GtkWidget *content_box; /* Dialog's box */
    GtkWidget *api_entry;   /* Entry for API key */
    GtkWidget *info_label;  /* Label with info about the key */
    char      *text = nullptr; /* Dialog title and entry label */
    int        res  = 0;    /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    if (provider == nullptr || provider->api_key_name == nullptr)
        return 0;

    text   = g_strdup_printf ("%s configuration", provider->label);
    dialog = gtk_dialog_new_with_buttons (text,
                                          nullptr,
                                          flags,
                                          "_OK",
                                          GTK_RESPONSE_ACCEPT,
                                          "_Cancel",
                                          GTK_RESPONSE_REJECT,
                                          nullptr);
    g_free (text);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    api_entry = gtk_entry_new ();

    gtk_entry_set_text (GTK_ENTRY (api_entry),
                        *api_key != nullptr ? *api_key : "");

    /* Packing dialog widgets */
    text = g_strdup_printf ("%s API key:", provider->label);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new (text),
                        FALSE, FALSE, 4);
    g_free (text);
    gtk_box_pack_start (GTK_BOX (content_box),
                        api_entry,
                        FALSE, FALSE, 4);

    if (provider->api_key_info != nullptr) {
        info_label = gtk_label_new (provider->api_key_info);
        gtk_label_set_line_wrap (GTK_LABEL (info_label), TRUE);
        gtk_label_set_max_width_chars (GTK_LABEL (info_label), 60);

        gtk_box_pack_start (GTK_BOX (content_box),
                            gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                            FALSE, FALSE, 4);
        gtk_box_pack_start (GTK_BOX (content_box),
                            info_label,
                            FALSE, FALSE, 4);
    }
    if (provider->api_key_url != nullptr) {
        gtk_box_pack_start (GTK_BOX (content_box),
                            gtk_link_button_new (provider->api_key_url),
                            FALSE, FALSE, 4);
    }
    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        free (*api_key);
        *api_key = strdup (
                gtk_entry_get_text (GTK_ENTRY (api_entry)));
    }
    gtk_widget_destroy (dialog);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create the widget of a search option.
 *
 * @param[in] option  Search option
 * @return    Spin button or combo box with the option's default value
 */
static GtkWidget *
search_opt_widget_create (const WebOption *option)
{
    GtkWidget *opt_widget = nullptr;

    if (option->type == WEB_OPT_INT) {
        opt_widget = gtk_spin_button_new_with_range (0.0,
                                                     (double) option->max,
                                                     1.0);
    }
    else {
        opt_widget = gtk_combo_box_text_new ();
        for (int i = 0; option->choices[i].value != nullptr; ++i) {
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (opt_widget),
                                            option->choices[i].label);
        }
        gtk_combo_box_set_active (GTK_COMBO_BOX (opt_widget), 0);
    }
    if (option->tooltip != nullptr)
        gtk_widget_set_tooltip_text (opt_widget, option->tooltip);

    return opt_widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the widget of a search option to the saved value.
 *
 * @param[in]  option      Search option
 * @param[in]  saved       Setting with the saved value, can be null
 * @param[out] opt_widget  Widget of the option
 * @return     none
 */
static void
search_opt_widget_set (const WebOption *option,
                       const Setting   *saved,
                       GtkWidget       *opt_widget)
{
    const char *value = nullptr; /* Saved value of a choice option */

    if (saved == nullptr)
        return;

    if (option->type == WEB_OPT_INT) {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (opt_widget),
                                   (double) setting_get_int (saved));
    }
    else if ((value = setting_get_string (saved)) != nullptr) {
        for (int i = 0; option->choices[i].value != nullptr; ++i) {
            if (strcmp (option->choices[i].value, value) == 0) {
                gtk_combo_box_set_active (GTK_COMBO_BOX (opt_widget), i);
                break;
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the value of a search option from its widget.
 *
 * @param[in] option      Search option
 * @param[in] opt_widget  Widget of the option
 * @return    New Setting with the option value, null when it is not set
 */
static Setting *
search_opt_widget_get (const WebOption *option,
                       GtkWidget       *opt_widget)
{
    const char *value = nullptr; /* Value of a choice option */
    int         num   = 0;       /* Value of a number option or choice index */

    if (option->type == WEB_OPT_INT) {
        num = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (opt_widget));

        return num > 0 ? setting_new_int (option->key, num) : nullptr;
    }
    num = gtk_combo_box_get_active (GTK_COMBO_BOX (opt_widget));

    /* The combo box holds exactly the option's choices */
    if (num >= 0)
        value = option->choices[num].value;

    return str_is_empty (value) ? nullptr :
                                  setting_new_string (option->key, value);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search options dialog made from a service's list of options.
 *
 * @param[in] widget    WebWidget item
 * @param[in] provider  Service with a WebProvider::options list
 * @return    New string with search options, null if dialog was cancelled
 */
static char *
search_opts_generic_dialog (WebWidget         *widget,
                            const WebProvider *provider)
{
    GtkWidget  *dialog;         /* Search options dialog */
    GtkWidget  *content_box;    /* Dialog's box */
    GtkWidget  *grid;           /* Grid with option labels and widgets */
    GtkWidget  *label;          /* Option label */
    GtkWidget **opt_widgets = nullptr; /* Widgets of options */
    Setting    *settings    = nullptr; /* Settings read or to save */
    Setting    *saved       = nullptr; /* Saved options of the service */
    Setting    *opts        = nullptr; /* List of options that are set */
    Setting    *opt         = nullptr; /* Option that is set */
    char       *title       = nullptr; /* Dialog title */
    char       *result      = nullptr; /* Result string */
    int         count       = 0;       /* Number of options */
    int         read_err    = 0;       /* Settings read error output */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    while (provider->options[count].key != nullptr)
        ++count;

    if ((opt_widgets = calloc ((size_t) count + 1,
                               sizeof (GtkWidget *))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    title  = g_strdup_printf ("%s search options", provider->label);
    dialog = gtk_dialog_new_with_buttons (title,
                                          nullptr,
                                          flags,
                                          "_OK",
                                          GTK_RESPONSE_ACCEPT,
                                          "_Cancel",
                                          GTK_RESPONSE_REJECT,
                                          nullptr);
    g_free (title);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 8);

    settings = setts_read (widget->config_file, &read_err);
    saved    = setting_get_child (settings_find (settings,
                                                 provider->opts_name));

    for (int i = 0; i < count; ++i) {
        label = gtk_label_new (provider->options[i].label);
        gtk_widget_set_halign (label, GTK_ALIGN_START);

        opt_widgets[i] = search_opt_widget_create (&provider->options[i]);
        search_opt_widget_set (&provider->options[i],
                               settings_find (saved,
                                              provider->options[i].key),
                               opt_widgets[i]);

        gtk_grid_attach (GTK_GRID (grid), label,          0, i, 1, 1);
        gtk_grid_attach (GTK_GRID (grid), opt_widgets[i], 1, i, 1, 1);
    }
    settings_free_all (settings);

    gtk_box_pack_start (GTK_BOX (content_box), grid, FALSE, FALSE, 4);
    gtk_widget_show_all (content_box);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {

        for (int i = 0; i < count; ++i) {
            opt = search_opt_widget_get (&provider->options[i],
                                         opt_widgets[i]);
            if (opt != nullptr)
                opts = settings_append (opts, opt);
        }
        settings = setting_new_setting (provider->opts_name);
        setting_add_child (settings, opts);

        setts_check_update_file (widget->config_file, settings);
        result = search_opts_to_str (setting_get_child (settings));
        settings_free_all (settings);
    }
    gtk_widget_destroy (dialog);
    free (opt_widgets);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search options dialog of the active search service.
 */
char *
ww_search_opts_dialog (WebWidget *widget)
{
    const WebProvider *provider = ww_provider (widget->active_service);

    if (provider == nullptr)
        return nullptr;

    if (provider->opts_dialog != nullptr)
        return provider->opts_dialog (widget);

    if (provider->options != nullptr)
        return search_opts_generic_dialog (widget, provider);

    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the data of a found image from a service whose search response
 *         holds only image ids.
 *
 * @param[in]     provider  Searched service
 * @param[in,out] item      Found image with its id set
 * @return        true if the image data was received and it can be used
 */
static bool
search_run_get_detail (const WebProvider *provider,
                       SearchItem        *item)
{
    UrlData *data = nullptr; /* Image data response */
    char    *url  = nullptr; /* Image data request url */
    bool     res  = false;

    if (item->id == nullptr)
        return false;

    url  = provider->detail_url (item->id);
    data = urldata_get_data (url);
    free (url);

    /* An image that can not be read is left out, the rest of the results is
     * still worth showing */
    if (data->errbuf == nullptr && urldata_full (data))
        res = provider->parse_detail (data->buffer, item);

    urldata_free (data);

    /* Keep the window responsive between requests */
    while (gtk_events_pending ())
        gtk_main_iteration ();

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Receive an image found by a search: show it on the image list and
 *         add it to the query cache entry.
 *
 * @param[in]     item       Found image, the function takes its ownership
 * @param[in,out] user_data  SearchRun with the data of the search
 * @return        none
 */
static void
search_run_add_item (SearchItem *item,
                     void       *user_data)
{
    SearchRun *run = user_data;

    if (run->provider->detail_url != nullptr &&
        !search_run_get_detail (run->provider, item)) {

        search_item_free (item);
        return;
    }
    add_search_item_to_img_view (run->widget->img_view,
                                 item,
                                 run->widget->wallpaper_dir,
                                 run->provider->name,
                                 run->widget->thumb_quality);
    cache_query_append_item (run->cache, item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search the active service and show a page of results.
 */
bool
ww_search (WebWidget  *widget,
           const char *api_key)
{
    const WebProvider *provider = ww_provider (widget->active_service);
    SearchRun   run   = {widget, provider, nullptr};
    UrlData    *data  = nullptr; /* For search results */
    char       *url   = nullptr; /* Search request url */
    int         found = 0;       /* Number of found images */
    int         err   = ERR_OK;  /* Error output */
    bool        res   = false;

    if (provider == nullptr)
        return false;

    url = provider->search_url (widget->query,
                                widget->search_opts,
                                api_key != nullptr ? api_key : "",
                                widget->page);
    if (url == nullptr) {
        message_dialog_warning (nullptr,
                "This service does not provide more pages of results.\n"
                "Try a more specific search query.");
        return false;
    }
    data = urldata_get_data (url);
    free (url);

    if (data->errbuf != nullptr) {
        message_dialog_error (nullptr, data->errbuf);
    }
    else {
        run.cache = cache_query_new (provider->name,
                                     widget->query,
                                     widget->search_opts,
                                     widget->page);

        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                    GTK_ICON_VIEW (widget->img_view))));

        res = provider->parse (data->buffer, &found,
                               search_run_add_item, &run);
        if (res) {
            widget->found_cnt     = found;
            run.cache->found_cnt  = found;

            err = cache_query_save (run.cache);

            if (err != ERR_OK)
                message_dialog_error (nullptr, err_get_message (err));
        }
        else {
            message_dialog_error (nullptr,
                    "The search service sent a response that could not "
                    "be read.");
        }
        cache_query_free (run.cache);
    }
    urldata_free (data);

    return res;
}
/*----------------------------------------------------------------------------*/
