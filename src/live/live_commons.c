/**
 * @file  live_commons.c
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
 * @brief  Wikimedia Commons picture of the day as a remote live wallpaper.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/string_functions.h"
#include "web/web_util.h"
#include "live_commons.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the width of the image to request.
 *
 * The larger screen dimension is used, so an image also fills a screen in
 * portrait orientation.
 *
 * @param[in] params  Wallpaper parameters
 * @return    Image width
 */
static int
commons_width (const LiveParams *params)
{
    return params->screen_width > params->screen_height ?
           params->screen_width : params->screen_height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Wikimedia Commons request url.
 *
 * The picture of the day is the image used on the "Template:Potd/<date>"
 * page. The server scales it to the requested width.
 *
 * @param[in] params  Wallpaper parameters
 * @return    New string with url, free it with free()
 */
static char *
commons_request_url (const LiveParams *params)
{
    char *url = nullptr; /* Result url */
    char  width[16];     /* Requested image width */

    snprintf (width, sizeof (width), "%d", commons_width (params));

    url = str_comb ("https://commons.wikimedia.org/w/api.php?action=query"
                    "&format=json&formatversion=2&generator=images"
                    "&titles=Template:Potd/", params->date);
    str_append (&url, "&prop=imageinfo"
                      "&iiprop=url%7Csize%7Cmime%7Cextmetadata"
                      "&iiextmetadatafilter=Artist%7CLicenseShortName"
                      "%7CLicenseUrl%7CObjectName&iiurlwidth=");
    str_append (&url, width);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a value from the image's extended metadata.
 *
 * @param[in] info  Json object with image info
 * @param[in] key   Metadata name
 * @return    New string with the value without html, or null; free it
 *            with free()
 */
static char *
commons_meta (json_object *info,
              const char  *key)
{
    json_object *meta = web_json_get (info, "extmetadata", json_type_object);

    return web_strip_html (
            web_json_str (web_json_get (meta, key, json_type_object),
                          "value"));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make a title from the file page title.
 *
 * "File:Eastern quoll.jpg" becomes "Eastern quoll".
 *
 * @param[in] page_title  Page title, can be null
 * @return    New string or null, free it with free()
 */
static char *
commons_title_from_page (const char *page_title)
{
    char *title = nullptr;
    char *dot   = nullptr;

    if (page_title == nullptr)
        return nullptr;

    if (strncmp (page_title, "File:", 5) == 0)
        page_title += 5;

    title = str_comb (page_title, "");

    if ((dot = strrchr (title, '.')) != nullptr)
        *dot = '\0';

    return title;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Wikimedia Commons response.
 *
 * The first image in jpeg, png or webp format is chosen. On some days the
 * picture of the day is a video or an svg drawing, then there is no usable
 * image.
 *
 * @param[in]  json    String with json data
 * @param[in]  params  Wallpaper parameters
 * @param[out] image   Chosen image
 * @return     false when the response holds no usable image
 */
static bool
commons_parse (const char       *json,
               const LiveParams *params,
               LiveImage        *image)
{
    json_object *obj   = web_json_parse (json);
    json_object *pages = nullptr; /* Found pages */
    json_object *page  = nullptr; /* Page with image */
    json_object *info  = nullptr; /* Image info */
    const char  *ext   = nullptr; /* Image file extension */
    const char  *url   = nullptr; /* Image url */
    char        *text  = nullptr; /* Text for image data */
    char         id[64];          /* Date and width as image id */
    bool         found = false;

    pages = web_json_get (web_json_get (obj, "query", json_type_object),
                          "pages", json_type_array);

    for (size_t i = 0; pages != nullptr &&
                       i < json_object_array_length (pages); ++i) {
        page = web_json_idx (pages, i);
        info = web_json_idx (web_json_get (page, "imageinfo",
                                           json_type_array), 0);
        ext  = web_ext_from_mime (web_json_str (info, "mime"));
        url  = web_json_str (info, "thumburl");

        if (url == nullptr)
            url = web_json_str (info, "url");

        if (ext == nullptr || url == nullptr)
            continue;

        live_image_set (&image->image_url, url);
        live_image_set (&image->page_url,  web_json_str (info,
                                                         "descriptionurl"));
        snprintf (id, sizeof (id), "%s_%d",
                  params->date, commons_width (params));
        image->file_name = live_file_name (live_commons.name, id, ext);

        text = commons_meta (info, "ObjectName");
        if (text == nullptr)
            text = commons_title_from_page (web_json_str (page, "title"));
        live_image_set (&image->title, text);
        free (text);

        text = commons_meta (info, "Artist");
        live_image_set (&image->author, text);
        free (text);

        text = commons_meta (info, "LicenseShortName");
        live_image_set (&image->license, text);
        free (text);

        text = commons_meta (info, "LicenseUrl");
        live_image_set (&image->license_url, text);
        free (text);

        found = true;
        break;
    }
    if (obj != nullptr)
        json_object_put (obj);

    return found;
}
/*----------------------------------------------------------------------------*/
const LiveSource live_commons = {
    .name             = "commons",
    .label            = "Wikimedia Commons picture of the day",
    .info             = "Featured picture of the day from Wikimedia Commons, "
                        "scaled to the screen size.\nA new image once a day, "
                        "already in the cache until then.\nImages are under "
                        "free licenses, the author and license are shown "
                        "with the preview.",
    .daily            = true,
    .keywords_setting = nullptr,
    .request_url      = commons_request_url,
    .parse            = commons_parse,
};
/*----------------------------------------------------------------------------*/
