/**
 * @file  live_bing.c
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
 * @brief  Bing image of the day as a remote live wallpaper.
 *
 * The HPImageArchive endpoint is unofficial and undocumented. Its images are
 * copyrighted, but the ones marked with the "wp" flag may be used as
 * wallpapers.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include "util/string_functions.h"
#include "web/web_util.h"
#include "live_bing.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   BING_HOST
 * @brief Address the image paths in the response are relative to
 *
 * @def   BING_ID_PREFIX
 * @brief Start of the "urlbase" value, the image id follows it
 */
#define BING_HOST      "https://www.bing.com"
#define BING_ID_PREFIX "/th?id="
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the size suffix of the Bing image that fits the screen best.
 *
 * Bing serves an image of the day only in a few sizes: 1366x768, 1920x1080
 * and UHD (3840x2160 or larger). Other sizes, like 2560x1440, do not exist.
 */
const char *
live_bing_size (const int screen_width,
                const int screen_height)
{
    if (screen_width <= 1366 && screen_height <= 768)
        return "1366x768";

    if (screen_width <= 1920 && screen_height <= 1080)
        return "1920x1080";

    return "UHD";
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Bing request url.
 *
 * Eight days are requested, so an image that may be used as a wallpaper can
 * be found when the present one may not.
 *
 * @param[in] params  Wallpaper parameters, not used
 * @return    New string with url, free it with free()
 */
static char *
bing_request_url ([[maybe_unused]] const LiveParams *params)
{
    return str_comb (BING_HOST,
                     "/HPImageArchive.aspx?format=js&idx=0&n=8");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set image title and author from the Bing copyright text.
 *
 * The text looks like "Abraham Lake in Alberta, Canada (© Guang Cao/Getty
 * Images)". The "title" member of the response is often only "Info".
 *
 * @param[in]     copyright  Copyright text, can be null
 * @param[in,out] image      Image to set
 * @return        none
 */
static void
bing_set_title_author (const char *copyright,
                       LiveImage  *image)
{
    const char *start = nullptr; /* Start of the "(©" part */
    const char *end   = nullptr; /* Closing bracket */
    char       *text  = nullptr; /* Copy of a part of the copyright text */

    if (copyright == nullptr)
        return;

    if ((start = strstr (copyright, " (©")) == nullptr) {
        live_image_set (&image->title, copyright);
        return;
    }
    text = strndup (copyright, (size_t) (start - copyright));
    live_image_set (&image->title, text);
    free (text);

    start += strlen (" (©");
    while (*start == ' ')
        ++start;

    if ((end = strrchr (start, ')')) == nullptr)
        end = start + strlen (start);

    text = strndup (start, (size_t) (end - start));
    live_image_set (&image->author, text);
    free (text);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check the "wp" flag of a Bing image.
 *
 * @param[in] obj  Json object with image data
 * @return    true if the image may be used as a wallpaper
 */
static bool
bing_is_wallpaper (json_object *obj)
{
    json_object *val = nullptr;

    return json_object_object_get_ex (obj, "wp", &val) &&
           json_object_get_type (val) == json_type_boolean &&
           json_object_get_boolean (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Bing response.
 *
 * @param[in]  json    String with json data
 * @param[in]  params  Wallpaper parameters
 * @param[out] image   Chosen image
 * @return     false when there is no image that may be used as a wallpaper
 */
static bool
bing_parse (const char       *json,
            const LiveParams *params,
            LiveImage        *image)
{
    json_object *obj     = web_json_parse (json);
    json_object *arr     = web_json_get (obj, "images", json_type_array);
    json_object *item    = nullptr; /* Image data */
    const char  *urlbase = nullptr; /* Image path without size */
    const char  *size    = nullptr; /* Image size suffix */
    char        *id      = nullptr; /* Image id with size */
    char        *url     = nullptr; /* Image url */
    bool         found   = false;

    size = live_bing_size (params->screen_width, params->screen_height);

    for (size_t i = 0; arr != nullptr && i < json_object_array_length (arr);
         ++i) {
        item    = web_json_idx (arr, i);
        urlbase = web_json_str (item, "urlbase");

        if (!bing_is_wallpaper (item) || urlbase == nullptr ||
            strncmp (urlbase, BING_ID_PREFIX, strlen (BING_ID_PREFIX)) != 0 ||
            urlbase[strlen (BING_ID_PREFIX)] == '\0') {
            continue;
        }
        url = str_comb (BING_HOST, urlbase);
        str_append (&url, "_");
        str_append (&url, size);
        str_append (&url, ".jpg");
        live_image_set (&image->image_url, url);
        free (url);

        id = str_comb (urlbase + strlen (BING_ID_PREFIX), "_");
        str_append (&id, size);
        image->file_name = live_file_name (live_bing.name, id, ".jpg");
        free (id);

        bing_set_title_author (web_json_str (item, "copyright"), image);
        live_image_set (&image->page_url, web_json_str (item, "copyrightlink"));
        live_image_set (&image->license,  "Use as wallpaper only");
        found = true;
        break;
    }
    if (obj != nullptr)
        json_object_put (obj);

    return found;
}
/*----------------------------------------------------------------------------*/
const LiveSource live_bing = {
    .name             = "bing",
    .label            = "Bing image of the day",
    .info             = "Photograph of the day from the Bing search page.\n"
                        "A new image once a day, already in the cache "
                        "until then.\nImages may be used as wallpapers only.",
    .daily            = true,
    .keywords_setting = nullptr,
    .request_url      = bing_request_url,
    .parse            = bing_parse,
};
/*----------------------------------------------------------------------------*/
