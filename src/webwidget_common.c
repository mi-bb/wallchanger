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
#include "strfun.h"
#include "webwidget_common.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_txt is null or empty and show message s_msg
 *         if it is.
 */
int
check_empty (const char *s_txt,
             const char *s_msg)
{
    if (s_txt == NULL || s_txt[0] == '\0') {
        message_dialog_warning (NULL, s_msg);
        return 1;
    }
    return 0;
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
                            const SearchItem *si_item)
{
    GtkListStore *list_store;
    GdkPixbuf    *gp_pbuf;
    GtkTreeIter   iter;

    list_store = GTK_LIST_STORE (gtk_icon_view_get_model (
                GTK_ICON_VIEW (gw_iconview)));

    if (si_item->s_thumb_url == NULL)
        return;
    if ((gp_pbuf = pixbuf_from_url (si_item->s_thumb_url)) != NULL) {
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           WEB_COL_PIXBUF,    gp_pbuf,
                           WEB_COL_WIDTH,     si_item->i_width,
                           WEB_COL_HEIGHT,    si_item->i_height,
                           WEB_COL_ID,        si_item->i_id,
                           WEB_COL_DISP_NAME, si_item->s_display_name,
                           WEB_COL_MARKUP,    si_item->s_display_markup,
                           WEB_COL_FILE_NAME, si_item->s_file_name,
                           WEB_COL_PAGE_URL,  si_item->s_page_url,
                           WEB_COL_IMAGE_URL, si_item->s_image_url,
                           WEB_COL_THUMB_URL, si_item->s_thumb_url,
                           -1);
        g_object_unref(gp_pbuf);
        gp_pbuf = NULL;
    }
    while (gtk_events_pending ())
        gtk_main_iteration ();
}
/*----------------------------------------------------------------------------*/
