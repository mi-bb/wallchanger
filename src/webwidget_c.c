/**
 * @file  webwidget_common.c
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
#include "urldata.h"
#include "imgs.h"
#include "fdfn.h"
#include "errs.h"
#include "chquery.h"
#include "strfun.h"
#include "webwidget_c.h"
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
 * @brief  Create pixbuf from raw data.
 *
 * @param[in] s_data  Buffer with image data
 * @param[in] i_size  Buffer size
 * @return    Pixbuf with image
 */
static GdkPixbuf *
pixbuf_from_data (const unsigned char *s_data,
                  const gssize         i_size)
{
    GdkPixbuf    *g_pbuf = NULL; /* Return pixbuf */
    GInputStream *stream;        /* Stream for passing data to pixbuf */

    stream = g_memory_input_stream_new ();
    g_memory_input_stream_add_data (G_MEMORY_INPUT_STREAM (stream),
                                    s_data, i_size, NULL);
    g_pbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
    g_object_unref (stream);

    return g_pbuf;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf from raw data
 *
 * @param[in] s_url  Url where should be image for pixbuf
 * @return    Pixbuf with image
 */
static GdkPixbuf *
pixbuf_from_url (const char *s_url)
{
    GdkPixbuf *g_pbuf  = NULL; /* Pixbuf to return */
    UrlData   *ud_data = NULL; /* Urldata for getting image data */
    char      *s_txt   = NULL; /* For error text */

    ud_data = urldata_get_data (s_url);

    if (ud_data->errbuf != NULL) {
        s_txt = str_comb ("Getting image error:\n",
                          ud_data->errbuf);
        message_dialog_error (NULL, s_txt);
        free (s_txt);
    }
    else if (urldata_full (ud_data)) {
        g_pbuf = pixbuf_from_data ((unsigned char *) ud_data->buffer,
                                   (gssize) ud_data->size);
    }
    urldata_free (ud_data);

    return g_pbuf;
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
 * @brief  Add image based on si_item data to icon view.
 */
void
add_searchitem_to_img_view (GtkWidget        *gw_iconview,
                            const SearchItem *si_item,
                            const char       *s_thumb_dir,
                            const char       *s_wallp_dir,
                            const char       *s_service_prefix)
{
    GtkListStore *list_store;
    GdkPixbuf    *gp_pbuf  = NULL; /* Pixbuf with thumbnail */
    GdkPixbuf    *gp_check = NULL; /* Pixbuf for check image */
    GError       *g_error  = NULL; /* For error output */
    char         *s_thfn   = NULL; /* Thumbnail file path */
    char         *s_wlfn   = NULL; /* Wallpaper file path */
    GtkTreeIter   iter;

    list_store = GTK_LIST_STORE (gtk_icon_view_get_model (
                GTK_ICON_VIEW (gw_iconview)));

    if (si_item->s_thumb_url == NULL)
        return;

    s_thfn = str_comb (s_thumb_dir, "/");
    str_append (&s_thfn, s_service_prefix);
    str_append (&s_thfn, si_item->s_id);
    str_append (&s_thfn, ".jpg");

    gp_pbuf = gdk_pixbuf_new_from_file (s_thfn, &g_error);

    if (gp_pbuf == NULL) {
        gp_pbuf = pixbuf_from_url (si_item->s_thumb_url);
        if (gp_pbuf != NULL) {
            g_error = NULL;
            gdk_pixbuf_save (gp_pbuf, s_thfn, "jpeg", &g_error,
                             "quality", "96", NULL);
        }
    }
    free (s_thfn);

    if (gp_pbuf != NULL) {

        s_wlfn = str_comb (s_wallp_dir, "/");
        str_append (&s_wlfn, si_item->s_file_name);

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
        free (s_wlfn);
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
                        const char *s_service_name,
                        const char *s_service_prefix)
{
    CacheQuery *cq_query_chk = NULL; /* For checking cache availability */
    int         i_err        = 0;    /* Error output */
    int         i            = 0;    /* i */

    cq_query_chk = cachequery_check_query (s_service_name,
                                           ww_widget->s_query,
                                           ww_widget->i_page,
                                           ww_widget->i_per_page,
                                           &i_err);
    if (i_err != ERR_OK) {
        message_dialog_error (NULL, err_get_message (i_err));
    }
    if (cq_query_chk != NULL) {
        gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                        GTK_ICON_VIEW (ww_widget->gw_img_view))));

        ww_widget->i_found_cnt = cq_query_chk->i_found_cnt;

        for (i = 0; i < cq_query_chk->i_per_page; ++i) {
            add_searchitem_to_img_view (ww_widget->gw_img_view,
                                        cq_query_chk->si_items[i],
                                        ww_widget->s_thumb_dir,
                                        ww_widget->s_wallp_dir,
                                        s_service_prefix);
        }
        cachequery_free (cq_query_chk);
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/

