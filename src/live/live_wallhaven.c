/**
 * @file  live_wallhaven.c
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
 * @brief  Random Wallhaven wallpaper as a remote live wallpaper.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include "util/string_functions.h"
#include "web/url_data.h"
#include "web/web_util.h"
#include "live_wallhaven.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Wallhaven request url.
 *
 * Asks for a random page of safe for work images at least as large as the
 * screen and with the same orientation. No API key is sent, the key only
 * gives access to images that are not safe for work.
 *
 * @param[in] params  Wallpaper parameters
 * @return    New string with url, free it with free()
 */
static char *
wallhaven_request_url (const LiveParams *params)
{
    char *url     = nullptr; /* Result url */
    char *escaped = nullptr; /* Percent-encoded keywords */
    char  size[32];          /* Minimum resolution */

    snprintf (size, sizeof (size), "%dx%d",
              params->screen_width, params->screen_height);

    url = str_comb ("https://wallhaven.cc/api/v1/search?sorting=random"
                    "&purity=100&categories=111&atleast=", size);
    str_append (&url, params->screen_height > params->screen_width ?
                      "&ratios=portrait" : "&ratios=landscape");

    if (!str_is_empty (params->keywords)) {
        escaped = urldata_escape (params->keywords);
        str_append (&url, "&q=");
        str_append (&url, escaped);
        free (escaped);
    }
    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Wallhaven response.
 *
 * Of the random images in the response the one whose aspect ratio is the
 * closest to the screen's is chosen, so the least of it gets cropped.
 *
 * @param[in]  json    String with json data
 * @param[in]  params  Wallpaper parameters
 * @param[out] image   Chosen image
 * @return     false when the response holds no usable image
 */
static bool
wallhaven_parse (const char       *json,
                 const LiveParams *params,
                 LiveImage        *image)
{
    json_object *obj    = web_json_parse (json);
    json_object *arr    = web_json_get (obj, "data", json_type_array);
    json_object *item   = nullptr; /* Image data */
    json_object *best   = nullptr; /* Image with the closest ratio */
    const char  *ext    = nullptr; /* Image file extension */
    const char  *best_ext = nullptr;
    double       screen = 0;       /* Screen aspect ratio */
    double       diff   = 0;       /* Ratio difference */
    double       best_diff = 0;
    int          width  = 0;
    int          height = 0;
    char        *title  = nullptr;

    screen = (double) params->screen_width / (double) params->screen_height;

    for (size_t i = 0; arr != nullptr && i < json_object_array_length (arr);
         ++i) {
        item   = web_json_idx (arr, i);
        width  = web_json_int (item, "dimension_x");
        height = web_json_int (item, "dimension_y");
        ext    = web_ext_from_url (web_json_str (item, "path"));

        if (ext == nullptr)
            ext = web_ext_from_mime (web_json_str (item, "file_type"));

        if (web_json_str (item, "id") == nullptr || ext == nullptr ||
            width <= 0 || height <= 0) {
            continue;
        }
        diff = (double) width / (double) height - screen;
        if (diff < 0)
            diff = -diff;

        if (best == nullptr || diff < best_diff) {
            best      = item;
            best_ext  = ext;
            best_diff = diff;
        }
    }
    if (best != nullptr) {
        live_image_set (&image->image_url, web_json_str (best, "path"));
        live_image_set (&image->page_url,  web_json_str (best, "url"));
        live_image_set (&image->license,   "Not specified");
        image->file_name = live_file_name (live_wallhaven.name,
                                           web_json_str (best, "id"),
                                           best_ext);
        title = str_comb ("Wallhaven ", web_json_str (best, "id"));
        live_image_set (&image->title, title);
        free (title);
    }
    if (obj != nullptr)
        json_object_put (obj);

    return best != nullptr;
}
/*----------------------------------------------------------------------------*/
const LiveSource live_wallhaven = {
    .name             = "wallhaven",
    .label            = "Wallhaven random wallpaper",
    .info             = "Random wallpaper from Wallhaven, at least as large "
                        "as the screen.\nA new image on every change. "
                        "Optional keywords narrow the choice,\n"
                        "e.g. <b>mountains</b> or <b>space nebula</b>.\n"
                        "Images are uploaded by users, without license "
                        "information.",
    .daily            = false,
    .keywords_setting = "Wallhaven live keywords",
    .request_url      = wallhaven_request_url,
    .parse            = wallhaven_parse,
};
/*----------------------------------------------------------------------------*/
