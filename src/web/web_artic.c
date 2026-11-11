/**
 * @file  web_artic.c
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
 * @brief  Art Institute of Chicago image search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "url_data.h"
#include "web_util.h"
#include "web_artic.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   ARTIC_IIIF_URL
 * @brief Address of the image server, used when the response does not give
 *        one.
 *
 * @def   ARTIC_WEBSITE_URL
 * @brief Address of the museum website, used when the response does not give
 *        one.
 *
 * @var   ARTIC_MAX_RESULTS
 * @brief Number of first search results the service gives access to.
 *
 * @var   ARTIC_MAX_WIDTH
 * @brief Width of the largest image the image server sends.
 *
 * @var   ARTIC_THUMB_WIDTH
 * @brief Width of the requested thumbnails.
 */
#define ARTIC_IIIF_URL    "https://www.artic.edu/iiif/2"
#define ARTIC_WEBSITE_URL "https://www.artic.edu"
constexpr int ARTIC_MAX_RESULTS = 1000;
constexpr int ARTIC_MAX_WIDTH   = 3000;
constexpr int ARTIC_THUMB_WIDTH = 200;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Art Institute of Chicago search request url.
 *
 * Only artworks in the public domain are searched; the museum shares their
 * images under CC0.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Not used, the service has no search options
 * @param[in] api_key      Not used, the service takes no key
 * @param[in] page         Results page number
 * @return    New string with url to free with free(), null when the page is
 *            beyond the results the service gives access to
 */
static char *
artic_search_url (const char *query,
                  [[maybe_unused]] const char *search_opts,
                  [[maybe_unused]] const char *api_key,
                  const int   page)
{
    char *escaped = nullptr; /* Percent-encoded query */
    char *url     = nullptr; /* Result url */

    if (page < 1 || page * WEB_PAGE_SIZE > ARTIC_MAX_RESULTS)
        return nullptr;

    escaped = urldata_escape (query);
    url = g_strdup_printf (
            "https://api.artic.edu/api/v1/artworks/search?q=%s"
            "&query%%5Bterm%%5D%%5Bis_public_domain%%5D=true"
            "&fields=id,title,artist_title,date_display,image_id,thumbnail"
            "&page=%d&limit=%d",
            escaped, page, WEB_PAGE_SIZE);
    free (escaped);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert an artwork json object to SearchItem item.
 *
 * @param[in] obj          Json object to convert
 * @param[in] iiif_url     Address of the image server
 * @param[in] website_url  Address of the museum website
 * @return    SearchItem item, null when the artwork has no image
 */
static SearchItem *
artic_json_obj_to_search_item (json_object *obj,
                               const char  *iiif_url,
                               const char  *website_url)
{
    SearchItem  *item     = nullptr;
    json_object *thumb    = web_json_get (obj, "thumbnail", json_type_object);
    const char  *image_id = web_json_str (obj, "image_id");
    const char  *title    = web_json_str (obj, "title");
    const char  *date     = web_json_str (obj, "date_display");
    char        *text     = nullptr; /* Title with date and built urls */
    int          art_id   = web_json_int (obj, "id");
    int          width    = web_json_int (thumb, "width");
    int          height   = web_json_int (thumb, "height");
    int          req_w    = ARTIC_MAX_WIDTH; /* Width of image to request */
    char         id[16];

    if (art_id <= 0 || image_id == nullptr)
        return nullptr;

    /* The size in the response is the size of the original image. The server
     * sends images up to ARTIC_MAX_WIDTH wide, so a larger one is downloaded
     * scaled down and a smaller one in its own size. */
    if (width > 0 && width < ARTIC_MAX_WIDTH) {
        req_w = width;
    }
    else if (width > 0) {
        height = (int) ((long long) height * ARTIC_MAX_WIDTH / width);
        width  = ARTIC_MAX_WIDTH;
    }
    snprintf (id, sizeof (id), "%d", art_id);

    item = search_item_new ();
    search_item_set_id_string    (item, id);
    search_item_set_author_name  (item, web_json_str (obj, "artist_title"));
    search_item_set_license_name (item, "CC0 1.0");
    search_item_set_license_url  (item,
            "https://creativecommons.org/publicdomain/zero/1.0/");
    search_item_set_width        (item, width);
    search_item_set_height       (item, height);

    /* The museum asks for a caption with the artist, title and date */
    if (title != nullptr && date != nullptr) {
        text = g_strdup_printf ("%s, %s", title, date);
        search_item_set_title (item, text);
        g_free (text);
    }
    else {
        search_item_set_title (item, title);
    }
    text = g_strdup_printf ("%s/%s/full/%d,/0/default.jpg",
                            iiif_url, image_id, req_w);
    search_item_set_image_url (item, text);
    g_free (text);

    text = g_strdup_printf ("%s/%s/full/%d,/0/default.jpg",
                            iiif_url, image_id, ARTIC_THUMB_WIDTH);
    search_item_set_thumb_url (item, text);
    g_free (text);

    text = g_strdup_printf ("%s/artworks/%d", website_url, art_id);
    search_item_set_page_url (item, text);
    g_free (text);

    search_item_set_names (item, artic_provider.name, ".jpg");

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Art Institute of Chicago search response.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found images
 * @param[in]  add_item   Function to pass found images to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not a search response of the service
 */
static bool
artic_parse (const char  *json,
             int         *found_cnt,
             WebItemFunc  add_item,
             void        *user_data)
{
    json_object *obj     = web_json_parse (json);
    json_object *arr     = web_json_get (obj, "data", json_type_array);
    json_object *config  = web_json_get (obj, "config", json_type_object);
    const char  *iiif    = web_json_str (config, "iiif_url");
    const char  *website = web_json_str (config, "website_url");
    SearchItem  *item    = nullptr;
    size_t       cnt     = 0;

    if (arr == nullptr) {
        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (web_json_get (obj, "pagination",
                                             json_type_object), "total");
    cnt = json_object_array_length (arr);

    for (size_t i = 0; i < cnt; ++i) {
        item = artic_json_obj_to_search_item (
                web_json_idx (arr, i),
                iiif    != nullptr ? iiif    : ARTIC_IIIF_URL,
                website != nullptr ? website : ARTIC_WEBSITE_URL);
        if (item != nullptr)
            add_item (item, user_data);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @var   artic_provider
 * @brief Art Institute of Chicago image search service.
 */
const WebProvider artic_provider = {
    .name       = "ArtIC",
    .label      = "Art Institute of Chicago",
    .opts_name  = "ArtIC_opts",
    .logo       = W_IMG_COUNT,
    .search_url = artic_search_url,
    .parse      = artic_parse
};
/*----------------------------------------------------------------------------*/
