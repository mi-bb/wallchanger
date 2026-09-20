/**
 * @file  thumbnail.c
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
 * @brief  Thumbnail getting
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include "urldata.h"
#include "string_functions.h"
#include "file_dir_functions.h"
#include "cfg_file.h"
#include "message_dialogs.h"
#include "thumbnail.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf from raw data.
 *
 * @param[in] data  Buffer with image data
 * @param[in] size  Buffer size
 * @return    Pixbuf with image
 */
static GdkPixbuf *
pixbuf_from_data (const unsigned char *data,
                  const gssize         size)
{
    GdkPixbuf    *pbuf = nullptr; /* Return pixbuf */
    GInputStream *stream;        /* Stream for passing data to pixbuf */

    stream = g_memory_input_stream_new ();
    g_memory_input_stream_add_data (G_MEMORY_INPUT_STREAM (stream),
                                    data, size, nullptr);
    pbuf = gdk_pixbuf_new_from_stream (stream, nullptr, nullptr);
    g_object_unref (stream);

    return pbuf;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf from raw data
 *
 * @param[in] url  Url where should be image for pixbuf
 * @return    Pixbuf with image
 */
static GdkPixbuf *
pixbuf_from_url (const char *url)
{
    GdkPixbuf *pbuf     = nullptr; /* Pixbuf to return */
    UrlData   *url_data = nullptr; /* Urldata for getting image data */
    char      *txt      = nullptr; /* For error text */

    url_data = urldata_get_data (url);

    if (url_data->errbuf != nullptr) {
        txt = str_comb ("Getting image error:\n",
                          url_data->errbuf);
        message_dialog_error (nullptr, txt);
        free (txt);
    }
    else if (urldata_full (url_data)) {
        pbuf = pixbuf_from_data ((unsigned char *) url_data->buffer,
                                   (gssize) url_data->size);
    }
    urldata_free (url_data);

    return pbuf;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get thumbnail image from cached data or from web.
 */
GdkPixbuf *
thumbnail_get (const char *service_name,
               const char *image_id,
               const int   jpg_quality,
               const char *thumbnail_url)
{
    GdkPixbuf *pbuf       = nullptr;
    GError    *g_error    = nullptr; /* For error output */
    char      *prefix     = nullptr;
    char      *thumb_path = nullptr;
    char       jpg_q[16];

    sprintf (jpg_q, "%d", jpg_quality);

    thumb_path = cfg_file_get_app_thumbnails_path ();
    prefix     = strdup (service_name);

    dir_create_with_subdirs (thumb_path);
    if (isupper (prefix[0]))
        prefix[0] = (char) tolower (prefix[0]);

    str_append (&thumb_path, "/");
    str_append (&thumb_path, prefix);
    str_append (&thumb_path, "_");
    str_append (&thumb_path, image_id);
    str_append (&thumb_path, ".jpg");
    free (prefix);

    pbuf = gdk_pixbuf_new_from_file (thumb_path, &g_error);

    if (pbuf == nullptr) {
        pbuf = pixbuf_from_url (thumbnail_url);
        if (pbuf != nullptr) {
            g_error = nullptr;
            gdk_pixbuf_save (pbuf, thumb_path, "jpeg", &g_error,
                             "quality", jpg_q, nullptr);
        }
    }
    free (thumb_path);
    return pbuf;
}
/*----------------------------------------------------------------------------*/

