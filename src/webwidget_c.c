/**
 * @file  webwidget_c.c
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
 * @brief  Common functions for webwidgets.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "dlgsmsg.h"
#include "thumbs.h"
#include "urldata.h"
#include "imgs.h"
#include "fdfn.h"
#include "errs.h"
#include "chquery.h"
#include "strfun.h"
#include "setts.h"
#include "webwidget_c.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget name.
 */
const char *
ww_name (const int i_site)
{
    switch (i_site) {
        case WEB_WIDGET_PEXELS:
            return "Pexels";
        case WEB_WIDGET_PIXBAY:
            return "Pixbay";
        case WEB_WIDGET_WALLHAVEN:
            return "Wallhaven";
        case WEB_WIDGET_WALLABYSS:
            return "WallpaperAbyss";
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
            return "Flickr";
#endif
        default:
            break;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget options name.
 */
const char *
ww_opts (const int i_site)
{
    switch (i_site) {
        case WEB_WIDGET_PEXELS:
            return "Pexels_opts";
        case WEB_WIDGET_PIXBAY:
            return "Pixbay_opts";
        case WEB_WIDGET_WALLHAVEN:
            return "Wallhaven_opts";
        case WEB_WIDGET_WALLABYSS:
            return "WallpaperAbyss_opts";
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
            return "Flickr_opts";
#endif
        default:
            break;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget logo image id based on webwidget if.
 */
int
ww_logo_id (const int i_site)
{
    switch (i_site) {
        case WEB_WIDGET_PEXELS:
            return W_LOGO_PEXELS;
        case WEB_WIDGET_PIXBAY:
            return W_LOGO_PIXBAY;
        case WEB_WIDGET_WALLHAVEN:
            return W_LOGO_WALLHAVEN;
        case WEB_WIDGET_WALLABYSS:
            return W_LOGO_WALLABYSS;
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
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
 * @param[in] i_site  Site id
 * @return    FourStrings item with API names
 */
static FourStrings *
ww_get_api_key_names (const int i_site)
{
    FourStrings *fs_data = NULL;

    fs_data = fourstrings_new ();

    for (int i = 0; i < 4; ++i) {
        *fs_data->s_str[i] = strdup ("");
    }
    switch (i_site) {
        case WEB_WIDGET_PEXELS:
            str_append (&fs_data->s_str1, "Pexels api");
            break;
        case WEB_WIDGET_PIXBAY:
            str_append (&fs_data->s_str1, "Pixbay api");
            break;
        case WEB_WIDGET_WALLHAVEN:
            str_append (&fs_data->s_str1, "Wallhaven api");
            break;
        case WEB_WIDGET_WALLABYSS:
            str_append (&fs_data->s_str1, "Wallpaper Abyss api");
            break;
#ifdef HAVE_FLICKCURL
        case WEB_WIDGET_FLICKR:
            str_append (&fs_data->s_str1, "Flickr client key");
            str_append (&fs_data->s_str2, "Flickr client secret");
            str_append (&fs_data->s_str3, "Flickr access token");
            str_append (&fs_data->s_str4, "Flickr access token secret");
            break;
#endif
        default:
            break;
    }
    return fs_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get API key data of given service from settings.
 */
FourStrings *
ww_get_api_key_data (Setting  *st_settings,
                     const int i_site)
{
    FourStrings *fs_key_names = NULL;  /* API key names in settings */
    FourStrings *fs_data      = NULL;  /* Data to rerutn */
    Setting     *st_sett      = NULL;  /* For individual setting */
    const char  *s_api        = NULL;  /* For API key */

    fs_key_names = ww_get_api_key_names (i_site);
    fs_data      = fourstrings_new ();

    for (int i = 0; i < 4; ++i) {
        *fs_data->s_str[i] = strdup ("");
    }
//    fs_data->s_str1 = strdup ("");
//    fs_data->s_str2 = strdup ("");
//    fs_data->s_str3 = strdup ("");
//    fs_data->s_str4 = strdup ("");

    for (int i = 0; i < 4; ++i) {
        if (*fs_key_names->s_str[i][0] != '\0') {
            st_sett = settings_find (st_settings, *fs_key_names->s_str[i]);
            s_api   = st_sett == NULL ? "" : setting_get_string (st_sett);
            str_append (fs_data->s_str[i], s_api);
        }
    }
//    if (fs_key_names->s_str1[0] != '\0') {
//        st_sett = settings_find (st_settings, fs_key_names->s_str1);
//        s_api   = st_sett == NULL ? "" : setting_get_string (st_sett);
//        str_append (&fs_data->s_str1, s_api);
//    }
//    if (fs_key_names->s_str2[0] != '\0') {
//        st_sett = settings_find (st_settings, fs_key_names->s_str2);
//        s_api   = st_sett == NULL ? "" : setting_get_string (st_sett);
//        str_append (&fs_data->s_str2, s_api);
//    }
//    if (fs_key_names->s_str3[0] != '\0') {
//        st_sett = settings_find (st_settings, fs_key_names->s_str3);
//        s_api   = st_sett == NULL ? "" : setting_get_string (st_sett);
//        str_append (&fs_data->s_str3, s_api);
//    }
//    if (fs_key_names->s_str4[0] != '\0') {
//        st_sett = settings_find (st_settings, fs_key_names->s_str4);
//        s_api   = st_sett == NULL ? "" : setting_get_string (st_sett);
//        str_append (&fs_data->s_str4, s_api);
//    }
    fourstrings_free (fs_key_names);

    return fs_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update API key data in settings.
 */
int
ww_update_api_key_data (const char  *s_cfg_file,
                        FourStrings *fs_key_data,
                        const int    i_site)
{
    Setting     *st_setts;
    FourStrings *fs_key_names;
    int          i_res = ERR_OK;

    fs_key_names = ww_get_api_key_names (i_site);

    st_setts = setting_new_string (fs_key_names->s_str1, fs_key_data->s_str1);

#ifdef HAVE_FLICKCURL
    if (i_site == WEB_WIDGET_FLICKR) {
//        for (int i = 1; i < 4; ++i) {
//            st_setts = settings_append (st_setts,
//                setting_new_string (*fs_key_names->s_str[i],
//                                    *fs_key_data->s_str[i]));
//        }
        st_setts = settings_append (st_setts,
            setting_new_string (fs_key_names->s_str2, fs_key_data->s_str2));
        st_setts = settings_append (st_setts,
            setting_new_string (fs_key_names->s_str3, fs_key_data->s_str3));
        st_setts = settings_append (st_setts,
            setting_new_string (fs_key_names->s_str4, fs_key_data->s_str4));
    }
#endif
    i_res = setts_check_update_file (s_cfg_file, st_setts);
    settings_free_all (st_setts);

    fourstrings_free (fs_key_names);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_txt is null or empty and show message s_msg
 *         if it is.
 */
int
str_is_empty_msg (const char *s_str,
                  const char *s_msg)
{
    if (str_is_empty (s_str)) {
        if (s_msg != NULL)
            message_dialog_warning (NULL, s_msg);
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_txt is made of alphanumeric chars and spaces
 *         and show message s_msg if it is not.
 */
int
str_is_alnum_or_space (const char *s_str,
                       const char *s_msg)
{
    gunichar u_c; /* Uni char to examine */

    while (*s_str != '\0') {
        u_c = g_utf8_get_char (s_str);
        if (!g_unichar_isalnum (u_c) && !g_unichar_isspace (u_c)) {
            if (s_msg != NULL)
                message_dialog_warning (NULL, s_msg);
            return 0;
        }
        s_str = g_utf8_find_next_char (s_str, NULL);
    }
    return 1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_txt is made of unicode chars
 *         and show message s_msg if it is not.
 */
int
check_unicode (const char *s_str,
               const char *s_msg)
{
    if (!g_utf8_validate (s_str, -1, NULL)) {
        if (s_msg != NULL)
            message_dialog_warning (NULL, s_msg);
        return 0;
    }
    return 1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check from string s_txt all non alphanumeric chars and non spaces.
 */
void
remove_non_alpha_space (char *s_str)
{
    gunichar  u_c;    /* Uni char to examine */
    char     *s_next; /* Position of next char */

    while (*s_str != '\0') {
        u_c = g_utf8_get_char (s_str);
        if (!g_unichar_isalnum (u_c) && !g_unichar_isspace (u_c)) {
            s_next = g_utf8_find_next_char (s_str, NULL);
            memmove (s_str, s_next, strlen (s_next) + 1);
        }
        else {
            s_str = g_utf8_find_next_char (s_str, NULL);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string value from Combobox's active i_col column.
 */
char *
combo_get_active_str (GtkWidget *gw_combo,
                      const int  i_col)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    char *s_ret = NULL;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, i_col, &s_ret, -1);
    }
    return s_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if given wallpaper is already in wallpaper dir and mark gp_pbuf
 *         with check mark if it is.
 *
 * @param[in]  s_wallp_dir  Wallpaper directory
 * @param[in]  s_file_name  Wallpaper file to check
 * @param[out] gp_pbuf      Thumbnail pixbuf
 * @retutn     none
 */
static void
wallpaper_check_mark (const char *s_wallp_dir,
                      const char *s_file_name,
                      GdkPixbuf  *gp_pbuf)
{
    GdkPixbuf *gp_check = NULL; /* Pixbuf for check image */
    char      *s_wlfn   = NULL; /* Wallpaper file path */

    s_wlfn = str_comb (s_wallp_dir, "/");
    str_append (&s_wlfn, s_file_name);

    if (file_check_permissions (s_wlfn) == ERR_OK) {

        gp_check = get_image (W_IMG_CHECK);
        gdk_pixbuf_composite (gp_check, gp_pbuf,
                              0, 0,
                              gdk_pixbuf_get_width (gp_check),
                              gdk_pixbuf_get_height (gp_check),
                              0, 0,
                              1.0, 1.0,
                              GDK_INTERP_HYPER,
                              255);
        g_object_unref (gp_check);
    }
    free (s_wlfn);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add image based on si_item data to icon view.
 */
void
add_searchitem_to_img_view (GtkWidget        *gw_iconview,
                            const SearchItem *si_item,
                            const char       *s_wallp_dir,
                            const char       *s_service_name,
                            const int         i_thumb_quality)
{
    GtkListStore *list_store;
    GdkPixbuf    *gp_pbuf  = NULL; /* Pixbuf with thumbnail */
    GtkTreeIter   iter;

    list_store = GTK_LIST_STORE (gtk_icon_view_get_model (
                GTK_ICON_VIEW (gw_iconview)));

    if (si_item->s_thumb_url == NULL)
        return;

    gp_pbuf = thumbnail_get (s_service_name,
                             si_item->s_id,
                             i_thumb_quality,
                             si_item->s_thumb_url);

    if (gp_pbuf != NULL) {

        wallpaper_check_mark (s_wallp_dir, si_item->s_file_name, gp_pbuf);

        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           WEB_COL_PIXBUF,    gp_pbuf,
                           WEB_COL_WIDTH,     si_item->i_width,
                           WEB_COL_HEIGHT,    si_item->i_height,
                           WEB_COL_ID,        si_item->s_id,
                           WEB_COL_DISP_NAME, si_item->s_display_name,
                           WEB_COL_AUTHOR,    si_item->s_author_name,
                           WEB_COL_MARKUP,    si_item->s_display_markup,
                           WEB_COL_FILE_NAME, si_item->s_file_name,
                           WEB_COL_PAGE_URL,  si_item->s_page_url,
                           WEB_COL_IMAGE_URL, si_item->s_image_url,
                           WEB_COL_THUMB_URL, si_item->s_thumb_url,
                           -1);
        g_object_unref (gp_pbuf);
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
check_for_cached_query (WebWidget  *ww_widget,
                        const char *s_service_name)
{
    CacheQuery *cq_query_chk = NULL; /* For checking cache availability */
    int         i_err        = 0;    /* Error output */
    int         i            = 0;    /* i */

    cq_query_chk = cachequery_check_query (s_service_name,
                                           ww_widget->s_query,
                                           ww_widget->s_search_opts,
                                           ww_widget->i_page,
                                           &i_err);
    if (i_err != ERR_OK) {
        message_dialog_error (NULL, err_get_message (i_err));
    }
    if (cq_query_chk != NULL) {
        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (ww_widget->gw_img_view))));

        ww_widget->i_found_cnt = cq_query_chk->i_found_cnt;

        for (i = 0; i < cq_query_chk->i_sicnt; ++i) {
            add_searchitem_to_img_view (ww_widget->gw_img_view,
                                        cq_query_chk->si_items[i],
                                        ww_widget->s_wallp_dir,
                                        s_service_name,
                                        ww_widget->i_thumb_quality);
        }
        cachequery_free (cq_query_chk);
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Converts image search options in Setting format to string for url.
 */
char *
search_opts_to_str (const Setting *st_setts)
{
    char          *s_res   = NULL;
    const Setting *st_item = NULL;
    char           s_buff[32];

    s_res   = strdup ("");
    st_item = st_setts;

    while (st_item != NULL) {
        str_append (&s_res, "&");
        str_append (&s_res, setting_get_name (st_item));
        str_append (&s_res, "=");
        if (setting_get_type (st_item) == SET_VAL_INT) {
            sprintf (s_buff, "%" PRId64, setting_get_int (st_item));
            str_append (&s_res, s_buff);
        }
        else if (setting_get_type (st_item) == SET_VAL_STRING) {
            str_append (&s_res, setting_get_string (st_item));
        }
        st_item = st_item->next;
    }
    return s_res;
}
/*----------------------------------------------------------------------------*/

