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
#include "message_dialogs.h"
#include "thumbnail.h"
#include "url_data.h"
#include "icons.h"
#include "file_dir_functions.h"
#include "errors.h"
#include "cache_query.h"
#include "string_functions.h"
#include "settings.h"
#include "webwidget_common.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget name.
 */
const char *
ww_name (const int site)
{
    switch (site) {
        case WEB_SERV_PIXABAY:
            return "Pixabay";
        case WEB_SERV_WALLHAVEN:
            return "Wallhaven";
        case WEB_SERV_WALLABYSS:
            return "WallpaperAbyss";
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            return "Flickr";
#endif
        default:
            break;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget options name.
 */
const char *
ww_opts (const int site)
{
    switch (site) {
        case WEB_SERV_PIXABAY:
            return "Pixabay_opts";
        case WEB_SERV_WALLHAVEN:
            return "Wallhaven_opts";
        case WEB_SERV_WALLABYSS:
            return "WallpaperAbyss_opts";
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            return "Flickr_opts";
#endif
        default:
            break;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget logo image id based on webwidget if.
 */
int
ww_logo_id (const int site)
{
    switch (site) {
        case WEB_SERV_PIXABAY:
            return W_LOGO_PIXABAY;
        case WEB_SERV_WALLHAVEN:
            return W_LOGO_WALLHAVEN;
        case WEB_SERV_WALLABYSS:
            return W_LOGO_WALLABYSS;
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            return W_LOGO_FLICKR;
#endif
        default:
            break;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings API names.
 *
 * @param[in] site  Site id
 * @return    NStrings item with API names
 */
static NStrings *
ww_get_api_key_names (const int site)
{
    NStrings *nstrings = nullptr;

    nstrings = nstrings_new (4, NS_VAL_EMPTY);

    switch (site) {
        case WEB_SERV_PIXABAY:
            str_append (&nstrings->str[0], "Pixabay api");
            break;
        case WEB_SERV_WALLHAVEN:
            str_append (&nstrings->str[0], "Wallhaven api");
            break;
        case WEB_SERV_WALLABYSS:
            str_append (&nstrings->str[0], "Wallpaper Abyss api");
            break;
#ifdef HAVE_FLICKCURL
        case WEB_SERV_FLICKR:
            str_append (&nstrings->str[0], "Flickr client key");
            str_append (&nstrings->str[1], "Flickr client secret");
            str_append (&nstrings->str[2], "Flickr access token");
            str_append (&nstrings->str[3], "Flickr access token secret");
            break;
#endif
        default:
            break;
    }
    return nstrings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get API key data of given service from settings.
 */
NStrings *
ww_get_api_key_data (Setting  *settings,
                     const int site)
{
    NStrings   *key_names = nullptr;  /* API key names in settings */
    NStrings   *nstrings  = nullptr;  /* Data to rerutn */
    Setting    *setting   = nullptr;  /* For individual setting */
    const char *api       = nullptr;  /* For API key */

    key_names = ww_get_api_key_names (site);
    nstrings      = nstrings_new (4, NS_VAL_EMPTY);

    for (int i = 0; i < 4; ++i) {
        if (key_names->str[i][0] != '\0') {
            setting = settings_find (settings, key_names->str[i]);
            api   = setting == nullptr ? "" : setting_get_string (setting);
            str_append (&nstrings->str[i], api);
        }
    }
    nstrings_free (key_names);

    return nstrings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update API key data in settings.
 */
int
ww_update_api_key_data (const char *cfg_file,
                        NStrings   *key_data,
                        const int   site)
{
    Setting  *settings;     /* Setting list for key data */
    NStrings *key_names;    /* Key names for the site */
    int       res = ERR_OK; /* Error output */

    /* Get api key names for a given wallpaper site */
    key_names = ww_get_api_key_names (site);
    /* Create Setting list with key name and key value */
    settings = setting_new_string (key_names->str[0],
                                   key_data->str[0]);
#ifdef HAVE_FLICKCURL
    if (site == WEB_SERV_FLICKR) {
        settings = settings_append (settings,
            setting_new_string (key_names->str[1], key_data->str[1]));
        settings = settings_append (settings,
            setting_new_string (key_names->str[2], key_data->str[2]));
        settings = settings_append (settings,
            setting_new_string (key_names->str[3], key_data->str[3]));
    }
#endif
    /* Update config file with new data */
    res = setts_check_update_file (cfg_file, settings);

    settings_free_all (settings);
    nstrings_free (key_names);

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
    int         err          = 0;       /* Error output */
    int         i            = 0;       /* i */

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
