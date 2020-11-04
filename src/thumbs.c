/**
 * @file  thumbs.c
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
 * @brief  Thumbnail getting
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include "urldata.h"
#include "strfun.h"
#include "fdfn.h"
#include "cfgfile.h"
#include "dlgsmsg.h"
#include "thumbs.h"
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
 * @brief  Get thumbnail image from cached data or from web.
 */
GdkPixbuf *
thumbnail_get (const char *s_service_name,
               const char *s_id,
               const int   i_jpgq,
               const char *s_thumb_url)
{
    GdkPixbuf *gp_pbuf      = NULL;
    GError    *g_error      = NULL; /* For error output */
    char      *s_prefix     = NULL;
    char      *s_thumb_file = NULL;
    char       s_jpgq[16];

    sprintf (s_jpgq, "%d", i_jpgq);

    s_thumb_file = cfgfile_get_app_thumbnails_path ();
    s_prefix     = strdup (s_service_name);

    dir_create_with_subdirs (s_thumb_file);
    if (isupper (s_prefix[0]))
        s_prefix[0] = tolower (s_prefix[0]);

    str_append (&s_thumb_file, "/");
    str_append (&s_thumb_file, s_prefix);
    str_append (&s_thumb_file, "_");
    str_append (&s_thumb_file, s_id);
    str_append (&s_thumb_file, ".jpg");
    free (s_prefix);

    gp_pbuf = gdk_pixbuf_new_from_file (s_thumb_file, &g_error);

    if (gp_pbuf == NULL) {
        gp_pbuf = pixbuf_from_url (s_thumb_url);
        if (gp_pbuf != NULL) {
            g_error = NULL;
            gdk_pixbuf_save (gp_pbuf, s_thumb_file, "jpeg", &g_error,
                             "quality", s_jpgq, NULL);
        }
    }
    free (s_thumb_file);
    return gp_pbuf;
}
/*----------------------------------------------------------------------------*/

